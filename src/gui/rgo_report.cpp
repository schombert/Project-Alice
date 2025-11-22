namespace alice_ui {
struct rgo_report_body_header_t;
struct rgo_report_body_t;
struct rgo_report_list_item_content_t;
struct rgo_report_list_item_t;
struct rgo_report_list_header_content_t;
struct rgo_report_list_header_t;
struct rgo_report_body_header_t : public alice_ui::template_label {
// BEGIN body::header::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct rgo_report_body_rgos_t : public layout_generator {
// BEGIN body::rgos::variables
// END
	struct list_item_option { dcon::commodity_id cid; };
	std::vector<std::unique_ptr<ui::element_base>> list_item_pool;
	int32_t list_item_pool_used = 0;
	void add_list_item( dcon::commodity_id cid);
	std::vector<std::unique_ptr<ui::element_base>> list_header_pool;
	int32_t list_header_pool_used = 0;
	std::vector<std::variant<std::monostate, list_item_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct rgo_report_list_item_content_t : public ui::element_base {
// BEGIN list_item::content::variables
// END
	int32_t template_id = -1;
	text::layout commodity_internal_layout;
	int32_t  commodity_text_color = 0;
	std::string commodity_cached_text;
	void set_commodity_text(sys::state & state, std::string const& new_text);
	text::layout employment_internal_layout;
	int32_t  employment_text_color = 0;
	std::string employment_cached_text;
	void set_employment_text(sys::state & state, std::string const& new_text);
	text::layout max_employment_internal_layout;
	int32_t  max_employment_text_color = 0;
	std::string max_employment_cached_text;
	void set_max_employment_text(sys::state & state, std::string const& new_text);
	text::layout output_internal_layout;
	int32_t  output_text_color = 0;
	std::string output_cached_text;
	void set_output_text(sys::state & state, std::string const& new_text);
	text::layout profit_internal_layout;
	int32_t  profit_text_color = 0;
	std::string profit_cached_text;
	void set_profit_text(sys::state & state, std::string const& new_text);
	text::layout wage_internal_layout;
	int32_t  wage_text_color = 0;
	std::string wage_cached_text;
	void set_wage_text(sys::state & state, std::string const& new_text);
	text::layout eff_spend_internal_layout;
	int32_t  eff_spend_text_color = 0;
	std::string eff_spend_cached_text;
	void set_eff_spend_text(sys::state & state, std::string const& new_text);
	text::layout eff_internal_layout;
	int32_t  eff_text_color = 0;
	std::string eff_cached_text;
	void set_eff_text(sys::state & state, std::string const& new_text);
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
struct rgo_report_list_header_content_t : public ui::element_base {
// BEGIN list_header::content::variables
// END
	int32_t template_id = -1;
	text::layout commodity_internal_layout;
	std::string commodity_cached_text;
	text::layout employment_internal_layout;
	std::string employment_cached_text;
	text::layout max_employment_internal_layout;
	std::string max_employment_cached_text;
	text::layout output_internal_layout;
	std::string output_cached_text;
	text::layout profit_internal_layout;
	std::string profit_cached_text;
	text::layout wage_internal_layout;
	std::string wage_cached_text;
	text::layout eff_spend_internal_layout;
	std::string eff_spend_cached_text;
	text::layout eff_internal_layout;
	std::string eff_cached_text;
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
struct rgo_report_body_t : public layout_window_element {
// BEGIN body::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<rgo_report_body_header_t> header;
	std::unique_ptr<template_label> employment_header;
	std::unique_ptr<template_label> efficiency_header;
	rgo_report_body_rgos_t rgos;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	std::string_view list_commodity_header_text_key;
	uint8_t list_commodity_header_text_color = 0;
	uint8_t list_commodity_column_text_color = 0;
	text::alignment list_commodity_text_alignment = text::alignment::center;
	int16_t list_commodity_column_start = 0;
	int16_t list_commodity_column_width = 0;
	std::string_view list_employment_header_text_key;
	uint8_t list_employment_header_text_color = 0;
	uint8_t list_employment_column_text_color = 0;
	text::alignment list_employment_text_alignment = text::alignment::center;
	int8_t list_employment_sort_direction = 0;
	int16_t list_employment_column_start = 0;
	int16_t list_employment_column_width = 0;
	std::string_view list_max_employment_header_text_key;
	uint8_t list_max_employment_header_text_color = 0;
	uint8_t list_max_employment_column_text_color = 0;
	text::alignment list_max_employment_text_alignment = text::alignment::center;
	int8_t list_max_employment_sort_direction = 0;
	int16_t list_max_employment_column_start = 0;
	int16_t list_max_employment_column_width = 0;
	std::string_view list_output_header_text_key;
	uint8_t list_output_header_text_color = 0;
	uint8_t list_output_column_text_color = 0;
	text::alignment list_output_text_alignment = text::alignment::center;
	int8_t list_output_sort_direction = 0;
	int16_t list_output_column_start = 0;
	int16_t list_output_column_width = 0;
	std::string_view list_profit_header_text_key;
	uint8_t list_profit_header_text_color = 0;
	uint8_t list_profit_column_text_color = 0;
	text::alignment list_profit_text_alignment = text::alignment::center;
	int8_t list_profit_sort_direction = 0;
	int16_t list_profit_column_start = 0;
	int16_t list_profit_column_width = 0;
	std::string_view list_wage_header_text_key;
	uint8_t list_wage_header_text_color = 0;
	uint8_t list_wage_column_text_color = 0;
	text::alignment list_wage_text_alignment = text::alignment::center;
	int8_t list_wage_sort_direction = 0;
	int16_t list_wage_column_start = 0;
	int16_t list_wage_column_width = 0;
	std::string_view list_eff_spend_header_text_key;
	uint8_t list_eff_spend_header_text_color = 0;
	uint8_t list_eff_spend_column_text_color = 0;
	text::alignment list_eff_spend_text_alignment = text::alignment::center;
	int8_t list_eff_spend_sort_direction = 0;
	int16_t list_eff_spend_column_start = 0;
	int16_t list_eff_spend_column_width = 0;
	std::string_view list_eff_header_text_key;
	uint8_t list_eff_header_text_color = 0;
	uint8_t list_eff_column_text_color = 0;
	text::alignment list_eff_text_alignment = text::alignment::center;
	int8_t list_eff_sort_direction = 0;
	int16_t list_eff_column_start = 0;
	int16_t list_eff_column_width = 0;
	std::string_view list_ascending_icon_key;
	dcon::texture_id list_ascending_icon;
	std::string_view list_descending_icon_key;
	dcon::texture_id list_descending_icon;
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
std::unique_ptr<ui::element_base> make_rgo_report_body(sys::state& state);
struct rgo_report_list_item_t : public layout_window_element {
// BEGIN list_item::variables
// END
	dcon::commodity_id cid;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<rgo_report_list_item_content_t> content;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "cid") {
			return (void*)(&cid);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_rgo_report_list_item(sys::state& state);
struct rgo_report_list_header_t : public layout_window_element {
// BEGIN list_header::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<rgo_report_list_header_content_t> content;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		return ui::message_result::unseen;
	}
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_rgo_report_list_header(sys::state& state);
void rgo_report_body_rgos_t::add_list_item(dcon::commodity_id cid) {
	values.emplace_back(list_item_option{cid});
}
void  rgo_report_body_rgos_t::on_create(sys::state& state, layout_window_element* parent) {
	rgo_report_body_t& body = *((rgo_report_body_t*)(parent)); 
// BEGIN body::rgos::on_create
// END
}
void  rgo_report_body_rgos_t::update(sys::state& state, layout_window_element* parent) {
	rgo_report_body_t& body = *((rgo_report_body_t*)(parent)); 
// BEGIN body::rgos::update
	auto pid = state.map_state.selected_province;

	values.clear();

	state.world.for_each_commodity([&](auto cid) {
		if(state.world.province_get_rgo_size(pid, cid) > 0) {
			add_list_item(cid);
		}
	});
// END
	{
	bool work_to_do = false;
	auto table_source = (rgo_report_body_t*)(parent);
	if(table_source->list_employment_sort_direction != 0) work_to_do = true;
	if(table_source->list_max_employment_sort_direction != 0) work_to_do = true;
	if(table_source->list_output_sort_direction != 0) work_to_do = true;
	if(table_source->list_profit_sort_direction != 0) work_to_do = true;
	if(table_source->list_wage_sort_direction != 0) work_to_do = true;
	if(table_source->list_eff_spend_sort_direction != 0) work_to_do = true;
	if(table_source->list_eff_sort_direction != 0) work_to_do = true;
	if(work_to_do) {
		for(size_t i = 0; i < values.size(); ) {
			if(std::holds_alternative<list_item_option>(values[i])) {
				auto start_i = i;
				while(i < values.size() && std::holds_alternative<list_item_option>(values[i])) ++i;
				if(table_source->list_employment_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<list_item_option>(raw_a);
						auto const& b = std::get<list_item_option>(raw_b);
						int8_t result = 0;
// BEGIN body::rgos::list::sort::employment
						auto value_a = economy::rgo_employment(state, a.cid, pid);
						auto value_b = economy::rgo_employment(state, b.cid, pid);
						result = cmp3(value_a, value_b);
// END
						return -result == table_source->list_employment_sort_direction;
					});
				}
				if(table_source->list_max_employment_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<list_item_option>(raw_a);
						auto const& b = std::get<list_item_option>(raw_b);
						int8_t result = 0;
// BEGIN body::rgos::list::sort::max_employment
						auto value_a = economy::rgo_max_employment(state, a.cid, pid);
						auto value_b = economy::rgo_max_employment(state, b.cid, pid);
						result = cmp3(value_a, value_b);
// END
						return -result == table_source->list_max_employment_sort_direction;
					});
				}
				if(table_source->list_output_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<list_item_option>(raw_a);
						auto const& b = std::get<list_item_option>(raw_b);
						int8_t result = 0;
// BEGIN body::rgos::list::sort::output
						auto value_a = economy::rgo_output(state, a.cid, pid);
						auto value_b = economy::rgo_output(state, b.cid, pid);
						result = cmp3(value_a, value_b);
// END
						return -result == table_source->list_output_sort_direction;
					});
				}
				if(table_source->list_profit_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<list_item_option>(raw_a);
						auto const& b = std::get<list_item_option>(raw_b);
						int8_t result = 0;
// BEGIN body::rgos::list::sort::profit
						auto value_a = economy::rgo_income(state, a.cid, pid);
						auto value_b = economy::rgo_income(state, b.cid, pid);
						result = cmp3(value_a, value_b);
// END
						return -result == table_source->list_profit_sort_direction;
					});
				}
				if(table_source->list_wage_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<list_item_option>(raw_a);
						auto const& b = std::get<list_item_option>(raw_b);
						int8_t result = 0;
// BEGIN body::rgos::list::sort::wage
						auto value_a = economy::rgo_wage(state, a.cid, pid);
						auto value_b = economy::rgo_wage(state, b.cid, pid);
						result = cmp3(value_a, value_b);
// END
						return -result == table_source->list_wage_sort_direction;
					});
				}
				if(table_source->list_eff_spend_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<list_item_option>(raw_a);
						auto const& b = std::get<list_item_option>(raw_b);
						int8_t result = 0;
// BEGIN body::rgos::list::sort::eff_spend
						auto value_a = economy::rgo_efficiency_spending(state, a.cid, pid);
						auto value_b = economy::rgo_efficiency_spending(state, b.cid, pid);
						result = cmp3(value_a, value_b);
// END
						return -result == table_source->list_eff_spend_sort_direction;
					});
				}
				if(table_source->list_eff_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<list_item_option>(raw_a);
						auto const& b = std::get<list_item_option>(raw_b);
						int8_t result = 0;
// BEGIN body::rgos::list::sort::eff
						auto value_a = state.world.province_get_rgo_efficiency(pid, a.cid);
						auto value_b = state.world.province_get_rgo_efficiency(pid, b.cid);
						result = cmp3(value_a, value_b);
// END
						return -result == table_source->list_eff_sort_direction;
					});
				}
			} else {
				++i;
			}
		}
	}
	}
}
measure_result  rgo_report_body_rgos_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<list_item_option>(values[index])) {
		if(list_header_pool.empty()) list_header_pool.emplace_back(make_rgo_report_list_header(state));
		if(list_item_pool.empty()) list_item_pool.emplace_back(make_rgo_report_list_item(state));
		if(index == 0 || first_in_section || (true && !std::holds_alternative<list_item_option>(values[index - 1]))) {
			if(destination) {
				if(list_header_pool.size() <= size_t(list_header_pool_used)) list_header_pool.emplace_back(make_rgo_report_list_header(state));
				if(list_item_pool.size() <= size_t(list_item_pool_used)) list_item_pool.emplace_back(make_rgo_report_list_item(state));
				list_header_pool[list_header_pool_used]->base_data.position.x = int16_t(x);
				list_header_pool[list_header_pool_used]->base_data.position.y = int16_t(y);
				if(!list_header_pool[list_header_pool_used]->parent) {
					list_header_pool[list_header_pool_used]->parent = destination;
					list_header_pool[list_header_pool_used]->impl_on_update(state);
					list_header_pool[list_header_pool_used]->impl_on_reset_text(state);
				}
				destination->children.push_back(list_header_pool[list_header_pool_used].get());
				list_item_pool[list_item_pool_used]->base_data.position.x = int16_t(x);
				list_item_pool[list_item_pool_used]->base_data.position.y = int16_t(y +  list_item_pool[0]->base_data.size.y + 0);
				list_item_pool[list_item_pool_used]->parent = destination;
				destination->children.push_back(list_item_pool[list_item_pool_used].get());
				((rgo_report_list_item_t*)(list_item_pool[list_item_pool_used].get()))->cid = std::get<list_item_option>(values[index]).cid;
			((rgo_report_list_item_t*)(list_item_pool[list_item_pool_used].get()))->set_alternate(!alternate);
				list_item_pool[list_item_pool_used]->impl_on_update(state);
				list_header_pool_used++;
				list_item_pool_used++;
			}
			return measure_result{std::max(list_header_pool[0]->base_data.size.x, list_item_pool[0]->base_data.size.x), list_header_pool[0]->base_data.size.y + list_item_pool[0]->base_data.size.y + 0, measure_result::special::none};
		}
		if(destination) {
			if(list_item_pool.size() <= size_t(list_item_pool_used)) list_item_pool.emplace_back(make_rgo_report_list_item(state));
			list_item_pool[list_item_pool_used]->base_data.position.x = int16_t(x);
			list_item_pool[list_item_pool_used]->base_data.position.y = int16_t(y);
			list_item_pool[list_item_pool_used]->parent = destination;
			destination->children.push_back(list_item_pool[list_item_pool_used].get());
			((rgo_report_list_item_t*)(list_item_pool[list_item_pool_used].get()))->cid = std::get<list_item_option>(values[index]).cid;
			((rgo_report_list_item_t*)(list_item_pool[list_item_pool_used].get()))->set_alternate(alternate);
			list_item_pool[list_item_pool_used]->impl_on_update(state);
			list_item_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ list_item_pool[0]->base_data.size.x, list_item_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  rgo_report_body_rgos_t::reset_pools() {
	list_header_pool_used = 0;
	list_item_pool_used = 0;
}
void rgo_report_body_header_t::on_update(sys::state& state) noexcept {
	rgo_report_body_t& body = *((rgo_report_body_t*)(parent)); 
// BEGIN body::header::update
	text::substitution_map m;
	text::add_to_substitution_map(m, text::variable_type::province, state.map_state.selected_province);
	auto result = text::resolve_string_substitution(state, "rgo_report", m);
	set_text(state, result);
// END
}
ui::message_result rgo_report_body_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	state.ui_state.drag_target = this;
	return ui::message_result::consumed;
}
ui::message_result rgo_report_body_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void rgo_report_body_t::on_update(sys::state& state) noexcept {
// BEGIN body::update
// END
	rgos.update(state, this);
	remake_layout(state, true);
}
void rgo_report_body_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "header") {
					temp.ptr = header.get();
				} else
				if(cname == "employment_header") {
					temp.ptr = employment_header.get();
				} else
				if(cname == "efficiency_header") {
					temp.ptr = efficiency_header.get();
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
				if(cname == "body") {
					temp.ptr = make_rgo_report_body(state);
				}
				if(cname == "list_item") {
					temp.ptr = make_rgo_report_list_item(state);
				}
				if(cname == "list_header") {
					temp.ptr = make_rgo_report_list_header(state);
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
				if(cname == "rgos") {
					temp.generator = &rgos;
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
void rgo_report_body_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("rgo_report::body"));
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
			header = std::make_unique<rgo_report_body_header_t>();
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
		if(child_data.name == "efficiency_header") {
			efficiency_header = std::make_unique<template_label>();
			efficiency_header->parent = this;
			auto cptr = efficiency_header.get();
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
		if(child_data.name == ".tablist") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			list_ascending_icon_key = main_section.read<std::string_view>();
			list_descending_icon_key = main_section.read<std::string_view>();
			main_section.read<ogl::color3f>();
			auto col_section = tbuffer.read_section();
			list_commodity_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			list_commodity_column_start = running_w_total;
			col_section.read(list_commodity_column_width);
			running_w_total += list_commodity_column_width;
			col_section.read(list_commodity_column_text_color);
			col_section.read(list_commodity_header_text_color);
			col_section.read(list_commodity_text_alignment);
			list_employment_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			list_employment_column_start = running_w_total;
			col_section.read(list_employment_column_width);
			running_w_total += list_employment_column_width;
			col_section.read(list_employment_column_text_color);
			col_section.read(list_employment_header_text_color);
			col_section.read(list_employment_text_alignment);
			list_max_employment_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			list_max_employment_column_start = running_w_total;
			col_section.read(list_max_employment_column_width);
			running_w_total += list_max_employment_column_width;
			col_section.read(list_max_employment_column_text_color);
			col_section.read(list_max_employment_header_text_color);
			col_section.read(list_max_employment_text_alignment);
			list_output_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			list_output_column_start = running_w_total;
			col_section.read(list_output_column_width);
			running_w_total += list_output_column_width;
			col_section.read(list_output_column_text_color);
			col_section.read(list_output_header_text_color);
			col_section.read(list_output_text_alignment);
			list_profit_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			list_profit_column_start = running_w_total;
			col_section.read(list_profit_column_width);
			running_w_total += list_profit_column_width;
			col_section.read(list_profit_column_text_color);
			col_section.read(list_profit_header_text_color);
			col_section.read(list_profit_text_alignment);
			list_wage_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			list_wage_column_start = running_w_total;
			col_section.read(list_wage_column_width);
			running_w_total += list_wage_column_width;
			col_section.read(list_wage_column_text_color);
			col_section.read(list_wage_header_text_color);
			col_section.read(list_wage_text_alignment);
			list_eff_spend_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			list_eff_spend_column_start = running_w_total;
			col_section.read(list_eff_spend_column_width);
			running_w_total += list_eff_spend_column_width;
			col_section.read(list_eff_spend_column_text_color);
			col_section.read(list_eff_spend_header_text_color);
			col_section.read(list_eff_spend_text_alignment);
			list_eff_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			list_eff_column_start = running_w_total;
			col_section.read(list_eff_column_width);
			running_w_total += list_eff_column_width;
			col_section.read(list_eff_column_text_color);
			col_section.read(list_eff_header_text_color);
			col_section.read(list_eff_text_alignment);
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
	rgos.on_create(state, this);
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN body::create
// END
}
std::unique_ptr<ui::element_base> make_rgo_report_body(sys::state& state) {
	auto ptr = std::make_unique<rgo_report_body_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result rgo_report_list_item_content_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result rgo_report_list_item_content_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void rgo_report_list_item_content_t::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept {
	auto table_source = (rgo_report_body_t*)(parent->parent);
	if(x >= table_source->list_commodity_column_start && x < table_source->list_commodity_column_start + table_source->list_commodity_column_width) {
	}
	if(x >= table_source->list_employment_column_start && x < table_source->list_employment_column_start + table_source->list_employment_column_width) {
	}
	if(x >= table_source->list_max_employment_column_start && x < table_source->list_max_employment_column_start + table_source->list_max_employment_column_width) {
	}
	if(x >= table_source->list_output_column_start && x < table_source->list_output_column_start + table_source->list_output_column_width) {
	}
	if(x >= table_source->list_profit_column_start && x < table_source->list_profit_column_start + table_source->list_profit_column_width) {
	}
	if(x >= table_source->list_wage_column_start && x < table_source->list_wage_column_start + table_source->list_wage_column_width) {
	}
	if(x >= table_source->list_eff_spend_column_start && x < table_source->list_eff_spend_column_start + table_source->list_eff_spend_column_width) {
	}
	if(x >= table_source->list_eff_column_start && x < table_source->list_eff_column_start + table_source->list_eff_column_width) {
	}
		ident = -1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
}
void rgo_report_list_item_content_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto table_source = (rgo_report_body_t*)(parent->parent);
	if(x >=  table_source->list_commodity_column_start && x <  table_source->list_commodity_column_start +  table_source->list_commodity_column_width) {
	}
	if(x >=  table_source->list_employment_column_start && x <  table_source->list_employment_column_start +  table_source->list_employment_column_width) {
	}
	if(x >=  table_source->list_max_employment_column_start && x <  table_source->list_max_employment_column_start +  table_source->list_max_employment_column_width) {
	}
	if(x >=  table_source->list_output_column_start && x <  table_source->list_output_column_start +  table_source->list_output_column_width) {
	}
	if(x >=  table_source->list_profit_column_start && x <  table_source->list_profit_column_start +  table_source->list_profit_column_width) {
	}
	if(x >=  table_source->list_wage_column_start && x <  table_source->list_wage_column_start +  table_source->list_wage_column_width) {
	}
	if(x >=  table_source->list_eff_spend_column_start && x <  table_source->list_eff_spend_column_start +  table_source->list_eff_spend_column_width) {
	}
	if(x >=  table_source->list_eff_column_start && x <  table_source->list_eff_column_start +  table_source->list_eff_column_width) {
	}
}
void rgo_report_list_item_content_t::set_commodity_text(sys::state & state, std::string const& new_text) {
		auto table_source = (rgo_report_body_t*)(parent->parent);
	if(new_text !=  commodity_cached_text) {
		commodity_cached_text = new_text;
		commodity_internal_layout.contents.clear();
		commodity_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ commodity_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->list_commodity_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->list_commodity_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, commodity_cached_text);
		}
	} else {
	}
}
void rgo_report_list_item_content_t::set_employment_text(sys::state & state, std::string const& new_text) {
		auto table_source = (rgo_report_body_t*)(parent->parent);
	if(new_text !=  employment_cached_text) {
		employment_cached_text = new_text;
		employment_internal_layout.contents.clear();
		employment_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ employment_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->list_employment_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->list_employment_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, employment_cached_text);
		}
	} else {
	}
}
void rgo_report_list_item_content_t::set_max_employment_text(sys::state & state, std::string const& new_text) {
		auto table_source = (rgo_report_body_t*)(parent->parent);
	if(new_text !=  max_employment_cached_text) {
		max_employment_cached_text = new_text;
		max_employment_internal_layout.contents.clear();
		max_employment_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ max_employment_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->list_max_employment_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->list_max_employment_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, max_employment_cached_text);
		}
	} else {
	}
}
void rgo_report_list_item_content_t::set_output_text(sys::state & state, std::string const& new_text) {
		auto table_source = (rgo_report_body_t*)(parent->parent);
	if(new_text !=  output_cached_text) {
		output_cached_text = new_text;
		output_internal_layout.contents.clear();
		output_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ output_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->list_output_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->list_output_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, output_cached_text);
		}
	} else {
	}
}
void rgo_report_list_item_content_t::set_profit_text(sys::state & state, std::string const& new_text) {
		auto table_source = (rgo_report_body_t*)(parent->parent);
	if(new_text !=  profit_cached_text) {
		profit_cached_text = new_text;
		profit_internal_layout.contents.clear();
		profit_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ profit_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->list_profit_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->list_profit_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, profit_cached_text);
		}
	} else {
	}
}
void rgo_report_list_item_content_t::set_wage_text(sys::state & state, std::string const& new_text) {
		auto table_source = (rgo_report_body_t*)(parent->parent);
	if(new_text !=  wage_cached_text) {
		wage_cached_text = new_text;
		wage_internal_layout.contents.clear();
		wage_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ wage_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->list_wage_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->list_wage_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, wage_cached_text);
		}
	} else {
	}
}
void rgo_report_list_item_content_t::set_eff_spend_text(sys::state & state, std::string const& new_text) {
		auto table_source = (rgo_report_body_t*)(parent->parent);
	if(new_text !=  eff_spend_cached_text) {
		eff_spend_cached_text = new_text;
		eff_spend_internal_layout.contents.clear();
		eff_spend_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ eff_spend_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->list_eff_spend_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->list_eff_spend_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, eff_spend_cached_text);
		}
	} else {
	}
}
void rgo_report_list_item_content_t::set_eff_text(sys::state & state, std::string const& new_text) {
		auto table_source = (rgo_report_body_t*)(parent->parent);
	if(new_text !=  eff_cached_text) {
		eff_cached_text = new_text;
		eff_internal_layout.contents.clear();
		eff_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ eff_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->list_eff_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->list_eff_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, eff_cached_text);
		}
	} else {
	}
}
void rgo_report_list_item_content_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto fh = text::make_font_id(state, false, 1.0f * 16);
	auto linesz = state.font_collection.line_height(state, fh); 
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto table_source = (rgo_report_body_t*)(parent->parent);
	auto abs_location = ui::get_absolute_location(state, *this);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - abs_location.x;
	int32_t rel_mouse_y = int32_t(state.mouse_y_position / state.user_settings.ui_scale) - abs_location.y;
	auto ink_color =template_id != -1 ? ogl::color3f(state.ui_templates.colors[state.ui_templates.table_t[template_id].table_color]) : ogl::color3f{}; 	bool col_um_commodity = rel_mouse_x >= table_source->list_commodity_column_start && rel_mouse_x < (table_source->list_commodity_column_start + table_source->list_commodity_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_commodity){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_commodity_column_start + table_source->list_commodity_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_commodity_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_commodity_column_start + table_source->list_commodity_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_commodity_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_commodity_column_start + table_source->list_commodity_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->list_commodity_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_commodity){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_commodity_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_commodity_column_start + table_source->list_commodity_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_commodity){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_commodity_column_start), float(y), float(table_source->list_commodity_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_commodity_column_start), float(y + base_data.size.y - 2), float(table_source->list_commodity_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_commodity = state.ui_templates.colors[commodity_text_color]; 	if(!commodity_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : commodity_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->list_commodity_column_start + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_commodity.r, col_color_commodity.g, col_color_commodity.b }, ogl::color_modification::none);
		}
	}
	bool col_um_employment = rel_mouse_x >= table_source->list_employment_column_start && rel_mouse_x < (table_source->list_employment_column_start + table_source->list_employment_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_employment){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_employment_column_start + table_source->list_employment_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_employment_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_employment_column_start + table_source->list_employment_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_employment_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_employment_column_start + table_source->list_employment_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->list_employment_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_employment){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_employment_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_employment_column_start + table_source->list_employment_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_employment){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_employment_column_start), float(y), float(table_source->list_employment_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_employment_column_start), float(y + base_data.size.y - 2), float(table_source->list_employment_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_employment = state.ui_templates.colors[employment_text_color]; 	if(!employment_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : employment_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->list_employment_column_start + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_employment.r, col_color_employment.g, col_color_employment.b }, ogl::color_modification::none);
		}
	}
	bool col_um_max_employment = rel_mouse_x >= table_source->list_max_employment_column_start && rel_mouse_x < (table_source->list_max_employment_column_start + table_source->list_max_employment_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_max_employment){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_max_employment_column_start + table_source->list_max_employment_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_max_employment_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_max_employment_column_start + table_source->list_max_employment_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_max_employment_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_max_employment_column_start + table_source->list_max_employment_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->list_max_employment_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_max_employment){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_max_employment_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_max_employment_column_start + table_source->list_max_employment_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_max_employment){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_max_employment_column_start), float(y), float(table_source->list_max_employment_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_max_employment_column_start), float(y + base_data.size.y - 2), float(table_source->list_max_employment_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_max_employment = state.ui_templates.colors[max_employment_text_color]; 	if(!max_employment_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : max_employment_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->list_max_employment_column_start + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_max_employment.r, col_color_max_employment.g, col_color_max_employment.b }, ogl::color_modification::none);
		}
	}
	bool col_um_output = rel_mouse_x >= table_source->list_output_column_start && rel_mouse_x < (table_source->list_output_column_start + table_source->list_output_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_output){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_output_column_start + table_source->list_output_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_output_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_output_column_start + table_source->list_output_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_output_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_output_column_start + table_source->list_output_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->list_output_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_output){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_output_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_output_column_start + table_source->list_output_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_output){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_output_column_start), float(y), float(table_source->list_output_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_output_column_start), float(y + base_data.size.y - 2), float(table_source->list_output_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_output = state.ui_templates.colors[output_text_color]; 	if(!output_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : output_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->list_output_column_start + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_output.r, col_color_output.g, col_color_output.b }, ogl::color_modification::none);
		}
	}
	bool col_um_profit = rel_mouse_x >= table_source->list_profit_column_start && rel_mouse_x < (table_source->list_profit_column_start + table_source->list_profit_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_profit){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_profit_column_start + table_source->list_profit_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_profit_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_profit_column_start + table_source->list_profit_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_profit_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_profit_column_start + table_source->list_profit_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->list_profit_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_profit){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_profit_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_profit_column_start + table_source->list_profit_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_profit){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_profit_column_start), float(y), float(table_source->list_profit_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_profit_column_start), float(y + base_data.size.y - 2), float(table_source->list_profit_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_profit = state.ui_templates.colors[profit_text_color]; 	if(!profit_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : profit_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->list_profit_column_start + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_profit.r, col_color_profit.g, col_color_profit.b }, ogl::color_modification::none);
		}
	}
	bool col_um_wage = rel_mouse_x >= table_source->list_wage_column_start && rel_mouse_x < (table_source->list_wage_column_start + table_source->list_wage_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_wage){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_wage_column_start + table_source->list_wage_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_wage_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_wage_column_start + table_source->list_wage_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_wage_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_wage_column_start + table_source->list_wage_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->list_wage_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_wage){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_wage_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_wage_column_start + table_source->list_wage_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_wage){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_wage_column_start), float(y), float(table_source->list_wage_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_wage_column_start), float(y + base_data.size.y - 2), float(table_source->list_wage_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_wage = state.ui_templates.colors[wage_text_color]; 	if(!wage_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : wage_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->list_wage_column_start + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_wage.r, col_color_wage.g, col_color_wage.b }, ogl::color_modification::none);
		}
	}
	bool col_um_eff_spend = rel_mouse_x >= table_source->list_eff_spend_column_start && rel_mouse_x < (table_source->list_eff_spend_column_start + table_source->list_eff_spend_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_eff_spend){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_spend_column_start + table_source->list_eff_spend_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_spend_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_spend_column_start + table_source->list_eff_spend_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_spend_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_spend_column_start + table_source->list_eff_spend_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->list_eff_spend_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_eff_spend){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_spend_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_spend_column_start + table_source->list_eff_spend_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_eff_spend){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_spend_column_start), float(y), float(table_source->list_eff_spend_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_spend_column_start), float(y + base_data.size.y - 2), float(table_source->list_eff_spend_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_eff_spend = state.ui_templates.colors[eff_spend_text_color]; 	if(!eff_spend_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : eff_spend_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->list_eff_spend_column_start + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_eff_spend.r, col_color_eff_spend.g, col_color_eff_spend.b }, ogl::color_modification::none);
		}
	}
	bool col_um_eff = rel_mouse_x >= table_source->list_eff_column_start && rel_mouse_x < (table_source->list_eff_column_start + table_source->list_eff_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_eff){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_column_start + table_source->list_eff_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_column_start + table_source->list_eff_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_column_start + table_source->list_eff_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->list_eff_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_eff){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_column_start + table_source->list_eff_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_eff){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_column_start), float(y), float(table_source->list_eff_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_column_start), float(y + base_data.size.y - 2), float(table_source->list_eff_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_eff = state.ui_templates.colors[eff_text_color]; 	if(!eff_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : eff_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->list_eff_column_start + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_eff.r, col_color_eff.g, col_color_eff.b }, ogl::color_modification::none);
		}
	}
}
void rgo_report_list_item_content_t::on_update(sys::state& state) noexcept {
	rgo_report_list_item_t& list_item = *((rgo_report_list_item_t*)(parent)); 
	rgo_report_body_t& body = *((rgo_report_body_t*)(parent->parent)); 
// BEGIN list_item::content::update
	auto pid = state.map_state.selected_province;
	set_commodity_text(state, text::get_commodity_name_with_icon(state, list_item.cid));
	set_employment_text(state, text::prettify((int64_t)(economy::rgo_employment(state, list_item.cid, pid))));
	set_max_employment_text(state, text::prettify((int64_t)(economy::rgo_max_employment(state, list_item.cid, pid))));
	set_profit_text(state, text::format_money(economy::rgo_income(state, list_item.cid, pid)));
	set_wage_text(state, text::format_money(economy::rgo_wage(state, list_item.cid, pid)));
	set_eff_text(state, text::format_percentage(state.world.province_get_rgo_efficiency(pid, list_item.cid)));
	set_eff_spend_text(state, text::format_money(economy::rgo_efficiency_spending(state, list_item.cid, pid)));
	set_output_text(state, text::format_float(economy::rgo_output(state, list_item.cid, pid), 2));
// END
}
void rgo_report_list_item_content_t::on_create(sys::state& state) noexcept {
// BEGIN list_item::content::create
// END
}
void  rgo_report_list_item_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 4 : 3;
}
ui::message_result rgo_report_list_item_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result rgo_report_list_item_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void rgo_report_list_item_t::on_update(sys::state& state) noexcept {
	rgo_report_body_t& body = *((rgo_report_body_t*)(parent->parent)); 
// BEGIN list_item::update
// END
	remake_layout(state, true);
}
void rgo_report_list_item_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "body") {
					temp.ptr = make_rgo_report_body(state);
				}
				if(cname == "list_item") {
					temp.ptr = make_rgo_report_list_item(state);
				}
				if(cname == "list_header") {
					temp.ptr = make_rgo_report_list_header(state);
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
void rgo_report_list_item_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("rgo_report::list_item"));
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
			content = std::make_unique<rgo_report_list_item_content_t>();
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
// BEGIN list_item::create
// END
}
std::unique_ptr<ui::element_base> make_rgo_report_list_item(sys::state& state) {
	auto ptr = std::make_unique<rgo_report_list_item_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result rgo_report_list_header_content_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	auto table_source = (rgo_report_body_t*)(parent->parent);
	if(x >= table_source->list_employment_column_start && x < table_source->list_employment_column_start + table_source->list_employment_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->list_employment_sort_direction;
		table_source->list_employment_sort_direction = 0;
		table_source->list_max_employment_sort_direction = 0;
		table_source->list_output_sort_direction = 0;
		table_source->list_profit_sort_direction = 0;
		table_source->list_wage_sort_direction = 0;
		table_source->list_eff_spend_sort_direction = 0;
		table_source->list_eff_sort_direction = 0;
		table_source->list_employment_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->list_max_employment_column_start && x < table_source->list_max_employment_column_start + table_source->list_max_employment_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->list_max_employment_sort_direction;
		table_source->list_employment_sort_direction = 0;
		table_source->list_max_employment_sort_direction = 0;
		table_source->list_output_sort_direction = 0;
		table_source->list_profit_sort_direction = 0;
		table_source->list_wage_sort_direction = 0;
		table_source->list_eff_spend_sort_direction = 0;
		table_source->list_eff_sort_direction = 0;
		table_source->list_max_employment_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->list_output_column_start && x < table_source->list_output_column_start + table_source->list_output_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->list_output_sort_direction;
		table_source->list_employment_sort_direction = 0;
		table_source->list_max_employment_sort_direction = 0;
		table_source->list_output_sort_direction = 0;
		table_source->list_profit_sort_direction = 0;
		table_source->list_wage_sort_direction = 0;
		table_source->list_eff_spend_sort_direction = 0;
		table_source->list_eff_sort_direction = 0;
		table_source->list_output_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->list_profit_column_start && x < table_source->list_profit_column_start + table_source->list_profit_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->list_profit_sort_direction;
		table_source->list_employment_sort_direction = 0;
		table_source->list_max_employment_sort_direction = 0;
		table_source->list_output_sort_direction = 0;
		table_source->list_profit_sort_direction = 0;
		table_source->list_wage_sort_direction = 0;
		table_source->list_eff_spend_sort_direction = 0;
		table_source->list_eff_sort_direction = 0;
		table_source->list_profit_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->list_wage_column_start && x < table_source->list_wage_column_start + table_source->list_wage_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->list_wage_sort_direction;
		table_source->list_employment_sort_direction = 0;
		table_source->list_max_employment_sort_direction = 0;
		table_source->list_output_sort_direction = 0;
		table_source->list_profit_sort_direction = 0;
		table_source->list_wage_sort_direction = 0;
		table_source->list_eff_spend_sort_direction = 0;
		table_source->list_eff_sort_direction = 0;
		table_source->list_wage_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->list_eff_spend_column_start && x < table_source->list_eff_spend_column_start + table_source->list_eff_spend_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->list_eff_spend_sort_direction;
		table_source->list_employment_sort_direction = 0;
		table_source->list_max_employment_sort_direction = 0;
		table_source->list_output_sort_direction = 0;
		table_source->list_profit_sort_direction = 0;
		table_source->list_wage_sort_direction = 0;
		table_source->list_eff_spend_sort_direction = 0;
		table_source->list_eff_sort_direction = 0;
		table_source->list_eff_spend_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->list_eff_column_start && x < table_source->list_eff_column_start + table_source->list_eff_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->list_eff_sort_direction;
		table_source->list_employment_sort_direction = 0;
		table_source->list_max_employment_sort_direction = 0;
		table_source->list_output_sort_direction = 0;
		table_source->list_profit_sort_direction = 0;
		table_source->list_wage_sort_direction = 0;
		table_source->list_eff_spend_sort_direction = 0;
		table_source->list_eff_sort_direction = 0;
		table_source->list_eff_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	return ui::message_result::consumed;}
ui::message_result rgo_report_list_header_content_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void rgo_report_list_header_content_t::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept {
	auto table_source = (rgo_report_body_t*)(parent->parent);
	if(x >= table_source->list_commodity_column_start && x < table_source->list_commodity_column_start + table_source->list_commodity_column_width) {
	}
	if(x >= table_source->list_employment_column_start && x < table_source->list_employment_column_start + table_source->list_employment_column_width) {
	}
	if(x >= table_source->list_max_employment_column_start && x < table_source->list_max_employment_column_start + table_source->list_max_employment_column_width) {
	}
	if(x >= table_source->list_output_column_start && x < table_source->list_output_column_start + table_source->list_output_column_width) {
	}
	if(x >= table_source->list_profit_column_start && x < table_source->list_profit_column_start + table_source->list_profit_column_width) {
	}
	if(x >= table_source->list_wage_column_start && x < table_source->list_wage_column_start + table_source->list_wage_column_width) {
	}
	if(x >= table_source->list_eff_spend_column_start && x < table_source->list_eff_spend_column_start + table_source->list_eff_spend_column_width) {
	}
	if(x >= table_source->list_eff_column_start && x < table_source->list_eff_column_start + table_source->list_eff_column_width) {
	}
		ident = -1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
}
void rgo_report_list_header_content_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto table_source = (rgo_report_body_t*)(parent->parent);
	if(x >=  table_source->list_commodity_column_start && x <  table_source->list_commodity_column_start +  table_source->list_commodity_column_width) {
	}
	if(x >=  table_source->list_employment_column_start && x <  table_source->list_employment_column_start +  table_source->list_employment_column_width) {
	}
	if(x >=  table_source->list_max_employment_column_start && x <  table_source->list_max_employment_column_start +  table_source->list_max_employment_column_width) {
	}
	if(x >=  table_source->list_output_column_start && x <  table_source->list_output_column_start +  table_source->list_output_column_width) {
	}
	if(x >=  table_source->list_profit_column_start && x <  table_source->list_profit_column_start +  table_source->list_profit_column_width) {
	}
	if(x >=  table_source->list_wage_column_start && x <  table_source->list_wage_column_start +  table_source->list_wage_column_width) {
	}
	if(x >=  table_source->list_eff_spend_column_start && x <  table_source->list_eff_spend_column_start +  table_source->list_eff_spend_column_width) {
	}
	if(x >=  table_source->list_eff_column_start && x <  table_source->list_eff_column_start +  table_source->list_eff_column_width) {
	}
}
void rgo_report_list_header_content_t::on_reset_text(sys::state& state) noexcept {
	auto table_source = (rgo_report_body_t*)(parent->parent);
	{
	commodity_cached_text = text::produce_simple_string(state, table_source->list_commodity_header_text_key);
	 commodity_internal_layout.contents.clear();
	 commodity_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  commodity_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->list_commodity_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->list_commodity_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, commodity_cached_text);
	}
	{
	employment_cached_text = text::produce_simple_string(state, table_source->list_employment_header_text_key);
	 employment_internal_layout.contents.clear();
	 employment_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  employment_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->list_employment_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->list_employment_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, employment_cached_text);
	}
	{
	max_employment_cached_text = text::produce_simple_string(state, table_source->list_max_employment_header_text_key);
	 max_employment_internal_layout.contents.clear();
	 max_employment_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  max_employment_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->list_max_employment_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->list_max_employment_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, max_employment_cached_text);
	}
	{
	output_cached_text = text::produce_simple_string(state, table_source->list_output_header_text_key);
	 output_internal_layout.contents.clear();
	 output_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  output_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->list_output_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->list_output_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, output_cached_text);
	}
	{
	profit_cached_text = text::produce_simple_string(state, table_source->list_profit_header_text_key);
	 profit_internal_layout.contents.clear();
	 profit_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  profit_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->list_profit_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->list_profit_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, profit_cached_text);
	}
	{
	wage_cached_text = text::produce_simple_string(state, table_source->list_wage_header_text_key);
	 wage_internal_layout.contents.clear();
	 wage_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  wage_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->list_wage_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->list_wage_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, wage_cached_text);
	}
	{
	eff_spend_cached_text = text::produce_simple_string(state, table_source->list_eff_spend_header_text_key);
	 eff_spend_internal_layout.contents.clear();
	 eff_spend_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  eff_spend_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->list_eff_spend_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->list_eff_spend_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, eff_spend_cached_text);
	}
	{
	eff_cached_text = text::produce_simple_string(state, table_source->list_eff_header_text_key);
	 eff_internal_layout.contents.clear();
	 eff_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  eff_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->list_eff_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->list_eff_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, eff_cached_text);
	}
}
void rgo_report_list_header_content_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto fh = text::make_font_id(state, false, 1.0f * 16);
	auto linesz = state.font_collection.line_height(state, fh); 
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto table_source = (rgo_report_body_t*)(parent->parent);
	auto abs_location = ui::get_absolute_location(state, *this);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - abs_location.x;
	int32_t rel_mouse_y = int32_t(state.mouse_y_position / state.user_settings.ui_scale) - abs_location.y;
	auto ink_color =template_id != -1 ? ogl::color3f(state.ui_templates.colors[state.ui_templates.table_t[template_id].table_color]) : ogl::color3f{}; 	bool col_um_commodity = rel_mouse_x >= table_source->list_commodity_column_start && rel_mouse_x < (table_source->list_commodity_column_start + table_source->list_commodity_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_commodity){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_commodity_column_start + table_source->list_commodity_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_commodity_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_commodity_column_start + table_source->list_commodity_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_commodity_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_commodity_column_start + table_source->list_commodity_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->list_commodity_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_commodity){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_commodity_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_commodity_column_start + table_source->list_commodity_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_commodity){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_commodity_column_start), float(y), float(table_source->list_commodity_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_commodity_column_start), float(y + base_data.size.y - 2), float(table_source->list_commodity_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_commodity = state.ui_templates.colors[table_source->list_commodity_header_text_color]; 	if(!commodity_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : commodity_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->list_commodity_column_start + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_commodity.r, col_color_commodity.g, col_color_commodity.b }, ogl::color_modification::none);
		}
	}
	bool col_um_employment = rel_mouse_x >= table_source->list_employment_column_start && rel_mouse_x < (table_source->list_employment_column_start + table_source->list_employment_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_employment) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->list_employment_column_start), float(y), float(table_source->list_employment_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->list_employment_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_employment){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_employment_column_start + table_source->list_employment_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_employment_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_employment_column_start + table_source->list_employment_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_employment_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_employment_column_start + table_source->list_employment_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->list_employment_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_employment){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_employment_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_employment_column_start + table_source->list_employment_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_employment){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_employment_column_start), float(y), float(table_source->list_employment_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_employment_column_start), float(y + base_data.size.y - 2), float(table_source->list_employment_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_employment = state.ui_templates.colors[table_source->list_employment_header_text_color]; 	if(table_source->list_employment_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->list_employment_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->list_employment_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->list_employment_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!employment_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : employment_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->list_employment_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_employment.r, col_color_employment.g, col_color_employment.b }, ogl::color_modification::none);
		}
	}
	bool col_um_max_employment = rel_mouse_x >= table_source->list_max_employment_column_start && rel_mouse_x < (table_source->list_max_employment_column_start + table_source->list_max_employment_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_max_employment) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->list_max_employment_column_start), float(y), float(table_source->list_max_employment_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->list_max_employment_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_max_employment){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_max_employment_column_start + table_source->list_max_employment_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_max_employment_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_max_employment_column_start + table_source->list_max_employment_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_max_employment_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_max_employment_column_start + table_source->list_max_employment_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->list_max_employment_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_max_employment){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_max_employment_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_max_employment_column_start + table_source->list_max_employment_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_max_employment){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_max_employment_column_start), float(y), float(table_source->list_max_employment_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_max_employment_column_start), float(y + base_data.size.y - 2), float(table_source->list_max_employment_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_max_employment = state.ui_templates.colors[table_source->list_max_employment_header_text_color]; 	if(table_source->list_max_employment_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->list_max_employment_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->list_max_employment_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->list_max_employment_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!max_employment_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : max_employment_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->list_max_employment_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_max_employment.r, col_color_max_employment.g, col_color_max_employment.b }, ogl::color_modification::none);
		}
	}
	bool col_um_output = rel_mouse_x >= table_source->list_output_column_start && rel_mouse_x < (table_source->list_output_column_start + table_source->list_output_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_output) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->list_output_column_start), float(y), float(table_source->list_output_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->list_output_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_output){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_output_column_start + table_source->list_output_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_output_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_output_column_start + table_source->list_output_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_output_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_output_column_start + table_source->list_output_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->list_output_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_output){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_output_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_output_column_start + table_source->list_output_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_output){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_output_column_start), float(y), float(table_source->list_output_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_output_column_start), float(y + base_data.size.y - 2), float(table_source->list_output_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_output = state.ui_templates.colors[table_source->list_output_header_text_color]; 	if(table_source->list_output_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->list_output_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->list_output_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->list_output_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!output_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : output_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->list_output_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_output.r, col_color_output.g, col_color_output.b }, ogl::color_modification::none);
		}
	}
	bool col_um_profit = rel_mouse_x >= table_source->list_profit_column_start && rel_mouse_x < (table_source->list_profit_column_start + table_source->list_profit_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_profit) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->list_profit_column_start), float(y), float(table_source->list_profit_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->list_profit_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_profit){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_profit_column_start + table_source->list_profit_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_profit_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_profit_column_start + table_source->list_profit_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_profit_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_profit_column_start + table_source->list_profit_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->list_profit_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_profit){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_profit_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_profit_column_start + table_source->list_profit_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_profit){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_profit_column_start), float(y), float(table_source->list_profit_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_profit_column_start), float(y + base_data.size.y - 2), float(table_source->list_profit_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_profit = state.ui_templates.colors[table_source->list_profit_header_text_color]; 	if(table_source->list_profit_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->list_profit_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->list_profit_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->list_profit_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!profit_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : profit_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->list_profit_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_profit.r, col_color_profit.g, col_color_profit.b }, ogl::color_modification::none);
		}
	}
	bool col_um_wage = rel_mouse_x >= table_source->list_wage_column_start && rel_mouse_x < (table_source->list_wage_column_start + table_source->list_wage_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_wage) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->list_wage_column_start), float(y), float(table_source->list_wage_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->list_wage_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_wage){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_wage_column_start + table_source->list_wage_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_wage_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_wage_column_start + table_source->list_wage_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_wage_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_wage_column_start + table_source->list_wage_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->list_wage_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_wage){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_wage_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_wage_column_start + table_source->list_wage_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_wage){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_wage_column_start), float(y), float(table_source->list_wage_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_wage_column_start), float(y + base_data.size.y - 2), float(table_source->list_wage_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_wage = state.ui_templates.colors[table_source->list_wage_header_text_color]; 	if(table_source->list_wage_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->list_wage_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->list_wage_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->list_wage_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!wage_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : wage_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->list_wage_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_wage.r, col_color_wage.g, col_color_wage.b }, ogl::color_modification::none);
		}
	}
	bool col_um_eff_spend = rel_mouse_x >= table_source->list_eff_spend_column_start && rel_mouse_x < (table_source->list_eff_spend_column_start + table_source->list_eff_spend_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_eff_spend) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->list_eff_spend_column_start), float(y), float(table_source->list_eff_spend_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->list_eff_spend_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_eff_spend){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_spend_column_start + table_source->list_eff_spend_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_spend_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_spend_column_start + table_source->list_eff_spend_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_spend_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_spend_column_start + table_source->list_eff_spend_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->list_eff_spend_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_eff_spend){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_spend_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_spend_column_start + table_source->list_eff_spend_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_eff_spend){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_spend_column_start), float(y), float(table_source->list_eff_spend_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_spend_column_start), float(y + base_data.size.y - 2), float(table_source->list_eff_spend_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_eff_spend = state.ui_templates.colors[table_source->list_eff_spend_header_text_color]; 	if(table_source->list_eff_spend_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->list_eff_spend_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->list_eff_spend_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->list_eff_spend_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!eff_spend_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : eff_spend_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->list_eff_spend_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_eff_spend.r, col_color_eff_spend.g, col_color_eff_spend.b }, ogl::color_modification::none);
		}
	}
	bool col_um_eff = rel_mouse_x >= table_source->list_eff_column_start && rel_mouse_x < (table_source->list_eff_column_start + table_source->list_eff_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_eff) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->list_eff_column_start), float(y), float(table_source->list_eff_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->list_eff_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_eff){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_column_start + table_source->list_eff_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_column_start + table_source->list_eff_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_column_start + table_source->list_eff_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->list_eff_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_eff){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_column_start + table_source->list_eff_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_eff){
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_column_start), float(y), float(table_source->list_eff_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_eff_column_start), float(y + base_data.size.y - 2), float(table_source->list_eff_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_eff = state.ui_templates.colors[table_source->list_eff_header_text_color]; 	if(table_source->list_eff_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->list_eff_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->list_eff_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->list_eff_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!eff_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : eff_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->list_eff_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_eff.r, col_color_eff.g, col_color_eff.b }, ogl::color_modification::none);
		}
	}
	if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y)){
	ogl::render_alpha_colored_rect(state, float(x), float(y + base_data.size.y - 1), float(base_data.size.x), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
}
void rgo_report_list_header_content_t::on_update(sys::state& state) noexcept {
	rgo_report_list_header_t& list_header = *((rgo_report_list_header_t*)(parent)); 
	rgo_report_body_t& body = *((rgo_report_body_t*)(parent->parent)); 
// BEGIN list_header::content::update
// END
}
void rgo_report_list_header_content_t::on_create(sys::state& state) noexcept {
// BEGIN list_header::content::create
// END
}
void rgo_report_list_header_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void rgo_report_list_header_t::on_update(sys::state& state) noexcept {
	rgo_report_body_t& body = *((rgo_report_body_t*)(parent->parent)); 
// BEGIN list_header::update
// END
	remake_layout(state, true);
}
void rgo_report_list_header_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "body") {
					temp.ptr = make_rgo_report_body(state);
				}
				if(cname == "list_item") {
					temp.ptr = make_rgo_report_list_item(state);
				}
				if(cname == "list_header") {
					temp.ptr = make_rgo_report_list_header(state);
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
void rgo_report_list_header_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("rgo_report::list_header"));
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
			content = std::make_unique<rgo_report_list_header_content_t>();
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
// BEGIN list_header::create
// END
}
std::unique_ptr<ui::element_base> make_rgo_report_list_header(sys::state& state) {
	auto ptr = std::make_unique<rgo_report_list_header_t>();
	ptr->on_create(state);
	return ptr;
}
// LOST-CODE
}
