namespace alice_ui {
struct gamerules_main_t;
struct gamerules_gamerule_item_desc_text_t;
struct gamerules_gamerule_item_current_setting_t;
struct gamerules_gamerule_item_t;
struct gamerules_gamerule_opt_option_name_t;
struct gamerules_gamerule_opt_t;
struct gamerules_main_gamerule_grid_g_t : public layout_generator {
// BEGIN main::gamerule_grid_g::variables
// END
	struct gamerule_item_option { dcon::gamerule_id value; };
	std::vector<std::unique_ptr<ui::element_base>> gamerule_item_pool;
	int32_t gamerule_item_pool_used = 0;
	void add_gamerule_item( dcon::gamerule_id value);
	std::vector<std::variant<std::monostate, gamerule_item_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct gamerules_gamerule_item_desc_text_t : public alice_ui::template_label {
// BEGIN gamerule_item::desc_text::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct gamerules_gamerule_item_current_setting_t : public alice_ui::template_drop_down_control {
// BEGIN gamerule_item::current_setting::variables
// END
	struct gamerule_opt_option { dcon::gamerule_id rule; uint8_t option; };
	std::vector<gamerule_opt_option> list_contents;
	std::vector<std::unique_ptr<gamerules_gamerule_opt_t >> list_pool;
	std::unique_ptr<gamerules_gamerule_opt_t> label_window_internal;
	void add_item( dcon::gamerule_id rule,  uint8_t option);
	ui::element_base* get_nth_item(sys::state& state, int32_t id, int32_t pool_id) override;
	void quiet_on_selection(sys::state& state, int32_t id);
	void on_selection(sys::state& state, int32_t id) override;
	void clear_list();
	void on_create(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct gamerules_gamerule_opt_option_name_t : public alice_ui::template_label {
// BEGIN gamerule_opt::option_name::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct gamerules_main_t : public layout_window_element {
// BEGIN main::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<template_label> header_text;
	gamerules_main_gamerule_grid_g_t gamerule_grid_g;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void on_hide(sys::state& state) noexcept override;
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
std::unique_ptr<ui::element_base> make_gamerules_main(sys::state& state);
struct gamerules_gamerule_item_t : public layout_window_element {
// BEGIN gamerule_item::variables
// END
	dcon::gamerule_id value;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<gamerules_gamerule_item_desc_text_t> desc_text;
	std::unique_ptr<gamerules_gamerule_item_current_setting_t> current_setting;
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
std::unique_ptr<ui::element_base> make_gamerules_gamerule_item(sys::state& state);
struct gamerules_gamerule_opt_t : public layout_window_element {
// BEGIN gamerule_opt::variables
// END
	dcon::gamerule_id rule;
	uint8_t option;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<gamerules_gamerule_opt_option_name_t> option_name;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "rule") {
			return (void*)(&rule);
		}
		if(name_parameter == "option") {
			return (void*)(&option);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_gamerules_gamerule_opt(sys::state& state);
void gamerules_main_gamerule_grid_g_t::add_gamerule_item(dcon::gamerule_id value) {
	values.emplace_back(gamerule_item_option{value});
}
void  gamerules_main_gamerule_grid_g_t::on_create(sys::state& state, layout_window_element* parent) {
	gamerules_main_t& main = *((gamerules_main_t*)(parent)); 
// BEGIN main::gamerule_grid_g::on_create
	for(auto gamerule : state.world.in_gamerule) {
		if(state.world.gamerule_get_settings_count(gamerule) > 0)
			add_gamerule_item(gamerule);
	}
// END
}
void  gamerules_main_gamerule_grid_g_t::update(sys::state& state, layout_window_element* parent) {
	gamerules_main_t& main = *((gamerules_main_t*)(parent)); 
// BEGIN main::gamerule_grid_g::update
// END
}
measure_result  gamerules_main_gamerule_grid_g_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<gamerule_item_option>(values[index])) {
		if(gamerule_item_pool.empty()) gamerule_item_pool.emplace_back(make_gamerules_gamerule_item(state));
		if(destination) {
			if(gamerule_item_pool.size() <= size_t(gamerule_item_pool_used)) gamerule_item_pool.emplace_back(make_gamerules_gamerule_item(state));
			gamerule_item_pool[gamerule_item_pool_used]->base_data.position.x = int16_t(x);
			gamerule_item_pool[gamerule_item_pool_used]->base_data.position.y = int16_t(y);
			gamerule_item_pool[gamerule_item_pool_used]->parent = destination;
			destination->children.push_back(gamerule_item_pool[gamerule_item_pool_used].get());
			((gamerules_gamerule_item_t*)(gamerule_item_pool[gamerule_item_pool_used].get()))->value = std::get<gamerule_item_option>(values[index]).value;
			((gamerules_gamerule_item_t*)(gamerule_item_pool[gamerule_item_pool_used].get()))->set_alternate(alternate);
			gamerule_item_pool[gamerule_item_pool_used]->impl_on_update(state);
			gamerule_item_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ gamerule_item_pool[0]->base_data.size.x, gamerule_item_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  gamerules_main_gamerule_grid_g_t::reset_pools() {
	gamerule_item_pool_used = 0;
}
ui::message_result gamerules_main_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	state.ui_state.drag_target = this;
	return ui::message_result::consumed;
}
ui::message_result gamerules_main_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void gamerules_main_t::on_hide(sys::state& state) noexcept {
// BEGIN main::on_hide
// END
}
void gamerules_main_t::on_update(sys::state& state) noexcept {
// BEGIN main::update
// END
	gamerule_grid_g.update(state, this);
	remake_layout(state, true);
}
void gamerules_main_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
					temp.ptr = make_gamerules_main(state);
				}
				if(cname == "gamerule_item") {
					temp.ptr = make_gamerules_gamerule_item(state);
				}
				if(cname == "gamerule_opt") {
					temp.ptr = make_gamerules_gamerule_opt(state);
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
				if(cname == "gamerule_grid_g") {
					temp.generator = &gamerule_grid_g;
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
void gamerules_main_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("gamerules::main"));
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
	gamerule_grid_g.on_create(state, this);
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN main::create
// END
}
std::unique_ptr<ui::element_base> make_gamerules_main(sys::state& state) {
	auto ptr = std::make_unique<gamerules_main_t>();
	ptr->on_create(state);
	return ptr;
}
void gamerules_gamerule_item_desc_text_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	gamerules_gamerule_item_t& gamerule_item = *((gamerules_gamerule_item_t*)(parent)); 
	gamerules_main_t& main = *((gamerules_main_t*)(parent->parent)); 
// BEGIN gamerule_item::desc_text::tooltip
	auto layout_box = text::open_layout_box(contents);
	auto desc_text_key = state.world.gamerule_get_tooltip_explain(gamerule_item.value);
	text::add_to_layout_box(state, contents, layout_box, desc_text_key);
	text::close_layout_box(contents, layout_box);

// END
}
void gamerules_gamerule_item_desc_text_t::on_update(sys::state& state) noexcept {
	gamerules_gamerule_item_t& gamerule_item = *((gamerules_gamerule_item_t*)(parent)); 
	gamerules_main_t& main = *((gamerules_main_t*)(parent->parent)); 
// BEGIN gamerule_item::desc_text::update
	auto desc_text_key = state.world.gamerule_get_name(gamerule_item.value);
	set_text(state, text::produce_simple_string(state, desc_text_key));
// END
}
void gamerules_gamerule_item_current_setting_t::add_item( dcon::gamerule_id rule,  uint8_t option) {
	list_contents.emplace_back(gamerule_opt_option{rule, option});
	++total_items;
}
ui::element_base* gamerules_gamerule_item_current_setting_t::get_nth_item(sys::state& state, int32_t id, int32_t pool_id) {
	while(pool_id >= int32_t(list_pool.size())) {
		list_pool.emplace_back(static_cast<gamerules_gamerule_opt_t*>(make_gamerules_gamerule_opt(state).release()));
	}
	list_pool[pool_id]->rule = list_contents[id].rule; 
	list_pool[pool_id]->option = list_contents[id].option; 
	return list_pool[pool_id].get();
}
void gamerules_gamerule_item_current_setting_t::quiet_on_selection(sys::state& state, int32_t id) {
	if(id < 0 || id >= int32_t(list_contents.size())) return;
	selected_item = id;
	label_window_internal->rule = list_contents[id].rule; 
	label_window_internal->option = list_contents[id].option; 
	label_window_internal->impl_on_update(state); 
}
void gamerules_gamerule_item_current_setting_t::on_selection(sys::state& state, int32_t id) {
	quiet_on_selection(state, id);
	gamerules_gamerule_item_t& gamerule_item = *((gamerules_gamerule_item_t*)(parent)); 
	gamerules_main_t& main = *((gamerules_main_t*)(parent->parent)); 
// BEGIN gamerule_item::current_setting::on_selection
	command::change_gamerule_setting(state, state.local_player_nation, gamerule_item.value, list_contents[id].option);
// END
}
void gamerules_gamerule_item_current_setting_t::clear_list() {	list_contents.clear();
	total_items = 0;
}
void gamerules_gamerule_item_current_setting_t::on_update(sys::state& state) noexcept {
	gamerules_gamerule_item_t& gamerule_item = *((gamerules_gamerule_item_t*)(parent)); 
	gamerules_main_t& main = *((gamerules_main_t*)(parent->parent)); 
// BEGIN gamerule_item::current_setting::update
	clear_list();
	auto setting_count = state.world.gamerule_get_settings_count(gamerule_item.value);
	for(int32_t i = 0; i < int32_t(setting_count); ++i) {
		add_item(gamerule_item.value, uint8_t(i));
	}
	quiet_on_selection(state, int32_t(state.world.gamerule_get_current_setting(gamerule_item.value)));
	disabled = (state.network_mode == sys::network_mode_type::client);
// END
}
void gamerules_gamerule_item_current_setting_t::on_create(sys::state& state) noexcept {
	template_drop_down_control::on_create(state);
// BEGIN gamerule_item::current_setting::create
// END
}
void  gamerules_gamerule_item_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 3 : 4;
}
ui::message_result gamerules_gamerule_item_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result gamerules_gamerule_item_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void gamerules_gamerule_item_t::on_update(sys::state& state) noexcept {
	gamerules_main_t& main = *((gamerules_main_t*)(parent->parent)); 
// BEGIN gamerule_item::update
// END
	remake_layout(state, true);
}
void gamerules_gamerule_item_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "desc_text") {
					temp.ptr = desc_text.get();
				} else
				if(cname == "current_setting") {
					temp.ptr = current_setting.get();
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
					temp.ptr = make_gamerules_main(state);
				}
				if(cname == "gamerule_item") {
					temp.ptr = make_gamerules_gamerule_item(state);
				}
				if(cname == "gamerule_opt") {
					temp.ptr = make_gamerules_gamerule_opt(state);
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
void gamerules_gamerule_item_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("gamerules::gamerule_item"));
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
		if(child_data.name == "desc_text") {
			desc_text = std::make_unique<gamerules_gamerule_item_desc_text_t>();
			desc_text->parent = this;
			auto cptr = desc_text.get();
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
		if(child_data.name == "current_setting") {
			current_setting = std::make_unique<gamerules_gamerule_item_current_setting_t>();
			current_setting->parent = this;
			auto cptr = current_setting.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->two_columns = (child_data.text_type != aui_text_type::body);
			cptr->target_page_height = child_data.border_size;
			cptr->label_window_internal = std::unique_ptr<gamerules_gamerule_opt_t>(static_cast<gamerules_gamerule_opt_t*>(make_gamerules_gamerule_opt(state).release()));
			cptr->element_x_size = cptr->label_window_internal->base_data.size.x;
			cptr->element_y_size = cptr->label_window_internal->base_data.size.y;
			cptr->label_window = cptr->label_window_internal.get();
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
// BEGIN gamerule_item::create
// END
}
std::unique_ptr<ui::element_base> make_gamerules_gamerule_item(sys::state& state) {
	auto ptr = std::make_unique<gamerules_gamerule_item_t>();
	ptr->on_create(state);
	return ptr;
}
void gamerules_gamerule_opt_option_name_t::on_update(sys::state& state) noexcept {
	gamerules_gamerule_opt_t& gamerule_opt = *((gamerules_gamerule_opt_t*)(parent)); 
// BEGIN gamerule_opt::option_name::update
	auto desc_text_key = state.world.gamerule_get_options(gamerule_opt.rule)[gamerule_opt.option].name;
	set_text(state, text::produce_simple_string(state, desc_text_key));
// END
}
ui::message_result gamerules_gamerule_opt_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result gamerules_gamerule_opt_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void gamerules_gamerule_opt_t::on_update(sys::state& state) noexcept {
// BEGIN gamerule_opt::update
// END
	remake_layout(state, true);
}
void gamerules_gamerule_opt_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "option_name") {
					temp.ptr = option_name.get();
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
					temp.ptr = make_gamerules_main(state);
				}
				if(cname == "gamerule_item") {
					temp.ptr = make_gamerules_gamerule_item(state);
				}
				if(cname == "gamerule_opt") {
					temp.ptr = make_gamerules_gamerule_opt(state);
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
void gamerules_gamerule_opt_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("gamerules::gamerule_opt"));
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
		if(child_data.name == "option_name") {
			option_name = std::make_unique<gamerules_gamerule_opt_option_name_t>();
			option_name->parent = this;
			auto cptr = option_name.get();
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
// BEGIN gamerule_opt::create
// END
}
std::unique_ptr<ui::element_base> make_gamerules_gamerule_opt(sys::state& state) {
	auto ptr = std::make_unique<gamerules_gamerule_opt_t>();
	ptr->on_create(state);
	return ptr;
}
// LOST-CODE
}
