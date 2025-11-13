namespace alice_ui {
struct macrobuilder2_main_apply_button_t;
struct macrobuilder2_main_t;
struct macrobuilder2_list_item_select_button_t;
struct macrobuilder2_list_item_delete_button_t;
struct macrobuilder2_list_item_t;
struct macrobuilder2_grid_item_unit_icon_t;
struct macrobuilder2_grid_item_decrease_count_t;
struct macrobuilder2_grid_item_increase_count_t;
struct macrobuilder2_grid_item_current_count_t;
struct macrobuilder2_grid_item_t;
struct macrobuilder2_spacer_t;
struct macrobuilder2_main_apply_button_t : public alice_ui::template_text_button {
// BEGIN main::apply_button::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct macrobuilder2_main_template_list_g_t : public layout_generator {
// BEGIN main::template_list_g::variables
// END
	struct list_item_option { int32_t value; };
	std::vector<std::unique_ptr<ui::element_base>> list_item_pool;
	int32_t list_item_pool_used = 0;
	void add_list_item( int32_t value);
	std::vector<std::variant<std::monostate, list_item_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct macrobuilder2_main_unit_grid_g_t : public layout_generator {
// BEGIN main::unit_grid_g::variables
// END
	struct grid_item_option { dcon::unit_type_id value; };
	std::vector<std::unique_ptr<ui::element_base>> grid_item_pool;
	int32_t grid_item_pool_used = 0;
	void add_grid_item( dcon::unit_type_id value);
	struct spacer_option { };
	std::vector<std::unique_ptr<ui::element_base>> spacer_pool;
	int32_t spacer_pool_used = 0;
	void add_spacer();
	std::vector<std::variant<std::monostate, grid_item_option, spacer_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct macrobuilder2_list_item_select_button_t : public alice_ui::template_mixed_button {
// BEGIN list_item::select_button::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct macrobuilder2_list_item_delete_button_t : public alice_ui::template_icon_button {
// BEGIN list_item::delete_button::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct macrobuilder2_grid_item_unit_icon_t : public ui::element_base {
// BEGIN grid_item::unit_icon::variables
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
struct macrobuilder2_grid_item_decrease_count_t : public alice_ui::template_icon_button {
// BEGIN grid_item::decrease_count::variables
// END
	bool button_action(sys::state& state) noexcept override;
	bool button_shift_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct macrobuilder2_grid_item_increase_count_t : public alice_ui::template_icon_button {
// BEGIN grid_item::increase_count::variables
// END
	bool button_action(sys::state& state) noexcept override;
	bool button_shift_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct macrobuilder2_grid_item_current_count_t : public alice_ui::template_label {
// BEGIN grid_item::current_count::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct macrobuilder2_main_t : public layout_window_element {
// BEGIN main::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<template_label> header_text;
	std::unique_ptr<macrobuilder2_main_apply_button_t> apply_button;
	macrobuilder2_main_template_list_g_t template_list_g;
	macrobuilder2_main_unit_grid_g_t unit_grid_g;
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
std::unique_ptr<ui::element_base> make_macrobuilder2_main(sys::state& state);
struct macrobuilder2_list_item_t : public layout_window_element {
// BEGIN list_item::variables
// END
	int32_t value;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<macrobuilder2_list_item_select_button_t> select_button;
	std::unique_ptr<macrobuilder2_list_item_delete_button_t> delete_button;
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
std::unique_ptr<ui::element_base> make_macrobuilder2_list_item(sys::state& state);
struct macrobuilder2_grid_item_t : public layout_window_element {
// BEGIN grid_item::variables
// END
	dcon::unit_type_id value;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<macrobuilder2_grid_item_unit_icon_t> unit_icon;
	std::unique_ptr<macrobuilder2_grid_item_decrease_count_t> decrease_count;
	std::unique_ptr<macrobuilder2_grid_item_increase_count_t> increase_count;
	std::unique_ptr<macrobuilder2_grid_item_current_count_t> current_count;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
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
std::unique_ptr<ui::element_base> make_macrobuilder2_grid_item(sys::state& state);
struct macrobuilder2_spacer_t : public ui::non_owning_container_base {
// BEGIN spacer::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		return ui::message_result::unseen;
	}
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_macrobuilder2_spacer(sys::state& state);
void macrobuilder2_main_template_list_g_t::add_list_item(int32_t value) {
	values.emplace_back(list_item_option{value});
}
void  macrobuilder2_main_template_list_g_t::on_create(sys::state& state, layout_window_element* parent) {
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent)); 
// BEGIN main::template_list_g::on_create
	auto sdir = simple_fs::get_or_create_templates_directory();
	auto f = simple_fs::open_file(sdir, state.loaded_scenario_file);
	if(f) {
		auto contents = simple_fs::view_contents(*f);

		uint32_t num_templates = contents.file_size / sizeof(sys::macro_builder_template);
		//Corruption protection
		if(num_templates >= 8192 * 4)
			num_templates = 8192 * 4;
		state.ui_state.templates.resize(num_templates);
		std::memcpy(state.ui_state.templates.data(), contents.data, num_templates * sizeof(sys::macro_builder_template));

		for(int32_t i = -1; i < int32_t(num_templates); ++i) {
			add_list_item(i);
		}
	} else {
		add_list_item(-1);
	}
// END
}
void  macrobuilder2_main_template_list_g_t::update(sys::state& state, layout_window_element* parent) {
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent)); 
// BEGIN main::template_list_g::update
// END
}
measure_result  macrobuilder2_main_template_list_g_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<list_item_option>(values[index])) {
		if(list_item_pool.empty()) list_item_pool.emplace_back(make_macrobuilder2_list_item(state));
		if(destination) {
			if(list_item_pool.size() <= size_t(list_item_pool_used)) list_item_pool.emplace_back(make_macrobuilder2_list_item(state));
			list_item_pool[list_item_pool_used]->base_data.position.x = int16_t(x);
			list_item_pool[list_item_pool_used]->base_data.position.y = int16_t(y);
			list_item_pool[list_item_pool_used]->parent = destination;
			destination->children.push_back(list_item_pool[list_item_pool_used].get());
			((macrobuilder2_list_item_t*)(list_item_pool[list_item_pool_used].get()))->value = std::get<list_item_option>(values[index]).value;
			((macrobuilder2_list_item_t*)(list_item_pool[list_item_pool_used].get()))->set_alternate(alternate);
			list_item_pool[list_item_pool_used]->impl_on_update(state);
			list_item_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ list_item_pool[0]->base_data.size.x, list_item_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  macrobuilder2_main_template_list_g_t::reset_pools() {
	list_item_pool_used = 0;
}
void macrobuilder2_main_unit_grid_g_t::add_grid_item(dcon::unit_type_id value) {
	values.emplace_back(grid_item_option{value});
}
void macrobuilder2_main_unit_grid_g_t::add_spacer() {
	values.emplace_back(spacer_option{});
}
void  macrobuilder2_main_unit_grid_g_t::on_create(sys::state& state, layout_window_element* parent) {
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent)); 
// BEGIN main::unit_grid_g::on_create
	for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size() && i < sys::macro_builder_template::max_types; ++i) {
		dcon::unit_type_id id{ dcon::unit_type_id::value_base_t(i) };
		if(state.military_definitions.unit_base_definitions[id].is_land) {
			add_grid_item(id);
		}
	}
	add_spacer();
	for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size() && i < sys::macro_builder_template::max_types; ++i) {
		dcon::unit_type_id id{ dcon::unit_type_id::value_base_t(i) };
		if(!state.military_definitions.unit_base_definitions[id].is_land) {
			add_grid_item(id);
		}
	}
// END
}
void  macrobuilder2_main_unit_grid_g_t::update(sys::state& state, layout_window_element* parent) {
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent)); 
// BEGIN main::unit_grid_g::update
// END
}
measure_result  macrobuilder2_main_unit_grid_g_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<grid_item_option>(values[index])) {
		if(grid_item_pool.empty()) grid_item_pool.emplace_back(make_macrobuilder2_grid_item(state));
		if(destination) {
			if(grid_item_pool.size() <= size_t(grid_item_pool_used)) grid_item_pool.emplace_back(make_macrobuilder2_grid_item(state));
			grid_item_pool[grid_item_pool_used]->base_data.position.x = int16_t(x);
			grid_item_pool[grid_item_pool_used]->base_data.position.y = int16_t(y);
			grid_item_pool[grid_item_pool_used]->parent = destination;
			destination->children.push_back(grid_item_pool[grid_item_pool_used].get());
			((macrobuilder2_grid_item_t*)(grid_item_pool[grid_item_pool_used].get()))->value = std::get<grid_item_option>(values[index]).value;
			grid_item_pool[grid_item_pool_used]->impl_on_update(state);
			grid_item_pool_used++;
		}
		alternate = true;
		return measure_result{ grid_item_pool[0]->base_data.size.x, grid_item_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	if(std::holds_alternative<spacer_option>(values[index])) {
		if(spacer_pool.empty()) spacer_pool.emplace_back(make_macrobuilder2_spacer(state));
		if(destination) {
			if(spacer_pool.size() <= size_t(spacer_pool_used)) spacer_pool.emplace_back(make_macrobuilder2_spacer(state));
			spacer_pool[spacer_pool_used]->base_data.position.x = int16_t(x);
			spacer_pool[spacer_pool_used]->base_data.position.y = int16_t(y);
			spacer_pool[spacer_pool_used]->parent = destination;
			destination->children.push_back(spacer_pool[spacer_pool_used].get());
			spacer_pool[spacer_pool_used]->impl_on_update(state);
			spacer_pool_used++;
		}
		alternate = true;
		return measure_result{ spacer_pool[0]->base_data.size.x, spacer_pool[0]->base_data.size.y + 0, measure_result::special::end_page};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  macrobuilder2_main_unit_grid_g_t::reset_pools() {
	grid_item_pool_used = 0;
	spacer_pool_used = 0;
}
void macrobuilder2_main_apply_button_t::on_update(sys::state& state) noexcept {
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent)); 
// BEGIN main::apply_button::update
	disabled = (state.ui_state.current_template == -1 || state.map_state.selected_province == dcon::province_id{} || state.world.province_get_nation_from_province_ownership(state.map_state.selected_province) != state.local_player_nation || state.world.province_get_nation_from_province_control(state.map_state.selected_province) != state.local_player_nation);
// END
}
bool macrobuilder2_main_apply_button_t::button_action(sys::state& state) noexcept {
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent)); 
// BEGIN main::apply_button::lbutton_action
	if(state.ui_state.current_template < 0 || state.ui_state.current_template >= int32_t(state.ui_state.templates.size()))
		return true;

	auto const& t = state.ui_state.templates[state.ui_state.current_template];

	bool is_land = true;
	for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size() && i < sys::macro_builder_template::max_types; ++i) {
		dcon::unit_type_id id{ dcon::unit_type_id::value_base_t(i) };
		if(!state.military_definitions.unit_base_definitions[id].is_land && t.amounts[i] > 0) {
			is_land = false;
		}
	}

	const auto template_province = state.map_state.selected_province;

	std::vector<dcon::province_id> provinces;
	state.fill_vector_of_connected_provinces(state.map_state.selected_province, is_land, provinces);
	if(provinces.empty())
		return true;

	if(is_land) {
		std::array<uint8_t, sys::macro_builder_template::max_types> current_distribution;
		current_distribution.fill(0);
		state.build_up_to_template_land(
			t,
			state.map_state.selected_province,
			provinces,
			current_distribution
		);
	} else {
		uint8_t rem_to_build[sys::macro_builder_template::max_types];
		std::memcpy(rem_to_build, t.amounts, sizeof(rem_to_build));

		std::sort(provinces.begin(), provinces.end(), [&state](auto const a, auto const b) {
			auto ab = state.world.province_get_province_naval_construction_as_province(a);
			auto bb = state.world.province_get_province_naval_construction_as_province(b);
			int32_t asz = int32_t(ab.end() - ab.begin());
			int32_t bsz = int32_t(bb.end() - bb.begin());
			if(asz == bsz)
				return a.index() < b.index();
			return asz < bsz;
		});
		for(dcon::unit_type_id::value_base_t i = 0; i < sys::macro_builder_template::max_types; i++) {
			dcon::unit_type_id utid = dcon::unit_type_id(i);
			if(rem_to_build[i] > 0
			&& is_land == state.military_definitions.unit_base_definitions[utid].is_land
			&& (state.military_definitions.unit_base_definitions[utid].active || state.world.nation_get_active_unit(state.local_player_nation, utid))) {
				for(const auto prov : provinces) {
					auto const port_level = state.world.province_get_building_level(prov, uint8_t(economy::province_building_type::naval_base));
					if(port_level >= state.military_definitions.unit_base_definitions[utid].min_port_level
					&& command::can_start_naval_unit_construction(state, state.local_player_nation, prov, utid, template_province)) {
						command::start_naval_unit_construction(state, state.local_player_nation, prov, utid, template_province);
						rem_to_build[i]--;
						if(rem_to_build[i] == 0)
							break;
					}
				}
				// sort provinces again so that the ships can be built on parallel
				std::sort(provinces.begin(), provinces.end(), [&state](auto const a, auto const b) {
					auto ab = state.world.province_get_province_naval_construction_as_province(a);
					auto bb = state.world.province_get_province_naval_construction_as_province(b);
					int32_t asz = int32_t(ab.end() - ab.begin());
					int32_t bsz = int32_t(bb.end() - bb.begin());
					if(asz == bsz)
						return a.index() < b.index();
					return asz < bsz;
				});
			}
		}
	}
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
ui::message_result macrobuilder2_main_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	state.ui_state.drag_target = this;
	return ui::message_result::consumed;
}
ui::message_result macrobuilder2_main_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void macrobuilder2_main_t::on_update(sys::state& state) noexcept {
// BEGIN main::update
// END
	template_list_g.update(state, this);
	unit_grid_g.update(state, this);
	remake_layout(state, true);
}
void macrobuilder2_main_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "header_text") {
					temp.ptr = header_text.get();
				} else
				if(cname == "apply_button") {
					temp.ptr = apply_button.get();
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
					temp.ptr = make_macrobuilder2_main(state);
				}
				if(cname == "list_item") {
					temp.ptr = make_macrobuilder2_list_item(state);
				}
				if(cname == "grid_item") {
					temp.ptr = make_macrobuilder2_grid_item(state);
				}
				if(cname == "spacer") {
					temp.ptr = make_macrobuilder2_spacer(state);
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
				if(cname == "template_list_g") {
					temp.generator = &template_list_g;
				}
				if(cname == "unit_grid_g") {
					temp.generator = &unit_grid_g;
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
void macrobuilder2_main_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("macrobuilder2::main"));
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
			header_text = std::make_unique<template_label>();
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
		if(child_data.name == "apply_button") {
			apply_button = std::make_unique<macrobuilder2_main_apply_button_t>();
			apply_button->parent = this;
			auto cptr = apply_button.get();
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
	template_list_g.on_create(state, this);
	unit_grid_g.on_create(state, this);
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN main::create
// END
}
std::unique_ptr<ui::element_base> make_macrobuilder2_main(sys::state& state) {
	auto ptr = std::make_unique<macrobuilder2_main_t>();
	ptr->on_create(state);
	return ptr;
}
void macrobuilder2_list_item_select_button_t::on_update(sys::state& state) noexcept {
	macrobuilder2_list_item_t& list_item = *((macrobuilder2_list_item_t*)(parent)); 
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent)); 
// BEGIN list_item::select_button::update
	static const int32_t select_icon = template_project::icon_by_name(state.ui_templates, "select_tab.svg");

	if(list_item.value == state.ui_state.current_template) {
		icon_id = int16_t(select_icon);
	} else {
		icon_id = -1;
	}
	if(list_item.value == -1) {
		set_text(state, text::produce_simple_string(state, "macro_new_template"));
		return;
	}
	if(list_item.value >= int32_t(state.ui_state.templates.size()))
		return;

	std::string accumulated;
	for(dcon::unit_type_id::value_base_t i = 2; i < state.military_definitions.unit_base_definitions.size() && i < sys::macro_builder_template::max_types; i++) {
		auto amount = state.ui_state.templates[list_item.value].amounts[i];
		if(amount < 1) {
			continue;
		}
		accumulated += std::to_string(amount);
		accumulated += "@*" + std::string(i < 10 ? "0" : "") + std::to_string(i);
	}
	set_text(state, accumulated);
// END
}
bool macrobuilder2_list_item_select_button_t::button_action(sys::state& state) noexcept {
	macrobuilder2_list_item_t& list_item = *((macrobuilder2_list_item_t*)(parent)); 
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent)); 
// BEGIN list_item::select_button::lbutton_action
	state.ui_state.current_template = list_item.value;
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
void macrobuilder2_list_item_delete_button_t::on_update(sys::state& state) noexcept {
	macrobuilder2_list_item_t& list_item = *((macrobuilder2_list_item_t*)(parent)); 
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent)); 
// BEGIN list_item::delete_button::update
	if(list_item.value == state.ui_state.current_template && list_item.value != -1) {
		element_base::flags &= ~element_base::is_invisible_mask;
	} else {
		element_base::flags |= element_base::is_invisible_mask;
	}
// END
}
bool macrobuilder2_list_item_delete_button_t::button_action(sys::state& state) noexcept {
	macrobuilder2_list_item_t& list_item = *((macrobuilder2_list_item_t*)(parent)); 
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent)); 
// BEGIN list_item::delete_button::lbutton_action
	if(list_item.value != -1) {
		if(list_item.value == state.ui_state.current_template) {
			state.ui_state.current_template = -1;
		}
		state.ui_state.templates.erase(state.ui_state.templates.begin() + list_item.value);
		main.template_list_g.values.pop_back();
		main.impl_on_update(state);
	}
// END
	return true;
}
void  macrobuilder2_list_item_t::set_alternate(bool alt) noexcept {
	window_template = alt ? -1 : 2;
	select_button->template_id = alt ? 0 : 1;
}
ui::message_result macrobuilder2_list_item_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result macrobuilder2_list_item_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void macrobuilder2_list_item_t::on_update(sys::state& state) noexcept {
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent)); 
// BEGIN list_item::update
// END
	remake_layout(state, true);
}
void macrobuilder2_list_item_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "select_button") {
					temp.ptr = select_button.get();
				} else
				if(cname == "delete_button") {
					temp.ptr = delete_button.get();
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
					temp.ptr = make_macrobuilder2_main(state);
				}
				if(cname == "list_item") {
					temp.ptr = make_macrobuilder2_list_item(state);
				}
				if(cname == "grid_item") {
					temp.ptr = make_macrobuilder2_grid_item(state);
				}
				if(cname == "spacer") {
					temp.ptr = make_macrobuilder2_spacer(state);
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
void macrobuilder2_list_item_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("macrobuilder2::list_item"));
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
		if(child_data.name == "select_button") {
			select_button = std::make_unique<macrobuilder2_list_item_select_button_t>();
			select_button->parent = this;
			auto cptr = select_button.get();
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
		if(child_data.name == "delete_button") {
			delete_button = std::make_unique<macrobuilder2_list_item_delete_button_t>();
			delete_button->parent = this;
			auto cptr = delete_button.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->flags |= ui::element_base::wants_update_when_hidden_mask;
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
// BEGIN list_item::create
// END
}
std::unique_ptr<ui::element_base> make_macrobuilder2_list_item(sys::state& state) {
	auto ptr = std::make_unique<macrobuilder2_list_item_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result macrobuilder2_grid_item_unit_icon_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result macrobuilder2_grid_item_unit_icon_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void macrobuilder2_grid_item_unit_icon_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	macrobuilder2_grid_item_t& grid_item = *((macrobuilder2_grid_item_t*)(parent)); 
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent)); 
// BEGIN grid_item::unit_icon::tooltip
	text::add_line(state, contents, state.military_definitions.unit_base_definitions[grid_item.value].name);
	text::add_line_break_to_layout(state, contents);
	auto utid = grid_item.value;
	auto is_land = state.military_definitions.unit_base_definitions[utid].is_land;

	float reconnaissance_or_fire_range = 0.f;
	float siege_or_torpedo_attack = 0.f;
	float attack_or_gun_power = 0.f;
	float defence_or_hull = 0.f;
	float discipline_or_evasion = std::numeric_limits<float>::max();
	float support = 0.f;
	float supply_consumption = 0.f;
	float maximum_speed = std::numeric_limits<float>::max();
	float maneuver = std::numeric_limits<float>::max();
	int32_t supply_consumption_score = 0;
	bool warn_overseas = false;
	bool warn_culture = false;
	bool warn_active = false;

	

		if(!state.military_definitions.unit_base_definitions[utid].active && !state.world.nation_get_active_unit(state.local_player_nation, utid))
			warn_active = true;
		if(state.military_definitions.unit_base_definitions[utid].primary_culture)
			warn_culture = true;
		if(!state.military_definitions.unit_base_definitions[utid].can_build_overseas)
			warn_overseas = true;

		reconnaissance_or_fire_range += state.world.nation_get_unit_stats(state.local_player_nation, utid).reconnaissance_or_fire_range;
		siege_or_torpedo_attack += state.world.nation_get_unit_stats(state.local_player_nation, utid).siege_or_torpedo_attack;
		attack_or_gun_power += state.world.nation_get_unit_stats(state.local_player_nation, utid).attack_or_gun_power;
		defence_or_hull += state.world.nation_get_unit_stats(state.local_player_nation, utid).defence_or_hull ;
		discipline_or_evasion += std::min(discipline_or_evasion, state.world.nation_get_unit_stats(state.local_player_nation, utid).discipline_or_evasion);
		supply_consumption += state.world.nation_get_unit_stats(state.local_player_nation, utid).supply_consumption;
		maximum_speed = std::min(maximum_speed, state.world.nation_get_unit_stats(state.local_player_nation, utid).maximum_speed);
		if(is_land) {
			support += state.world.nation_get_unit_stats(state.local_player_nation, utid).support ;
			maneuver += state.world.nation_get_unit_stats(state.local_player_nation, utid).maneuver;
		} else {
			supply_consumption_score += state.military_definitions.unit_base_definitions[utid].supply_consumption_score;
		}
	

	if(warn_overseas)
		text::add_line(state, contents, "macro_warn_overseas");
	if(warn_culture)
		text::add_line(state, contents, "macro_warn_culture");
	if(warn_active)
		text::add_line(state, contents, "macro_warn_unlocked");
	
	if(maximum_speed == std::numeric_limits<float>::max()) maximum_speed = 0.f;
	if(discipline_or_evasion == std::numeric_limits<float>::max()) discipline_or_evasion = 0.f;
	if(maneuver == std::numeric_limits<float>::max()) maneuver = 0.f;
	if(is_land) {
		if(reconnaissance_or_fire_range > 0.f) {
			text::add_line(state, contents, "unit_recon", text::variable_type::x, text::format_float(reconnaissance_or_fire_range, 2));
		}
		if(siege_or_torpedo_attack > 0.f) {
			text::add_line(state, contents, "unit_siege", text::variable_type::x, text::format_float(siege_or_torpedo_attack, 2));
		}
		text::add_line(state, contents, "unit_attack", text::variable_type::x, text::format_float(attack_or_gun_power, 2));
		text::add_line(state, contents, "unit_defence", text::variable_type::x, text::format_float(defence_or_hull, 2));
		text::add_line(state, contents, "unit_discipline", text::variable_type::x, text::format_percentage(discipline_or_evasion, 0));
		if(support > 0.f) {
			text::add_line(state, contents, "unit_support", text::variable_type::x, text::format_float(support, 0));
		}
		text::add_line(state, contents, "unit_maneuver", text::variable_type::x, text::format_float(maneuver, 0));
		text::add_line(state, contents, "unit_max_speed", text::variable_type::x, text::format_float(maximum_speed, 2));
		text::add_line(state, contents, "unit_supply_consumption", text::variable_type::x, text::format_percentage(supply_consumption, 0));
	} else {
		text::add_line(state, contents, "unit_max_speed", text::variable_type::x, text::format_float(maximum_speed, 2));
		text::add_line(state, contents, "unit_attack", text::variable_type::x, text::format_float(attack_or_gun_power, 2));
		if(siege_or_torpedo_attack > 0.f) {
			text::add_line(state, contents, "unit_torpedo_attack", text::variable_type::x, text::format_float(siege_or_torpedo_attack, 2));
		}
		text::add_line(state, contents, "unit_hull", text::variable_type::x, text::format_float(defence_or_hull, 2));
		text::add_line(state, contents, "unit_fire_range", text::variable_type::x, text::format_float(reconnaissance_or_fire_range, 2));
		if(discipline_or_evasion > 0.f) {
			text::add_line(state, contents, "unit_evasion", text::variable_type::x, text::format_percentage(discipline_or_evasion, 0));
		}
		text::add_line(state, contents, "unit_supply_consumption", text::variable_type::x, text::format_percentage(supply_consumption, 0));
		text::add_line(state, contents, "unit_supply_load", text::variable_type::x, supply_consumption_score);
	}
// END
}
void macrobuilder2_grid_item_unit_icon_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void macrobuilder2_grid_item_unit_icon_t::on_update(sys::state& state) noexcept {
	macrobuilder2_grid_item_t& grid_item = *((macrobuilder2_grid_item_t*)(parent)); 
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent)); 
// BEGIN grid_item::unit_icon::update
	if(!grid_item.value)
		return;
	frame = state.military_definitions.unit_base_definitions[grid_item.value].icon - 1;
// END
}
void macrobuilder2_grid_item_unit_icon_t::on_create(sys::state& state) noexcept {
	if(auto it = state.ui_state.gfx_by_name.find(state.lookup_key(gfx_key)); it != state.ui_state.gfx_by_name.end()) {
		background_gid = it->second;
	}
// BEGIN grid_item::unit_icon::create
// END
}
void macrobuilder2_grid_item_decrease_count_t::on_update(sys::state& state) noexcept {
	macrobuilder2_grid_item_t& grid_item = *((macrobuilder2_grid_item_t*)(parent)); 
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent)); 
// BEGIN grid_item::decrease_count::update
	if(!grid_item.value)
		return;

	if(state.ui_state.current_template == -1 || state.ui_state.templates[state.ui_state.current_template].amounts[grid_item.value.index()] == 0) {
		disabled = true;
	} else {
		disabled = false;
	}
// END
}
bool macrobuilder2_grid_item_decrease_count_t::button_action(sys::state& state) noexcept {
	macrobuilder2_grid_item_t& grid_item = *((macrobuilder2_grid_item_t*)(parent)); 
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent)); 
// BEGIN grid_item::decrease_count::lbutton_action
	if(state.ui_state.current_template != -1) {
		auto& v = state.ui_state.templates[state.ui_state.current_template].amounts[grid_item.value.index()];
		v = uint8_t(std::clamp(v - 1, 0, 255));
	}
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
bool macrobuilder2_grid_item_decrease_count_t::button_shift_action(sys::state& state) noexcept {
	macrobuilder2_grid_item_t& grid_item = *((macrobuilder2_grid_item_t*)(parent)); 
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent)); 
// BEGIN grid_item::decrease_count::lbutton_shift_action
		if(state.ui_state.current_template != -1) {
			auto& v = state.ui_state.templates[state.ui_state.current_template].amounts[grid_item.value.index()];
			v = uint8_t(std::clamp(v - 10, 0, 255));
		}
		state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
void macrobuilder2_grid_item_increase_count_t::on_update(sys::state& state) noexcept {
	macrobuilder2_grid_item_t& grid_item = *((macrobuilder2_grid_item_t*)(parent)); 
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent)); 
// BEGIN grid_item::increase_count::update
// END
}
bool macrobuilder2_grid_item_increase_count_t::button_action(sys::state& state) noexcept {
	macrobuilder2_grid_item_t& grid_item = *((macrobuilder2_grid_item_t*)(parent)); 
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent)); 
// BEGIN grid_item::increase_count::lbutton_action
	if(state.ui_state.current_template != -1) {
		auto& v = state.ui_state.templates[state.ui_state.current_template].amounts[grid_item.value.index()];
		v = uint8_t(std::clamp(v + 1, 0, 255));
	} else {
		state.ui_state.templates.emplace_back();
		state.ui_state.current_template = int32_t(state.ui_state.templates.size() - 1);
		main.template_list_g.add_list_item(int32_t(state.ui_state.templates.size() - 1));
		auto& v = state.ui_state.templates[state.ui_state.current_template].amounts[grid_item.value.index()];
		v = uint8_t(std::clamp(v + 1, 0, 255));
		main.impl_on_update(state);
	}
	bool is_land = state.military_definitions.unit_base_definitions[grid_item.value].is_land;
	for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size() && i < sys::macro_builder_template::max_types; ++i) {
		dcon::unit_type_id id{ dcon::unit_type_id::value_base_t(i) };
		if(is_land != state.military_definitions.unit_base_definitions[id].is_land) {
			state.ui_state.templates[state.ui_state.current_template].amounts[i] = 0;
		}
	}
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
bool macrobuilder2_grid_item_increase_count_t::button_shift_action(sys::state& state) noexcept {
	macrobuilder2_grid_item_t& grid_item = *((macrobuilder2_grid_item_t*)(parent)); 
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent)); 
// BEGIN grid_item::increase_count::lbutton_shift_action
		if(state.ui_state.current_template != -1) {
			auto& v = state.ui_state.templates[state.ui_state.current_template].amounts[grid_item.value.index()];
			v = uint8_t(std::clamp(v + 10, 0, 255));
		} else {
			state.ui_state.templates.emplace_back();
			state.ui_state.current_template = int32_t(state.ui_state.templates.size() - 1);
			main.template_list_g.add_list_item(int32_t(state.ui_state.templates.size() - 1));
			auto& v = state.ui_state.templates[state.ui_state.current_template].amounts[grid_item.value.index()];
			v = uint8_t(std::clamp(v + 10, 0, 255));
			main.impl_on_update(state);
		}
		bool is_land = state.military_definitions.unit_base_definitions[grid_item.value].is_land;
		for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size() && i < sys::macro_builder_template::max_types; ++i) {
			dcon::unit_type_id id{ dcon::unit_type_id::value_base_t(i) };
			if(is_land != state.military_definitions.unit_base_definitions[id].is_land) {
				state.ui_state.templates[state.ui_state.current_template].amounts[i] = 0;
			}
		}
		state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
void macrobuilder2_grid_item_current_count_t::on_update(sys::state& state) noexcept {
	macrobuilder2_grid_item_t& grid_item = *((macrobuilder2_grid_item_t*)(parent)); 
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent)); 
// BEGIN grid_item::current_count::update
	if(!grid_item.value)
		return;
	if(state.ui_state.current_template != -1) {
		auto& v = state.ui_state.templates[state.ui_state.current_template].amounts[grid_item.value.index()];
		set_text(state, std::to_string(v));
	} else {
		set_text(state, "0");
	}
// END
}
ui::message_result macrobuilder2_grid_item_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result macrobuilder2_grid_item_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void macrobuilder2_grid_item_t::on_update(sys::state& state) noexcept {
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent)); 
// BEGIN grid_item::update
	if(!value) {
		flags |= element_base::is_invisible_mask;
	}
// END
	remake_layout(state, true);
}
void macrobuilder2_grid_item_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "unit_icon") {
					temp.ptr = unit_icon.get();
				} else
				if(cname == "decrease_count") {
					temp.ptr = decrease_count.get();
				} else
				if(cname == "increase_count") {
					temp.ptr = increase_count.get();
				} else
				if(cname == "current_count") {
					temp.ptr = current_count.get();
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
					temp.ptr = make_macrobuilder2_main(state);
				}
				if(cname == "list_item") {
					temp.ptr = make_macrobuilder2_list_item(state);
				}
				if(cname == "grid_item") {
					temp.ptr = make_macrobuilder2_grid_item(state);
				}
				if(cname == "spacer") {
					temp.ptr = make_macrobuilder2_spacer(state);
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
void macrobuilder2_grid_item_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("macrobuilder2::grid_item"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	layout_window_element::initialize_template(state, win_data.template_id, win_data.grid_size, win_data.auto_close_button);
	ui::element_base::flags |= ui::element_base::wants_update_when_hidden_mask;
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		if(child_data.name == "unit_icon") {
			unit_icon = std::make_unique<macrobuilder2_grid_item_unit_icon_t>();
			unit_icon->parent = this;
			auto cptr = unit_icon.get();
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
		if(child_data.name == "decrease_count") {
			decrease_count = std::make_unique<macrobuilder2_grid_item_decrease_count_t>();
			decrease_count->parent = this;
			auto cptr = decrease_count.get();
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
		if(child_data.name == "increase_count") {
			increase_count = std::make_unique<macrobuilder2_grid_item_increase_count_t>();
			increase_count->parent = this;
			auto cptr = increase_count.get();
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
		if(child_data.name == "current_count") {
			current_count = std::make_unique<macrobuilder2_grid_item_current_count_t>();
			current_count->parent = this;
			auto cptr = current_count.get();
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
// BEGIN grid_item::create
// END
}
std::unique_ptr<ui::element_base> make_macrobuilder2_grid_item(sys::state& state) {
	auto ptr = std::make_unique<macrobuilder2_grid_item_t>();
	ptr->on_create(state);
	return ptr;
}
void macrobuilder2_spacer_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
}
void macrobuilder2_spacer_t::on_update(sys::state& state) noexcept {
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent)); 
// BEGIN spacer::update
// END
}
void macrobuilder2_spacer_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("macrobuilder2::spacer"));
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
// BEGIN spacer::create
// END
}
std::unique_ptr<ui::element_base> make_macrobuilder2_spacer(sys::state& state) {
	auto ptr = std::make_unique<macrobuilder2_spacer_t>();
	ptr->on_create(state);
	return ptr;
}
// LOST-CODE
}
