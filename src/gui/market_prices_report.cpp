namespace alice_ui {
struct market_prices_report_body_header_t;
struct market_prices_report_body_t;
struct market_prices_report_list_header_bg_t;
struct market_prices_report_list_header_t;
struct market_prices_report_list_item_bg_t;
struct market_prices_report_list_item_icon_t;
struct market_prices_report_list_item_t;
struct market_prices_report_body_header_t : public alice_ui::template_label {
// BEGIN body::header::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct market_prices_report_body_price_list_t : public layout_generator {
// BEGIN body::price_list::variables
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
struct market_prices_report_list_header_bg_t : public ui::element_base {
// BEGIN list_header::bg::variables
// END
	text::layout name_internal_layout;
	std::string name_cached_text;
	text::layout local_price_internal_layout;
	std::string local_price_cached_text;
	text::layout median_price_internal_layout;
	std::string median_price_cached_text;
	text::layout ratio_price_internal_layout;
	std::string ratio_price_cached_text;
	text::layout future_change_internal_layout;
	std::string future_change_cached_text;
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
	void tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept override;
	void on_reset_text(sys::state & state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct market_prices_report_list_item_bg_t : public ui::element_base {
// BEGIN list_item::bg::variables
// END
	text::layout spacer_internal_layout;
	text::text_color  spacer_text_color = text::text_color::black;
	std::string spacer_cached_text;
	void set_spacer_text(sys::state & state, std::string const& new_text);
	text::layout name_internal_layout;
	text::text_color  name_text_color = text::text_color::black;
	std::string name_cached_text;
	void set_name_text(sys::state & state, std::string const& new_text);
	text::layout local_price_internal_layout;
	text::text_color  local_price_text_color = text::text_color::black;
	std::string local_price_cached_text;
	void set_local_price_text(sys::state & state, std::string const& new_text);
	text::layout median_price_internal_layout;
	text::text_color  median_price_text_color = text::text_color::black;
	std::string median_price_cached_text;
	void set_median_price_text(sys::state & state, std::string const& new_text);
	text::layout ratio_price_internal_layout;
	text::text_color  ratio_price_text_color = text::text_color::black;
	std::string ratio_price_cached_text;
	void set_ratio_price_text(sys::state & state, std::string const& new_text);
	text::layout future_change_internal_layout;
	text::text_color  future_change_text_color = text::text_color::black;
	std::string future_change_cached_text;
	void set_future_change_text(sys::state & state, std::string const& new_text);
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
	void tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct market_prices_report_list_item_icon_t : public ui::element_base {
// BEGIN list_item::icon::variables
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
struct market_prices_report_body_t : public layout_window_element {
// BEGIN body::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<market_prices_report_body_header_t> header;
	market_prices_report_body_price_list_t price_list;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	text::text_color list_spacer_header_text_color = text::text_color::black;
	text::text_color list_spacer_column_text_color = text::text_color::black;
	text::alignment list_spacer_text_alignment = text::alignment::center;
	int16_t list_spacer_column_start = 0;
	int16_t list_spacer_column_width = 0;
	std::string_view list_name_header_text_key;
	text::text_color list_name_header_text_color = text::text_color::black;
	text::text_color list_name_column_text_color = text::text_color::black;
	text::alignment list_name_text_alignment = text::alignment::left;
	int8_t list_name_sort_direction = 0;
	int16_t list_name_column_start = 0;
	int16_t list_name_column_width = 0;
	std::string_view list_local_price_header_text_key;
	text::text_color list_local_price_header_text_color = text::text_color::black;
	text::text_color list_local_price_column_text_color = text::text_color::black;
	text::alignment list_local_price_text_alignment = text::alignment::right;
	int8_t list_local_price_sort_direction = 0;
	int16_t list_local_price_column_start = 0;
	int16_t list_local_price_column_width = 0;
	std::string_view list_median_price_header_text_key;
	text::text_color list_median_price_header_text_color = text::text_color::black;
	text::text_color list_median_price_column_text_color = text::text_color::black;
	text::alignment list_median_price_text_alignment = text::alignment::right;
	int8_t list_median_price_sort_direction = 0;
	int16_t list_median_price_column_start = 0;
	int16_t list_median_price_column_width = 0;
	std::string_view list_ratio_price_header_text_key;
	text::text_color list_ratio_price_header_text_color = text::text_color::black;
	text::text_color list_ratio_price_column_text_color = text::text_color::black;
	text::alignment list_ratio_price_text_alignment = text::alignment::center;
	int8_t list_ratio_price_sort_direction = 0;
	int16_t list_ratio_price_column_start = 0;
	int16_t list_ratio_price_column_width = 0;
	std::string_view list_future_change_header_text_key;
	text::text_color list_future_change_header_text_color = text::text_color::black;
	text::text_color list_future_change_column_text_color = text::text_color::black;
	text::alignment list_future_change_text_alignment = text::alignment::center;
	int8_t list_future_change_sort_direction = 0;
	int16_t list_future_change_column_start = 0;
	int16_t list_future_change_column_width = 0;
	std::string_view list_ascending_icon_key;
	dcon::texture_id list_ascending_icon;
	std::string_view list_descending_icon_key;
	dcon::texture_id list_descending_icon;
	ogl::color3f list_divider_color{float(0.000000), float(0.000000), float(0.000000)};
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
std::unique_ptr<ui::element_base> make_market_prices_report_body(sys::state& state);
struct market_prices_report_list_header_t : public layout_window_element {
// BEGIN list_header::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<market_prices_report_list_header_bg_t> bg;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		return ui::message_result::unseen;
	}
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_market_prices_report_list_header(sys::state& state);
struct market_prices_report_list_item_t : public layout_window_element {
// BEGIN list_item::variables
// END
	dcon::commodity_id cid;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<market_prices_report_list_item_bg_t> bg;
	std::unique_ptr<market_prices_report_list_item_icon_t> icon;
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
		if(name_parameter == "cid") {
			return (void*)(&cid);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_market_prices_report_list_item(sys::state& state);
void market_prices_report_body_price_list_t::add_list_item(dcon::commodity_id cid) {
	values.emplace_back(list_item_option{cid});
}
void  market_prices_report_body_price_list_t::on_create(sys::state& state, layout_window_element* parent) {
	market_prices_report_body_t& body = *((market_prices_report_body_t*)(parent)); 
// BEGIN body::price_list::on_create
	state.world.for_each_commodity([&](auto cid) {
		add_list_item(cid);
	});
// END
}
void  market_prices_report_body_price_list_t::update(sys::state& state, layout_window_element* parent) {
	market_prices_report_body_t& body = *((market_prices_report_body_t*)(parent)); 
// BEGIN body::price_list::update
// END
	{
	bool work_to_do = false;
	auto table_source = (market_prices_report_body_t*)(parent);
	if(table_source->list_name_sort_direction != 0) work_to_do = true;
	if(table_source->list_local_price_sort_direction != 0) work_to_do = true;
	if(table_source->list_median_price_sort_direction != 0) work_to_do = true;
	if(table_source->list_ratio_price_sort_direction != 0) work_to_do = true;
	if(table_source->list_future_change_sort_direction != 0) work_to_do = true;
	if(work_to_do) {
		for(size_t i = 0; i < values.size(); ) {
			if(std::holds_alternative<list_item_option>(values[i])) {
				auto start_i = i;
				while(i < values.size() && std::holds_alternative<list_item_option>(values[i])) ++i;
				if(table_source->list_name_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<list_item_option>(raw_a);
						auto const& b = std::get<list_item_option>(raw_b);
						int8_t result = 0;
// BEGIN body::price_list::list::sort::name
						auto name_a = text::produce_simple_string(state, state.world.commodity_get_name(a.cid));
						auto name_b = text::produce_simple_string(state, state.world.commodity_get_name(b.cid));
						result = cmp3(name_a, name_b);
// END
						return -result == table_source->list_name_sort_direction;
					});
				}
				if(table_source->list_local_price_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<list_item_option>(raw_a);
						auto const& b = std::get<list_item_option>(raw_b);
						int8_t result = 0;
// BEGIN body::price_list::list::sort::local_price
						auto pid = state.map_state.get_selected_province();
						auto sid = state.world.province_get_state_membership(pid);
						auto mid = state.world.state_instance_get_market_from_local_market(sid);
						float local_a = economy::price(state, sid, a.cid);
						float local_b = economy::price(state, sid, b.cid);
						result = cmp3(local_a, local_b);
// END
						return -result == table_source->list_local_price_sort_direction;
					});
				}
				if(table_source->list_median_price_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<list_item_option>(raw_a);
						auto const& b = std::get<list_item_option>(raw_b);
						int8_t result = 0;
// BEGIN body::price_list::list::sort::median_price
						auto pid = state.map_state.get_selected_province();
						auto sid = state.world.province_get_state_membership(pid);
						auto mid = state.world.state_instance_get_market_from_local_market(sid);
						float value_a = state.world.commodity_get_median_price(a.cid);
						float value_b = state.world.commodity_get_median_price(b.cid);
						result = cmp3(value_a, value_b);
// END
						return -result == table_source->list_median_price_sort_direction;
					});
				}
				if(table_source->list_ratio_price_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<list_item_option>(raw_a);
						auto const& b = std::get<list_item_option>(raw_b);
						int8_t result = 0;
// BEGIN body::price_list::list::sort::ratio_price
						auto pid = state.map_state.get_selected_province();
						auto sid = state.world.province_get_state_membership(pid);
						auto mid = state.world.state_instance_get_market_from_local_market(sid);
						float value_a = economy::price(state, sid, a.cid) / state.world.commodity_get_median_price(a.cid);
						float value_b = economy::price(state, sid, b.cid) / state.world.commodity_get_median_price(b.cid);
						result = cmp3(value_a, value_b);
// END
						return -result == table_source->list_ratio_price_sort_direction;
					});
				}
				if(table_source->list_future_change_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<list_item_option>(raw_a);
						auto const& b = std::get<list_item_option>(raw_b);
						int8_t result = 0;
// BEGIN body::price_list::list::sort::future_change
						auto pid = state.map_state.get_selected_province();
						auto sid = state.world.province_get_state_membership(pid);
						auto mid = state.world.state_instance_get_market_from_local_market(sid);
						auto supply_a = economy::supply(state, mid, a.cid);
						auto supply_b = economy::supply(state, mid, b.cid);
						auto demand_a = economy::demand(state, mid, a.cid);
						auto demand_b = economy::demand(state, mid, b.cid);
						float local_a = economy::price(state, sid, a.cid);
						float local_b = economy::price(state, sid, b.cid);
						float value_a = economy::price_properties::change(local_a, supply_a, demand_a);
						float value_b = economy::price_properties::change(local_b, supply_b, demand_b);
						result = cmp3(value_a, value_b);
// END
						return -result == table_source->list_future_change_sort_direction;
					});
				}
			} else {
				++i;
			}
		}
	}
	}
}
measure_result  market_prices_report_body_price_list_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<list_item_option>(values[index])) {
		if(list_header_pool.empty()) list_header_pool.emplace_back(make_market_prices_report_list_header(state));
		if(list_item_pool.empty()) list_item_pool.emplace_back(make_market_prices_report_list_item(state));
		if(index == 0 || first_in_section || (true && !std::holds_alternative<list_item_option>(values[index - 1]))) {
			if(destination) {
				if(list_header_pool.size() <= size_t(list_header_pool_used)) list_header_pool.emplace_back(make_market_prices_report_list_header(state));
				if(list_item_pool.size() <= size_t(list_item_pool_used)) list_item_pool.emplace_back(make_market_prices_report_list_item(state));
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
				((market_prices_report_list_item_t*)(list_item_pool[list_item_pool_used].get()))->cid = std::get<list_item_option>(values[index]).cid;
			((market_prices_report_list_item_t*)(list_item_pool[list_item_pool_used].get()))->is_active = !alternate;
				list_item_pool[list_item_pool_used]->impl_on_update(state);
				list_header_pool_used++;
				list_item_pool_used++;
			}
			return measure_result{std::max(list_header_pool[0]->base_data.size.x, list_item_pool[0]->base_data.size.x), list_header_pool[0]->base_data.size.y + list_item_pool[0]->base_data.size.y + 0, measure_result::special::none};
		}
		if(destination) {
			if(list_item_pool.size() <= size_t(list_item_pool_used)) list_item_pool.emplace_back(make_market_prices_report_list_item(state));
			list_item_pool[list_item_pool_used]->base_data.position.x = int16_t(x);
			list_item_pool[list_item_pool_used]->base_data.position.y = int16_t(y);
			list_item_pool[list_item_pool_used]->parent = destination;
			destination->children.push_back(list_item_pool[list_item_pool_used].get());
			((market_prices_report_list_item_t*)(list_item_pool[list_item_pool_used].get()))->cid = std::get<list_item_option>(values[index]).cid;
			((market_prices_report_list_item_t*)(list_item_pool[list_item_pool_used].get()))->is_active = alternate;
			list_item_pool[list_item_pool_used]->impl_on_update(state);
			list_item_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ list_item_pool[0]->base_data.size.x, list_item_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  market_prices_report_body_price_list_t::reset_pools() {
	list_header_pool_used = 0;
	list_item_pool_used = 0;
}
void market_prices_report_body_header_t::on_update(sys::state& state) noexcept {
	market_prices_report_body_t& body = *((market_prices_report_body_t*)(parent)); 
// BEGIN body::header::update
	text::substitution_map m;
	text::add_to_substitution_map(m, text::variable_type::province, state.map_state.selected_province);
	auto result = text::resolve_string_substitution(state, "market_prices_report", m);
	set_text(state, result);
// END
}
ui::message_result market_prices_report_body_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	state.ui_state.drag_target = this;
	return ui::message_result::consumed;
}
ui::message_result market_prices_report_body_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void market_prices_report_body_t::on_update(sys::state& state) noexcept {
// BEGIN body::update
// END
	price_list.update(state, this);
	remake_layout(state, true);
}
void market_prices_report_body_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
					temp.ptr = make_market_prices_report_body(state);
				}
				if(cname == "list_header") {
					temp.ptr = make_market_prices_report_list_header(state);
				}
				if(cname == "list_item") {
					temp.ptr = make_market_prices_report_list_item(state);
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
				if(cname == "price_list") {
					temp.generator = &price_list;
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
void market_prices_report_body_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("market_prices_report::body"));
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
			header = std::make_unique<market_prices_report_body_header_t>();
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
		if(child_data.name == ".tablist") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			list_ascending_icon_key = main_section.read<std::string_view>();
			list_descending_icon_key = main_section.read<std::string_view>();
			main_section.read(list_divider_color);
			auto col_section = tbuffer.read_section();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			list_spacer_column_start = running_w_total;
			col_section.read(list_spacer_column_width);
			running_w_total += list_spacer_column_width;
			col_section.read(list_spacer_column_text_color);
			col_section.read(list_spacer_header_text_color);
			col_section.read(list_spacer_text_alignment);
			list_name_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			list_name_column_start = running_w_total;
			col_section.read(list_name_column_width);
			running_w_total += list_name_column_width;
			col_section.read(list_name_column_text_color);
			col_section.read(list_name_header_text_color);
			col_section.read(list_name_text_alignment);
			list_local_price_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			list_local_price_column_start = running_w_total;
			col_section.read(list_local_price_column_width);
			running_w_total += list_local_price_column_width;
			col_section.read(list_local_price_column_text_color);
			col_section.read(list_local_price_header_text_color);
			col_section.read(list_local_price_text_alignment);
			list_median_price_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			list_median_price_column_start = running_w_total;
			col_section.read(list_median_price_column_width);
			running_w_total += list_median_price_column_width;
			col_section.read(list_median_price_column_text_color);
			col_section.read(list_median_price_header_text_color);
			col_section.read(list_median_price_text_alignment);
			list_ratio_price_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			list_ratio_price_column_start = running_w_total;
			col_section.read(list_ratio_price_column_width);
			running_w_total += list_ratio_price_column_width;
			col_section.read(list_ratio_price_column_text_color);
			col_section.read(list_ratio_price_header_text_color);
			col_section.read(list_ratio_price_text_alignment);
			list_future_change_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			list_future_change_column_start = running_w_total;
			col_section.read(list_future_change_column_width);
			running_w_total += list_future_change_column_width;
			col_section.read(list_future_change_column_text_color);
			col_section.read(list_future_change_header_text_color);
			col_section.read(list_future_change_text_alignment);
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
	price_list.on_create(state, this);
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN body::create
// END
}
std::unique_ptr<ui::element_base> make_market_prices_report_body(sys::state& state) {
	auto ptr = std::make_unique<market_prices_report_body_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result market_prices_report_list_header_bg_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	auto table_source = (market_prices_report_body_t*)(parent->parent);
	if(x >= table_source->list_name_column_start && x < table_source->list_name_column_start + table_source->list_name_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->list_name_sort_direction;
		table_source->list_name_sort_direction = 0;
		table_source->list_local_price_sort_direction = 0;
		table_source->list_median_price_sort_direction = 0;
		table_source->list_ratio_price_sort_direction = 0;
		table_source->list_future_change_sort_direction = 0;
		table_source->list_name_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->list_local_price_column_start && x < table_source->list_local_price_column_start + table_source->list_local_price_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->list_local_price_sort_direction;
		table_source->list_name_sort_direction = 0;
		table_source->list_local_price_sort_direction = 0;
		table_source->list_median_price_sort_direction = 0;
		table_source->list_ratio_price_sort_direction = 0;
		table_source->list_future_change_sort_direction = 0;
		table_source->list_local_price_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->list_median_price_column_start && x < table_source->list_median_price_column_start + table_source->list_median_price_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->list_median_price_sort_direction;
		table_source->list_name_sort_direction = 0;
		table_source->list_local_price_sort_direction = 0;
		table_source->list_median_price_sort_direction = 0;
		table_source->list_ratio_price_sort_direction = 0;
		table_source->list_future_change_sort_direction = 0;
		table_source->list_median_price_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->list_ratio_price_column_start && x < table_source->list_ratio_price_column_start + table_source->list_ratio_price_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->list_ratio_price_sort_direction;
		table_source->list_name_sort_direction = 0;
		table_source->list_local_price_sort_direction = 0;
		table_source->list_median_price_sort_direction = 0;
		table_source->list_ratio_price_sort_direction = 0;
		table_source->list_future_change_sort_direction = 0;
		table_source->list_ratio_price_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->list_future_change_column_start && x < table_source->list_future_change_column_start + table_source->list_future_change_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->list_future_change_sort_direction;
		table_source->list_name_sort_direction = 0;
		table_source->list_local_price_sort_direction = 0;
		table_source->list_median_price_sort_direction = 0;
		table_source->list_ratio_price_sort_direction = 0;
		table_source->list_future_change_sort_direction = 0;
		table_source->list_future_change_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	return ui::message_result::consumed;}
ui::message_result market_prices_report_list_header_bg_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void market_prices_report_list_header_bg_t::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept {
	auto table_source = (market_prices_report_body_t*)(parent->parent);
	if(x >= table_source->list_spacer_column_start && x < table_source->list_spacer_column_start + table_source->list_spacer_column_width) {
	}
	if(x >= table_source->list_name_column_start && x < table_source->list_name_column_start + table_source->list_name_column_width) {
	}
	if(x >= table_source->list_local_price_column_start && x < table_source->list_local_price_column_start + table_source->list_local_price_column_width) {
	}
	if(x >= table_source->list_median_price_column_start && x < table_source->list_median_price_column_start + table_source->list_median_price_column_width) {
	}
	if(x >= table_source->list_ratio_price_column_start && x < table_source->list_ratio_price_column_start + table_source->list_ratio_price_column_width) {
	}
	if(x >= table_source->list_future_change_column_start && x < table_source->list_future_change_column_start + table_source->list_future_change_column_width) {
	}
		ident = -1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
}
void market_prices_report_list_header_bg_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto table_source = (market_prices_report_body_t*)(parent->parent);
	if(x >=  table_source->list_spacer_column_start && x <  table_source->list_spacer_column_start +  table_source->list_spacer_column_width) {
	}
	if(x >=  table_source->list_name_column_start && x <  table_source->list_name_column_start +  table_source->list_name_column_width) {
	}
	if(x >=  table_source->list_local_price_column_start && x <  table_source->list_local_price_column_start +  table_source->list_local_price_column_width) {
	}
	if(x >=  table_source->list_median_price_column_start && x <  table_source->list_median_price_column_start +  table_source->list_median_price_column_width) {
	}
	if(x >=  table_source->list_ratio_price_column_start && x <  table_source->list_ratio_price_column_start +  table_source->list_ratio_price_column_width) {
	}
	if(x >=  table_source->list_future_change_column_start && x <  table_source->list_future_change_column_start +  table_source->list_future_change_column_width) {
	}
}
void market_prices_report_list_header_bg_t::on_reset_text(sys::state& state) noexcept {
	auto table_source = (market_prices_report_body_t*)(parent->parent);
	{
	name_cached_text = text::produce_simple_string(state, table_source->list_name_header_text_key);
	 name_internal_layout.contents.clear();
	 name_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  name_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->list_name_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->list_name_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, name_cached_text);
	}
	{
	local_price_cached_text = text::produce_simple_string(state, table_source->list_local_price_header_text_key);
	 local_price_internal_layout.contents.clear();
	 local_price_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  local_price_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->list_local_price_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->list_local_price_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, local_price_cached_text);
	}
	{
	median_price_cached_text = text::produce_simple_string(state, table_source->list_median_price_header_text_key);
	 median_price_internal_layout.contents.clear();
	 median_price_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  median_price_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->list_median_price_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->list_median_price_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, median_price_cached_text);
	}
	{
	ratio_price_cached_text = text::produce_simple_string(state, table_source->list_ratio_price_header_text_key);
	 ratio_price_internal_layout.contents.clear();
	 ratio_price_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  ratio_price_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->list_ratio_price_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->list_ratio_price_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, ratio_price_cached_text);
	}
	{
	future_change_cached_text = text::produce_simple_string(state, table_source->list_future_change_header_text_key);
	 future_change_internal_layout.contents.clear();
	 future_change_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  future_change_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->list_future_change_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->list_future_change_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, future_change_cached_text);
	}
}
void market_prices_report_list_header_bg_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto fh = text::make_font_id(state, false, 1.0f * 16);
	auto linesz = state.font_collection.line_height(state, fh); 
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto table_source = (market_prices_report_body_t*)(parent->parent);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - ui::get_absolute_location(state, *this).x;
	bool col_um_spacer = rel_mouse_x >= table_source->list_spacer_column_start && rel_mouse_x < (table_source->list_spacer_column_start + table_source->list_spacer_column_width);
	bool col_um_name = rel_mouse_x >= table_source->list_name_column_start && rel_mouse_x < (table_source->list_name_column_start + table_source->list_name_column_width);
	if(table_source->list_name_sort_direction > 0) {
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_name, false, true), float(x + table_source->list_name_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), ogl::get_late_load_texture_handle(state, table_source->list_ascending_icon, table_source->list_ascending_icon_key), ui::rotation::upright, false, state_is_rtl(state));
	}
	if(table_source->list_name_sort_direction < 0) {
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_name, false, true), float(x + table_source->list_name_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), ogl::get_late_load_texture_handle(state, table_source->list_descending_icon, table_source->list_descending_icon_key), ui::rotation::upright, false, state_is_rtl(state));
	}
	if(!name_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_name , false, true); 
		for(auto& t : name_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->list_name_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, table_source->list_name_header_text_color), cmod);
		}
	}
	bool col_um_local_price = rel_mouse_x >= table_source->list_local_price_column_start && rel_mouse_x < (table_source->list_local_price_column_start + table_source->list_local_price_column_width);
	if(table_source->list_local_price_sort_direction > 0) {
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_local_price, false, true), float(x + table_source->list_local_price_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), ogl::get_late_load_texture_handle(state, table_source->list_ascending_icon, table_source->list_ascending_icon_key), ui::rotation::upright, false, state_is_rtl(state));
	}
	if(table_source->list_local_price_sort_direction < 0) {
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_local_price, false, true), float(x + table_source->list_local_price_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), ogl::get_late_load_texture_handle(state, table_source->list_descending_icon, table_source->list_descending_icon_key), ui::rotation::upright, false, state_is_rtl(state));
	}
	if(!local_price_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_local_price , false, true); 
		for(auto& t : local_price_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->list_local_price_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, table_source->list_local_price_header_text_color), cmod);
		}
	}
	bool col_um_median_price = rel_mouse_x >= table_source->list_median_price_column_start && rel_mouse_x < (table_source->list_median_price_column_start + table_source->list_median_price_column_width);
	if(table_source->list_median_price_sort_direction > 0) {
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_median_price, false, true), float(x + table_source->list_median_price_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), ogl::get_late_load_texture_handle(state, table_source->list_ascending_icon, table_source->list_ascending_icon_key), ui::rotation::upright, false, state_is_rtl(state));
	}
	if(table_source->list_median_price_sort_direction < 0) {
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_median_price, false, true), float(x + table_source->list_median_price_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), ogl::get_late_load_texture_handle(state, table_source->list_descending_icon, table_source->list_descending_icon_key), ui::rotation::upright, false, state_is_rtl(state));
	}
	if(!median_price_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_median_price , false, true); 
		for(auto& t : median_price_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->list_median_price_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, table_source->list_median_price_header_text_color), cmod);
		}
	}
	bool col_um_ratio_price = rel_mouse_x >= table_source->list_ratio_price_column_start && rel_mouse_x < (table_source->list_ratio_price_column_start + table_source->list_ratio_price_column_width);
	if(table_source->list_ratio_price_sort_direction > 0) {
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_ratio_price, false, true), float(x + table_source->list_ratio_price_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), ogl::get_late_load_texture_handle(state, table_source->list_ascending_icon, table_source->list_ascending_icon_key), ui::rotation::upright, false, state_is_rtl(state));
	}
	if(table_source->list_ratio_price_sort_direction < 0) {
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_ratio_price, false, true), float(x + table_source->list_ratio_price_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), ogl::get_late_load_texture_handle(state, table_source->list_descending_icon, table_source->list_descending_icon_key), ui::rotation::upright, false, state_is_rtl(state));
	}
	if(!ratio_price_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_ratio_price , false, true); 
		for(auto& t : ratio_price_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->list_ratio_price_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, table_source->list_ratio_price_header_text_color), cmod);
		}
	}
	bool col_um_future_change = rel_mouse_x >= table_source->list_future_change_column_start && rel_mouse_x < (table_source->list_future_change_column_start + table_source->list_future_change_column_width);
	if(table_source->list_future_change_sort_direction > 0) {
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_future_change, false, true), float(x + table_source->list_future_change_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), ogl::get_late_load_texture_handle(state, table_source->list_ascending_icon, table_source->list_ascending_icon_key), ui::rotation::upright, false, state_is_rtl(state));
	}
	if(table_source->list_future_change_sort_direction < 0) {
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_future_change, false, true), float(x + table_source->list_future_change_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), ogl::get_late_load_texture_handle(state, table_source->list_descending_icon, table_source->list_descending_icon_key), ui::rotation::upright, false, state_is_rtl(state));
	}
	if(!future_change_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_future_change , false, true); 
		for(auto& t : future_change_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->list_future_change_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, table_source->list_future_change_header_text_color), cmod);
		}
	}
	ogl::render_alpha_colored_rect(state, float(x), float(y + base_data.size.y - 1), float(base_data.size.x), float(1), table_source->list_divider_color.r, table_source->list_divider_color.g, table_source->list_divider_color.b, 1.0f);
}
void market_prices_report_list_header_bg_t::on_update(sys::state& state) noexcept {
	market_prices_report_list_header_t& list_header = *((market_prices_report_list_header_t*)(parent)); 
	market_prices_report_body_t& body = *((market_prices_report_body_t*)(parent->parent)); 
// BEGIN list_header::bg::update
// END
}
void market_prices_report_list_header_bg_t::on_create(sys::state& state) noexcept {
// BEGIN list_header::bg::create
// END
}
void market_prices_report_list_header_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto cmod = ui::get_color_modification(false, false,  false);
	for (auto& _item : textures_to_render) {
		if (_item.texture_type == background_type::texture)
			ogl::render_textured_rect(state, cmod, float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (_item.texture_type == background_type::border_texture_repeat)
			ogl::render_rect_with_repeated_border(state, cmod, float(8), float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (_item.texture_type == background_type::textured_corners)
			ogl::render_rect_with_repeated_corner(state, cmod, float(8), float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
	}
	auto table_source = (market_prices_report_body_t*)(parent);
	auto under_mouse = [&](){auto p = state.ui_state.under_mouse; while(p){ if(p == this) return true; p = p->parent; } return false;}();
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - ui::get_absolute_location(state, *this).x;
	if(under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), 0.772549f, 0.219608f, 0.219608f, 0.117647f);
	}
	bool col_um_spacer = rel_mouse_x >= table_source->list_spacer_column_start && rel_mouse_x < (table_source->list_spacer_column_start + table_source->list_spacer_column_width);
	if(col_um_spacer && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_spacer_column_start), float(y), float(table_source->list_spacer_column_width), float(base_data.size.y), 0.772549f, 0.219608f, 0.219608f, 0.117647f);
	}
	bool col_um_name = rel_mouse_x >= table_source->list_name_column_start && rel_mouse_x < (table_source->list_name_column_start + table_source->list_name_column_width);
	if(col_um_name && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_name_column_start), float(y), float(table_source->list_name_column_width), float(base_data.size.y), 0.772549f, 0.219608f, 0.219608f, 0.117647f);
	}
	bool col_um_local_price = rel_mouse_x >= table_source->list_local_price_column_start && rel_mouse_x < (table_source->list_local_price_column_start + table_source->list_local_price_column_width);
	if(col_um_local_price && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_local_price_column_start), float(y), float(table_source->list_local_price_column_width), float(base_data.size.y), 0.772549f, 0.219608f, 0.219608f, 0.117647f);
	}
	bool col_um_median_price = rel_mouse_x >= table_source->list_median_price_column_start && rel_mouse_x < (table_source->list_median_price_column_start + table_source->list_median_price_column_width);
	if(col_um_median_price && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_median_price_column_start), float(y), float(table_source->list_median_price_column_width), float(base_data.size.y), 0.772549f, 0.219608f, 0.219608f, 0.117647f);
	}
	bool col_um_ratio_price = rel_mouse_x >= table_source->list_ratio_price_column_start && rel_mouse_x < (table_source->list_ratio_price_column_start + table_source->list_ratio_price_column_width);
	if(col_um_ratio_price && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_ratio_price_column_start), float(y), float(table_source->list_ratio_price_column_width), float(base_data.size.y), 0.772549f, 0.219608f, 0.219608f, 0.117647f);
	}
	bool col_um_future_change = rel_mouse_x >= table_source->list_future_change_column_start && rel_mouse_x < (table_source->list_future_change_column_start + table_source->list_future_change_column_width);
	if(col_um_future_change && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_future_change_column_start), float(y), float(table_source->list_future_change_column_width), float(base_data.size.y), 0.772549f, 0.219608f, 0.219608f, 0.117647f);
	}
}
void market_prices_report_list_header_t::on_update(sys::state& state) noexcept {
	market_prices_report_body_t& body = *((market_prices_report_body_t*)(parent->parent)); 
// BEGIN list_header::update
// END
	remake_layout(state, true);
}
void market_prices_report_list_header_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "bg") {
					temp.ptr = bg.get();
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
					temp.ptr = make_market_prices_report_body(state);
				}
				if(cname == "list_header") {
					temp.ptr = make_market_prices_report_list_header(state);
				}
				if(cname == "list_item") {
					temp.ptr = make_market_prices_report_list_item(state);
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
void market_prices_report_list_header_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("market_prices_report::list_header"));
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
		if(child_data.name == "bg") {
			bg = std::make_unique<market_prices_report_list_header_bg_t>();
			bg->parent = this;
			auto cptr = bg.get();
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
// BEGIN list_header::create
// END
}
std::unique_ptr<ui::element_base> make_market_prices_report_list_header(sys::state& state) {
	auto ptr = std::make_unique<market_prices_report_list_header_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result market_prices_report_list_item_bg_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result market_prices_report_list_item_bg_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void market_prices_report_list_item_bg_t::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept {
	auto table_source = (market_prices_report_body_t*)(parent->parent);
	if(x >= table_source->list_spacer_column_start && x < table_source->list_spacer_column_start + table_source->list_spacer_column_width) {
	}
	if(x >= table_source->list_name_column_start && x < table_source->list_name_column_start + table_source->list_name_column_width) {
	}
	if(x >= table_source->list_local_price_column_start && x < table_source->list_local_price_column_start + table_source->list_local_price_column_width) {
	}
	if(x >= table_source->list_median_price_column_start && x < table_source->list_median_price_column_start + table_source->list_median_price_column_width) {
	}
	if(x >= table_source->list_ratio_price_column_start && x < table_source->list_ratio_price_column_start + table_source->list_ratio_price_column_width) {
	}
	if(x >= table_source->list_future_change_column_start && x < table_source->list_future_change_column_start + table_source->list_future_change_column_width) {
	}
		ident = -1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
}
void market_prices_report_list_item_bg_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto table_source = (market_prices_report_body_t*)(parent->parent);
	if(x >=  table_source->list_spacer_column_start && x <  table_source->list_spacer_column_start +  table_source->list_spacer_column_width) {
	}
	if(x >=  table_source->list_name_column_start && x <  table_source->list_name_column_start +  table_source->list_name_column_width) {
	}
	if(x >=  table_source->list_local_price_column_start && x <  table_source->list_local_price_column_start +  table_source->list_local_price_column_width) {
	}
	if(x >=  table_source->list_median_price_column_start && x <  table_source->list_median_price_column_start +  table_source->list_median_price_column_width) {
	}
	if(x >=  table_source->list_ratio_price_column_start && x <  table_source->list_ratio_price_column_start +  table_source->list_ratio_price_column_width) {
	}
	if(x >=  table_source->list_future_change_column_start && x <  table_source->list_future_change_column_start +  table_source->list_future_change_column_width) {
	}
}
void market_prices_report_list_item_bg_t::set_spacer_text(sys::state & state, std::string const& new_text) {
		auto table_source = (market_prices_report_body_t*)(parent->parent);
	if(new_text !=  spacer_cached_text) {
		spacer_cached_text = new_text;
		spacer_internal_layout.contents.clear();
		spacer_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ spacer_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->list_spacer_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->list_spacer_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, spacer_cached_text);
		}
	} else {
	}
}
void market_prices_report_list_item_bg_t::set_name_text(sys::state & state, std::string const& new_text) {
		auto table_source = (market_prices_report_body_t*)(parent->parent);
	if(new_text !=  name_cached_text) {
		name_cached_text = new_text;
		name_internal_layout.contents.clear();
		name_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ name_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->list_name_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->list_name_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, name_cached_text);
		}
	} else {
	}
}
void market_prices_report_list_item_bg_t::set_local_price_text(sys::state & state, std::string const& new_text) {
		auto table_source = (market_prices_report_body_t*)(parent->parent);
	if(new_text !=  local_price_cached_text) {
		local_price_cached_text = new_text;
		local_price_internal_layout.contents.clear();
		local_price_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ local_price_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->list_local_price_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->list_local_price_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, local_price_cached_text);
		}
	} else {
	}
}
void market_prices_report_list_item_bg_t::set_median_price_text(sys::state & state, std::string const& new_text) {
		auto table_source = (market_prices_report_body_t*)(parent->parent);
	if(new_text !=  median_price_cached_text) {
		median_price_cached_text = new_text;
		median_price_internal_layout.contents.clear();
		median_price_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ median_price_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->list_median_price_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->list_median_price_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, median_price_cached_text);
		}
	} else {
	}
}
void market_prices_report_list_item_bg_t::set_ratio_price_text(sys::state & state, std::string const& new_text) {
		auto table_source = (market_prices_report_body_t*)(parent->parent);
	if(new_text !=  ratio_price_cached_text) {
		ratio_price_cached_text = new_text;
		ratio_price_internal_layout.contents.clear();
		ratio_price_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ ratio_price_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->list_ratio_price_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->list_ratio_price_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, ratio_price_cached_text);
		}
	} else {
	}
}
void market_prices_report_list_item_bg_t::set_future_change_text(sys::state & state, std::string const& new_text) {
		auto table_source = (market_prices_report_body_t*)(parent->parent);
	if(new_text !=  future_change_cached_text) {
		future_change_cached_text = new_text;
		future_change_internal_layout.contents.clear();
		future_change_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ future_change_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->list_future_change_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->list_future_change_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, future_change_cached_text);
		}
	} else {
	}
}
void market_prices_report_list_item_bg_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto fh = text::make_font_id(state, false, 1.0f * 16);
	auto linesz = state.font_collection.line_height(state, fh); 
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto table_source = (market_prices_report_body_t*)(parent->parent);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - ui::get_absolute_location(state, *this).x;
	bool col_um_spacer = rel_mouse_x >= table_source->list_spacer_column_start && rel_mouse_x < (table_source->list_spacer_column_start + table_source->list_spacer_column_width);
	if(!spacer_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_spacer , false, false); 
		for(auto& t : spacer_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->list_spacer_column_start + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, spacer_text_color), cmod);
		}
	}
	bool col_um_name = rel_mouse_x >= table_source->list_name_column_start && rel_mouse_x < (table_source->list_name_column_start + table_source->list_name_column_width);
	if(!name_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_name , false, false); 
		for(auto& t : name_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->list_name_column_start + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, name_text_color), cmod);
		}
	}
	bool col_um_local_price = rel_mouse_x >= table_source->list_local_price_column_start && rel_mouse_x < (table_source->list_local_price_column_start + table_source->list_local_price_column_width);
	if(!local_price_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_local_price , false, false); 
		for(auto& t : local_price_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->list_local_price_column_start + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, local_price_text_color), cmod);
		}
	}
	bool col_um_median_price = rel_mouse_x >= table_source->list_median_price_column_start && rel_mouse_x < (table_source->list_median_price_column_start + table_source->list_median_price_column_width);
	if(!median_price_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_median_price , false, false); 
		for(auto& t : median_price_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->list_median_price_column_start + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, median_price_text_color), cmod);
		}
	}
	bool col_um_ratio_price = rel_mouse_x >= table_source->list_ratio_price_column_start && rel_mouse_x < (table_source->list_ratio_price_column_start + table_source->list_ratio_price_column_width);
	if(!ratio_price_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_ratio_price , false, false); 
		for(auto& t : ratio_price_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->list_ratio_price_column_start + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, ratio_price_text_color), cmod);
		}
	}
	bool col_um_future_change = rel_mouse_x >= table_source->list_future_change_column_start && rel_mouse_x < (table_source->list_future_change_column_start + table_source->list_future_change_column_width);
	if(!future_change_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_future_change , false, false); 
		for(auto& t : future_change_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->list_future_change_column_start + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, future_change_text_color), cmod);
		}
	}
}
void market_prices_report_list_item_bg_t::on_update(sys::state& state) noexcept {
	market_prices_report_list_item_t& list_item = *((market_prices_report_list_item_t*)(parent)); 
	market_prices_report_body_t& body = *((market_prices_report_body_t*)(parent->parent)); 
// BEGIN list_item::bg::update
	auto pid = state.map_state.get_selected_province();
	auto sid = state.world.province_get_state_membership(pid);
	auto mid = state.world.state_instance_get_market_from_local_market(sid);
	float supply = economy::supply(state, mid, list_item.cid);
	float demand = economy::demand(state, mid, list_item.cid);
	float local = economy::price(state, sid, list_item.cid);
	float median = state.world.commodity_get_median_price(list_item.cid);

	set_name_text(state, text::produce_simple_string(state, state.world.commodity_get_name(list_item.cid)));
	set_local_price_text(state, text::format_money(local));
	set_median_price_text(state, text::format_money(median));
	set_ratio_price_text(state, text::format_percentage(local / median));
	set_future_change_text(state, text::format_money(economy::price_properties::change<float>(local, supply, demand)));
// END
}
void market_prices_report_list_item_bg_t::on_create(sys::state& state) noexcept {
// BEGIN list_item::bg::create
// END
}
ui::message_result market_prices_report_list_item_icon_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result market_prices_report_list_item_icon_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void market_prices_report_list_item_icon_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void market_prices_report_list_item_icon_t::on_update(sys::state& state) noexcept {
	market_prices_report_list_item_t& list_item = *((market_prices_report_list_item_t*)(parent)); 
	market_prices_report_body_t& body = *((market_prices_report_body_t*)(parent->parent)); 
// BEGIN list_item::icon::update
	frame = state.world.commodity_get_icon(list_item.cid);
// END
}
void market_prices_report_list_item_icon_t::on_create(sys::state& state) noexcept {
	if(auto it = state.ui_state.gfx_by_name.find(state.lookup_key(gfx_key)); it != state.ui_state.gfx_by_name.end()) {
		background_gid = it->second;
	}
// BEGIN list_item::icon::create
// END
}
ui::message_result market_prices_report_list_item_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result market_prices_report_list_item_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void market_prices_report_list_item_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(is_active)
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, alt_background_texture, alt_texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	else
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	auto cmod = ui::get_color_modification(false, false,  false);
	for (auto& _item : textures_to_render) {
		if (_item.texture_type == background_type::texture)
			ogl::render_textured_rect(state, cmod, float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (_item.texture_type == background_type::border_texture_repeat)
			ogl::render_rect_with_repeated_border(state, cmod, float(8), float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (_item.texture_type == background_type::textured_corners)
			ogl::render_rect_with_repeated_corner(state, cmod, float(8), float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
	}
	auto table_source = (market_prices_report_body_t*)(parent);
	auto under_mouse = [&](){auto p = state.ui_state.under_mouse; while(p){ if(p == this) return true; p = p->parent; } return false;}();
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - ui::get_absolute_location(state, *this).x;
	if(under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), 0.772549f, 0.219608f, 0.219608f, 0.117647f);
	}
	bool col_um_spacer = rel_mouse_x >= table_source->list_spacer_column_start && rel_mouse_x < (table_source->list_spacer_column_start + table_source->list_spacer_column_width);
	if(col_um_spacer && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_spacer_column_start), float(y), float(table_source->list_spacer_column_width), float(base_data.size.y), 0.772549f, 0.219608f, 0.219608f, 0.117647f);
	}
	bool col_um_name = rel_mouse_x >= table_source->list_name_column_start && rel_mouse_x < (table_source->list_name_column_start + table_source->list_name_column_width);
	if(col_um_name && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_name_column_start), float(y), float(table_source->list_name_column_width), float(base_data.size.y), 0.772549f, 0.219608f, 0.219608f, 0.117647f);
	}
	bool col_um_local_price = rel_mouse_x >= table_source->list_local_price_column_start && rel_mouse_x < (table_source->list_local_price_column_start + table_source->list_local_price_column_width);
	if(col_um_local_price && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_local_price_column_start), float(y), float(table_source->list_local_price_column_width), float(base_data.size.y), 0.772549f, 0.219608f, 0.219608f, 0.117647f);
	}
	bool col_um_median_price = rel_mouse_x >= table_source->list_median_price_column_start && rel_mouse_x < (table_source->list_median_price_column_start + table_source->list_median_price_column_width);
	if(col_um_median_price && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_median_price_column_start), float(y), float(table_source->list_median_price_column_width), float(base_data.size.y), 0.772549f, 0.219608f, 0.219608f, 0.117647f);
	}
	bool col_um_ratio_price = rel_mouse_x >= table_source->list_ratio_price_column_start && rel_mouse_x < (table_source->list_ratio_price_column_start + table_source->list_ratio_price_column_width);
	if(col_um_ratio_price && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_ratio_price_column_start), float(y), float(table_source->list_ratio_price_column_width), float(base_data.size.y), 0.772549f, 0.219608f, 0.219608f, 0.117647f);
	}
	bool col_um_future_change = rel_mouse_x >= table_source->list_future_change_column_start && rel_mouse_x < (table_source->list_future_change_column_start + table_source->list_future_change_column_width);
	if(col_um_future_change && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->list_future_change_column_start), float(y), float(table_source->list_future_change_column_width), float(base_data.size.y), 0.772549f, 0.219608f, 0.219608f, 0.117647f);
	}
}
void market_prices_report_list_item_t::on_update(sys::state& state) noexcept {
	market_prices_report_body_t& body = *((market_prices_report_body_t*)(parent->parent)); 
// BEGIN list_item::update
// END
	remake_layout(state, true);
}
void market_prices_report_list_item_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "bg") {
					temp.ptr = bg.get();
				} else
				if(cname == "icon") {
					temp.ptr = icon.get();
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
					temp.ptr = make_market_prices_report_body(state);
				}
				if(cname == "list_header") {
					temp.ptr = make_market_prices_report_list_header(state);
				}
				if(cname == "list_item") {
					temp.ptr = make_market_prices_report_list_item(state);
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
void market_prices_report_list_item_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("market_prices_report::list_item"));
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
		if(child_data.name == "bg") {
			bg = std::make_unique<market_prices_report_list_item_bg_t>();
			bg->parent = this;
			auto cptr = bg.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "icon") {
			icon = std::make_unique<market_prices_report_list_item_icon_t>();
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
std::unique_ptr<ui::element_base> make_market_prices_report_list_item(sys::state& state) {
	auto ptr = std::make_unique<market_prices_report_list_item_t>();
	ptr->on_create(state);
	return ptr;
}
// LOST-CODE
}
