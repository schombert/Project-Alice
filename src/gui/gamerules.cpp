namespace alice_ui {
struct gamerules_main_cancel_button_t;
struct gamerules_main_header_text_t;
struct gamerules_main_apply_button_t;
struct gamerules_main_reset_defaults_t;
struct gamerules_main_t;
struct gamerules_gamerule_item_desc_text_t;
struct gamerules_gamerule_item_previous_setting_t;
struct gamerules_gamerule_item_next_setting_t;
struct gamerules_gamerule_item_current_setting_t;
struct gamerules_gamerule_item_t;
struct gamerules_spacer_t;
struct gamerules_main_cancel_button_t : public ui::element_base {
// BEGIN main::cancel_button::variables
// END
	std::string_view gfx_key;
	dcon::gfx_object_id background_gid;
	int32_t frame = 0;
	text::layout internal_layout;
	text::text_color text_color = text::text_color::black;
	float text_scale = 1.000000f; 
	bool text_is_header = false; 
	text::alignment text_alignment = text::alignment::center;
	std::string cached_text;
	dcon::text_key text_key;
	void set_text(sys::state & state, std::string const& new_text);
	void on_reset_text(sys::state & state) noexcept override;
	dcon::text_key tooltip_key;
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
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct gamerules_main_header_text_t : public ui::element_base {
// BEGIN main::header_text::variables
// END
	text::layout internal_layout;
	text::text_color text_color = text::text_color::black;
	float text_scale = 1.000000f; 
	bool text_is_header = true; 
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
struct gamerules_main_apply_button_t : public ui::element_base {
// BEGIN main::apply_button::variables
// END
	std::string_view gfx_key;
	dcon::gfx_object_id background_gid;
	int32_t frame = 0;
	bool disabled = false;
	text::layout internal_layout;
	text::text_color text_color = text::text_color::black;
	float text_scale = 1.000000f; 
	bool text_is_header = false; 
	text::alignment text_alignment = text::alignment::center;
	std::string cached_text;
	dcon::text_key text_key;
	void set_text(sys::state & state, std::string const& new_text);
	void on_reset_text(sys::state & state) noexcept override;
	dcon::text_key tooltip_key;
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
	ui::message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct gamerules_main_reset_defaults_t : public ui::element_base {
// BEGIN main::reset_defaults::variables
// END
	std::string_view gfx_key;
	dcon::gfx_object_id background_gid;
	int32_t frame = 0;
	bool disabled = false;
	text::layout internal_layout;
	text::text_color text_color = text::text_color::black;
	float text_scale = 1.000000f; 
	bool text_is_header = false; 
	text::alignment text_alignment = text::alignment::center;
	std::string cached_text;
	dcon::text_key text_key;
	void set_text(sys::state & state, std::string const& new_text);
	void on_reset_text(sys::state & state) noexcept override;
	dcon::text_key tooltip_key;
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
struct gamerules_main_gamerule_grid_g_t : public layout_generator {
// BEGIN main::gamerule_grid_g::variables
// END
	struct gamerule_item_option { dcon::gamerule_id value; };
	std::vector<std::unique_ptr<ui::element_base>> gamerule_item_pool;
	int32_t gamerule_item_pool_used = 0;
	void add_gamerule_item( dcon::gamerule_id value);
	struct spacer_option { };
	std::vector<std::unique_ptr<ui::element_base>> spacer_pool;
	int32_t spacer_pool_used = 0;
	void add_spacer();
	std::vector<std::variant<std::monostate, gamerule_item_option, spacer_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct gamerules_gamerule_item_desc_text_t : public ui::element_base {
// BEGIN gamerule_item::desc_text::variables
// END
	text::layout internal_layout;
	text::text_color text_color = text::text_color::black;
	float text_scale = 0.100000f; 
	bool text_is_header = false; 
	text::alignment text_alignment = text::alignment::center;
	std::string cached_text;
	void set_text(sys::state & state, std::string const& new_text);
	void on_reset_text(sys::state & state) noexcept override;
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
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct gamerules_gamerule_item_previous_setting_t : public ui::element_base {
// BEGIN gamerule_item::previous_setting::variables
// END
	std::string_view texture_key;
	dcon::texture_id background_texture;
	bool disabled = false;
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
struct gamerules_gamerule_item_next_setting_t : public ui::element_base {
// BEGIN gamerule_item::next_setting::variables
// END
	std::string_view texture_key;
	dcon::texture_id background_texture;
	bool disabled = false;
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
struct gamerules_gamerule_item_current_setting_t : public ui::element_base {
// BEGIN gamerule_item::current_setting::variables
// END
	text::layout internal_layout;
	text::text_color text_color = text::text_color::black;
	float text_scale = 0.600000f; 
	bool text_is_header = false; 
	text::alignment text_alignment = text::alignment::center;
	std::string cached_text;
	void set_text(sys::state & state, std::string const& new_text);
	void on_reset_text(sys::state & state) noexcept override;
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
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct gamerules_main_t : public layout_window_element {
// BEGIN main::variables
// END
	std::unique_ptr<gamerules_main_cancel_button_t> cancel_button;
	std::unique_ptr<gamerules_main_header_text_t> header_text;
	std::unique_ptr<gamerules_main_apply_button_t> apply_button;
	std::unique_ptr<gamerules_main_reset_defaults_t> reset_defaults;
	gamerules_main_gamerule_grid_g_t gamerule_grid_g;
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
std::unique_ptr<ui::element_base> make_gamerules_main(sys::state& state);
struct gamerules_gamerule_item_t : public layout_window_element {
// BEGIN gamerule_item::variables
// END
	dcon::gamerule_id value;
	std::unique_ptr<gamerules_gamerule_item_desc_text_t> desc_text;
	std::unique_ptr<gamerules_gamerule_item_previous_setting_t> previous_setting;
	std::unique_ptr<gamerules_gamerule_item_next_setting_t> next_setting;
	std::unique_ptr<gamerules_gamerule_item_current_setting_t> current_setting;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	std::string_view texture_key;
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
		if(name_parameter == "value") {
			return (void*)(&value);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_gamerules_gamerule_item(sys::state& state);
struct gamerules_spacer_t : public ui::non_owning_container_base {
// BEGIN spacer::variables
// END
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		return ui::message_result::unseen;
	}
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_gamerules_spacer(sys::state& state);
void gamerules_main_gamerule_grid_g_t::add_gamerule_item(dcon::gamerule_id value) {
	values.emplace_back(gamerule_item_option{value});
}
void gamerules_main_gamerule_grid_g_t::add_spacer() {
	values.emplace_back(spacer_option{});
}
void  gamerules_main_gamerule_grid_g_t::on_create(sys::state& state, layout_window_element* parent) {
	gamerules_main_t& main = *((gamerules_main_t*)(parent)); 
// BEGIN main::gamerule_grid_g::on_create
	for(auto gamerule : state.world.in_gamerule) {
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
			gamerule_item_pool[gamerule_item_pool_used]->impl_on_update(state);
			gamerule_item_pool_used++;
		}
		alternate = true;
		return measure_result{ gamerule_item_pool[0]->base_data.size.x, gamerule_item_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	if(std::holds_alternative<spacer_option>(values[index])) {
		if(spacer_pool.empty()) spacer_pool.emplace_back(make_gamerules_spacer(state));
		if(destination) {
			if(spacer_pool.size() <= size_t(spacer_pool_used)) spacer_pool.emplace_back(make_gamerules_spacer(state));
			spacer_pool[spacer_pool_used]->base_data.position.x = int16_t(x);
			spacer_pool[spacer_pool_used]->base_data.position.y = int16_t(y);
			spacer_pool[spacer_pool_used]->parent = destination;
			destination->children.push_back(spacer_pool[spacer_pool_used].get());
			spacer_pool[spacer_pool_used]->impl_on_update(state);
			spacer_pool_used++;
		}
		alternate = true;
		return measure_result{ spacer_pool[0]->base_data.size.x, spacer_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  gamerules_main_gamerule_grid_g_t::reset_pools() {
	gamerule_item_pool_used = 0;
	spacer_pool_used = 0;
}
ui::message_result gamerules_main_cancel_button_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	gamerules_main_t& main = *((gamerules_main_t*)(parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN main::cancel_button::lbutton_action
 	main.set_visible(state, false);
// END
	return ui::message_result::consumed;
}
ui::message_result gamerules_main_cancel_button_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result gamerules_main_cancel_button_t::on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept {
	if(key == sys::virtual_key::ESCAPE) {
		on_lbutton_down(state, 0, 0, mods);
		return ui::message_result::consumed;
	}
	return ui::message_result::unseen;
}
void gamerules_main_cancel_button_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	text::add_line(state, contents, tooltip_key);
}
void gamerules_main_cancel_button_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 2), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void gamerules_main_cancel_button_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 2), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void gamerules_main_cancel_button_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(background_gid) {
		auto& gfx_def = state.ui_defs.gfx[background_gid];
		if(gfx_def.primary_texture_handle) {
			if(gfx_def.get_object_type() == ui::object_type::bordered_rect) {
				ogl::render_bordered_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), gfx_def.type_dependent, float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()), base_data.get_rotation(), gfx_def.is_vertically_flipped(), state_is_rtl(state)); 
			} else if(gfx_def.number_of_frames > 1) {
				ogl::render_subsprite(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), frame, gfx_def.number_of_frames, float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()), base_data.get_rotation(), gfx_def.is_vertically_flipped(), state_is_rtl(state)); 
			} else {
				ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()), base_data.get_rotation(), gfx_def.is_vertically_flipped(), state_is_rtl(state)); 
			}
		}
	}
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 2);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, true); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void gamerules_main_cancel_button_t::on_update(sys::state& state) noexcept {
	gamerules_main_t& main = *((gamerules_main_t*)(parent)); 
// BEGIN main::cancel_button::update
// END
}
void gamerules_main_cancel_button_t::on_create(sys::state& state) noexcept {
	if(auto it = state.ui_state.gfx_by_name.find(state.lookup_key(gfx_key)); it != state.ui_state.gfx_by_name.end()) {
		background_gid = it->second;
	}
	on_reset_text(state);
// BEGIN main::cancel_button::create
// END
}
void gamerules_main_header_text_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 2), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void gamerules_main_header_text_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 2), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void gamerules_main_header_text_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 2);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void gamerules_main_header_text_t::on_update(sys::state& state) noexcept {
	gamerules_main_t& main = *((gamerules_main_t*)(parent)); 
// BEGIN main::header_text::update
// END
}
void gamerules_main_header_text_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN main::header_text::create
// END
}
ui::message_result gamerules_main_apply_button_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(disabled) return ui::message_result::consumed;
	gamerules_main_t& main = *((gamerules_main_t*)(parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN main::apply_button::lbutton_action
	for(const auto& item : main.gamerule_grid_g.values) {
		if(std::holds_alternative<gamerules_main_gamerule_grid_g_t::gamerule_item_option>(item)) {
			auto item_option = std::get< gamerules_main_gamerule_grid_g_t::gamerule_item_option>(item);
			auto ui_gamerule_setting = state.ui_state.gamerule_ui_settings.find(item_option.value)->second;
			auto current_gr_setting = state.world.gamerule_get_current_setting(item_option.value);
			if(current_gr_setting != ui_gamerule_setting && command::can_change_gamerule_setting(state, state.local_player_nation, item_option.value, ui_gamerule_setting)) {
				command::change_gamerule_setting(state, state.local_player_nation, item_option.value, ui_gamerule_setting);
			}
		}
	}

// END
	return ui::message_result::consumed;
}
ui::message_result gamerules_main_apply_button_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result gamerules_main_apply_button_t::on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept {
	if(key == sys::virtual_key::RETURN && !disabled) {
		on_lbutton_down(state, 0, 0, mods);
		return ui::message_result::consumed;
	}
	return ui::message_result::unseen;
}
void gamerules_main_apply_button_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	gamerules_main_t& main = *((gamerules_main_t*)(parent)); 
// BEGIN main::apply_button::tooltip
	if(state.network_mode == sys::network_mode_type::client) {
		text::add_line(state, contents, "alice_gamerules_nothost");
	}
	else {
		text::add_line(state, contents, "SM_APPLY");
	}
// END
}
void gamerules_main_apply_button_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 2), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void gamerules_main_apply_button_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 2), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void gamerules_main_apply_button_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 2);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, disabled, true); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void gamerules_main_apply_button_t::on_update(sys::state& state) noexcept {
	gamerules_main_t& main = *((gamerules_main_t*)(parent)); 
// BEGIN main::apply_button::update
	if(state.network_mode == sys::network_mode_type::client) {
		disabled = true;
		return;
	}
	for(const auto& item : main.gamerule_grid_g.values) {
		if(std::holds_alternative<gamerules_main_gamerule_grid_g_t::gamerule_item_option>(item)) {
			auto item_option = std::get< gamerules_main_gamerule_grid_g_t::gamerule_item_option>(item);
			auto ui_gamerule_setting = state.ui_state.gamerule_ui_settings.find(item_option.value)->second;
			auto current_gr_setting = state.world.gamerule_get_current_setting(item_option.value);
			if(current_gr_setting != ui_gamerule_setting && command::can_change_gamerule_setting(state, state.local_player_nation, item_option.value, ui_gamerule_setting)) {
				disabled = false;
				return;
			}
		}
	}
	disabled = true;
// END
}
void gamerules_main_apply_button_t::on_create(sys::state& state) noexcept {
	if(auto it = state.ui_state.gfx_by_name.find(state.lookup_key(gfx_key)); it != state.ui_state.gfx_by_name.end()) {
		background_gid = it->second;
	}
	on_reset_text(state);
// BEGIN main::apply_button::create
// END
}
ui::message_result gamerules_main_reset_defaults_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(disabled) return ui::message_result::consumed;
	gamerules_main_t& main = *((gamerules_main_t*)(parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN main::reset_defaults::lbutton_action
	for(const auto& item : main.gamerule_grid_g.values) {
		if(std::holds_alternative<gamerules_main_gamerule_grid_g_t::gamerule_item_option>(item)) {
			auto item_option = std::get< gamerules_main_gamerule_grid_g_t::gamerule_item_option>(item);
			auto default_setting = state.world.gamerule_get_default_setting(item_option.value);
			auto current_gr_setting = state.world.gamerule_get_current_setting(item_option.value);
			if(current_gr_setting != default_setting && command::can_change_gamerule_setting(state, state.local_player_nation, item_option.value, default_setting)) {
				command::change_gamerule_setting(state, state.local_player_nation, item_option.value, default_setting);
			}
		}
	}
// END
	return ui::message_result::consumed;
}
ui::message_result gamerules_main_reset_defaults_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void gamerules_main_reset_defaults_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	gamerules_main_t& main = *((gamerules_main_t*)(parent)); 
// BEGIN main::reset_defaults::tooltip
	if(state.network_mode == sys::network_mode_type::client) {
		text::add_line(state, contents, "alice_gamerules_nothost");
	} else {
		text::add_line(state, contents, "MENU_MESSAGES_RESET");
	}
// END
}
void gamerules_main_reset_defaults_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 2), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void gamerules_main_reset_defaults_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 2), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void gamerules_main_reset_defaults_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 2);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, disabled, true); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void gamerules_main_reset_defaults_t::on_update(sys::state& state) noexcept {
	gamerules_main_t& main = *((gamerules_main_t*)(parent)); 
// BEGIN main::reset_defaults::update
	if(state.network_mode == sys::network_mode_type::client) {
		disabled = true;
		return;
	}
	for(const auto& item : main.gamerule_grid_g.values) {
		if(std::holds_alternative<gamerules_main_gamerule_grid_g_t::gamerule_item_option>(item)) {
			auto item_option = std::get< gamerules_main_gamerule_grid_g_t::gamerule_item_option>(item);
			auto current_gr_setting = state.world.gamerule_get_current_setting(item_option.value);
			auto default_gr_setting = state.world.gamerule_get_default_setting(item_option.value);
			if(default_gr_setting != current_gr_setting && command::can_change_gamerule_setting(state, state.local_player_nation, item_option.value, default_gr_setting)) {
				disabled = false;
				return;
			}
		}
	}
	disabled = true;
// END
}
void gamerules_main_reset_defaults_t::on_create(sys::state& state) noexcept {
	if(auto it = state.ui_state.gfx_by_name.find(state.lookup_key(gfx_key)); it != state.ui_state.gfx_by_name.end()) {
		background_gid = it->second;
	}
	on_reset_text(state);
// BEGIN main::reset_defaults::create
// END
}
ui::message_result gamerules_main_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	state.ui_state.drag_target = this;
	return ui::message_result::consumed;
}
ui::message_result gamerules_main_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void gamerules_main_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	auto cmod = ui::get_color_modification(false, false,  false);
	for (auto& __item : textures_to_render) {
		if (__item.texture_type == background_type::texture)
			ogl::render_textured_rect(state, cmod, float(x + __item.x), float(y + __item.y), float(__item.w), float(__item.h), ogl::get_late_load_texture_handle(state, __item.texture_id, __item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (__item.texture_type == background_type::border_texture_repeat)
			ogl::render_rect_with_repeated_border(state, cmod, float(1), float(x + __item.x), float(y + __item.y), float(__item.w), float(__item.h), ogl::get_late_load_texture_handle(state, __item.texture_id, __item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (__item.texture_type == background_type::textured_corners)
			ogl::render_rect_with_repeated_corner(state, cmod, float(1), float(x + __item.x), float(y + __item.y), float(__item.w), float(__item.h), ogl::get_late_load_texture_handle(state, __item.texture_id, __item.texture), base_data.get_rotation(), false, state_is_rtl(state));
	}
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
		lvl.page_controls->base_data.size.x = int16_t(10);
		lvl.page_controls->base_data.size.y = int16_t(2);
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
				if(cname == "cancel_button") {
					temp.ptr = cancel_button.get();
				}
				if(cname == "header_text") {
					temp.ptr = header_text.get();
				}
				if(cname == "apply_button") {
					temp.ptr = apply_button.get();
				}
				if(cname == "reset_defaults") {
					temp.ptr = reset_defaults.get();
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
				if(cname == "spacer") {
					temp.ptr = make_gamerules_spacer(state);
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
	texture_key = win_data.texture;
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		if(child_data.name == "cancel_button") {
			cancel_button = std::make_unique<gamerules_main_cancel_button_t>();
			cancel_button->parent = this;
			auto cptr = cancel_button.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->gfx_key = child_data.texture;
			cptr->text_key = state.lookup_key(child_data.text_key);
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "header_text") {
			header_text = std::make_unique<gamerules_main_header_text_t>();
			header_text->parent = this;
			auto cptr = header_text.get();
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
		if(child_data.name == "apply_button") {
			apply_button = std::make_unique<gamerules_main_apply_button_t>();
			apply_button->parent = this;
			auto cptr = apply_button.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->gfx_key = child_data.texture;
			cptr->text_key = state.lookup_key(child_data.text_key);
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "reset_defaults") {
			reset_defaults = std::make_unique<gamerules_main_reset_defaults_t>();
			reset_defaults->parent = this;
			auto cptr = reset_defaults.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->gfx_key = child_data.texture;
			cptr->text_key = state.lookup_key(child_data.text_key);
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
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
void gamerules_gamerule_item_desc_text_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 2), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void gamerules_gamerule_item_desc_text_t::on_reset_text(sys::state& state) noexcept {
}
void gamerules_gamerule_item_desc_text_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 2);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void gamerules_gamerule_item_desc_text_t::on_update(sys::state& state) noexcept {
	gamerules_gamerule_item_t& gamerule_item = *((gamerules_gamerule_item_t*)(parent)); 
	gamerules_main_t& main = *((gamerules_main_t*)(parent->parent)); 
// BEGIN gamerule_item::desc_text::update
	auto desc_text_key = state.world.gamerule_get_name(gamerule_item.value);
	set_text(state, text::produce_simple_string(state, desc_text_key));
// END
}
void gamerules_gamerule_item_desc_text_t::on_create(sys::state& state) noexcept {
// BEGIN gamerule_item::desc_text::create
// END
}
ui::message_result gamerules_gamerule_item_previous_setting_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(disabled) return ui::message_result::consumed;
	gamerules_gamerule_item_t& gamerule_item = *((gamerules_gamerule_item_t*)(parent)); 
	gamerules_main_t& main = *((gamerules_main_t*)(parent->parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN gamerule_item::previous_setting::lbutton_action
	auto setting_count = state.world.gamerule_get_settings_count(gamerule_item.value);
	auto current_gamerule_setting = state.ui_state.gamerule_ui_settings.find(gamerule_item.value)->second;
	if(setting_count > 1) {
		if(current_gamerule_setting == 0) {
			state.ui_state.gamerule_ui_settings.insert_or_assign(gamerule_item.value, current_gamerule_setting = setting_count - 1);
		}
		else {
			state.ui_state.gamerule_ui_settings.insert_or_assign(gamerule_item.value, current_gamerule_setting = current_gamerule_setting - 1);
			current_gamerule_setting = current_gamerule_setting - 1;
		}
	}
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return ui::message_result::consumed;
}
ui::message_result gamerules_gamerule_item_previous_setting_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void gamerules_gamerule_item_previous_setting_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, disabled, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
}
void gamerules_gamerule_item_previous_setting_t::on_update(sys::state& state) noexcept {
	gamerules_gamerule_item_t& gamerule_item = *((gamerules_gamerule_item_t*)(parent)); 
	gamerules_main_t& main = *((gamerules_main_t*)(parent->parent)); 
// BEGIN gamerule_item::previous_setting::update
	if(!bool(gamerule_item.value)) {
		disabled = true;
	}
	else {
		disabled = false;
	}
// END
}
void gamerules_gamerule_item_previous_setting_t::on_create(sys::state& state) noexcept {
// BEGIN gamerule_item::previous_setting::create
// END
}
ui::message_result gamerules_gamerule_item_next_setting_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(disabled) return ui::message_result::consumed;
	gamerules_gamerule_item_t& gamerule_item = *((gamerules_gamerule_item_t*)(parent)); 
	gamerules_main_t& main = *((gamerules_main_t*)(parent->parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN gamerule_item::next_setting::lbutton_action
	auto setting_count = state.world.gamerule_get_settings_count(gamerule_item.value);
	auto& current_gamerule_setting = state.ui_state.gamerule_ui_settings.find(gamerule_item.value)->second;
	if(setting_count > 1) {
		if(current_gamerule_setting >= setting_count - 1) {
			state.ui_state.gamerule_ui_settings.insert_or_assign(gamerule_item.value, 0);
		}
		else {
			state.ui_state.gamerule_ui_settings.insert_or_assign(gamerule_item.value, current_gamerule_setting = current_gamerule_setting + 1);
		}
	}
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return ui::message_result::consumed;
}
ui::message_result gamerules_gamerule_item_next_setting_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void gamerules_gamerule_item_next_setting_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, disabled, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
}
void gamerules_gamerule_item_next_setting_t::on_update(sys::state& state) noexcept {
	gamerules_gamerule_item_t& gamerule_item = *((gamerules_gamerule_item_t*)(parent)); 
	gamerules_main_t& main = *((gamerules_main_t*)(parent->parent)); 
// BEGIN gamerule_item::next_setting::update
	if(!bool(gamerule_item.value)) {
		disabled = true;
	} else {
		disabled = false;
	}
// END
}
void gamerules_gamerule_item_next_setting_t::on_create(sys::state& state) noexcept {
// BEGIN gamerule_item::next_setting::create
// END
}
void gamerules_gamerule_item_current_setting_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	gamerules_gamerule_item_t& gamerule_item = *((gamerules_gamerule_item_t*)(parent)); 
	gamerules_main_t& main = *((gamerules_main_t*)(parent->parent)); 
// BEGIN gamerule_item::current_setting::tooltip
	// maybe add a tooltip for each option?
	auto layout_box = text::open_layout_box(contents);
	auto desc_text_key = state.world.gamerule_get_tooltip_explain(gamerule_item.value);
	text::add_to_layout_box(state, contents, layout_box, desc_text_key);
	text::close_layout_box(contents, layout_box);
// END
}
void gamerules_gamerule_item_current_setting_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 2), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void gamerules_gamerule_item_current_setting_t::on_reset_text(sys::state& state) noexcept {
}
void gamerules_gamerule_item_current_setting_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 2);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void gamerules_gamerule_item_current_setting_t::on_update(sys::state& state) noexcept {
	gamerules_gamerule_item_t& gamerule_item = *((gamerules_gamerule_item_t*)(parent)); 
	gamerules_main_t& main = *((gamerules_main_t*)(parent->parent)); 
// BEGIN gamerule_item::current_setting::update
	auto current_gamerule_setting = state.ui_state.gamerule_ui_settings.find(gamerule_item.value)->second;
	auto desc_text_key = state.world.gamerule_get_options(gamerule_item.value)[current_gamerule_setting].name;
	set_text(state, text::produce_simple_string(state, desc_text_key));
// END
}
void gamerules_gamerule_item_current_setting_t::on_create(sys::state& state) noexcept {
// BEGIN gamerule_item::current_setting::create
// END
}
ui::message_result gamerules_gamerule_item_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result gamerules_gamerule_item_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void gamerules_gamerule_item_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	auto cmod = ui::get_color_modification(false, false,  false);
	for (auto& __item : textures_to_render) {
		if (__item.texture_type == background_type::texture)
			ogl::render_textured_rect(state, cmod, float(x + __item.x), float(y + __item.y), float(__item.w), float(__item.h), ogl::get_late_load_texture_handle(state, __item.texture_id, __item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (__item.texture_type == background_type::border_texture_repeat)
			ogl::render_rect_with_repeated_border(state, cmod, float(1), float(x + __item.x), float(y + __item.y), float(__item.w), float(__item.h), ogl::get_late_load_texture_handle(state, __item.texture_id, __item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (__item.texture_type == background_type::textured_corners)
			ogl::render_rect_with_repeated_corner(state, cmod, float(1), float(x + __item.x), float(y + __item.y), float(__item.w), float(__item.h), ogl::get_late_load_texture_handle(state, __item.texture_id, __item.texture), base_data.get_rotation(), false, state_is_rtl(state));
	}
}
void gamerules_gamerule_item_t::on_update(sys::state& state) noexcept {
	gamerules_main_t& main = *((gamerules_main_t*)(parent->parent)); 
// BEGIN gamerule_item::update
	if(!value) {
		flags |= element_base::is_invisible_mask;
	}
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
		lvl.page_controls->base_data.size.x = int16_t(10);
		lvl.page_controls->base_data.size.y = int16_t(2);
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
				if(cname == "desc_text") {
					temp.ptr = desc_text.get();
				}
				if(cname == "previous_setting") {
					temp.ptr = previous_setting.get();
				}
				if(cname == "next_setting") {
					temp.ptr = next_setting.get();
				}
				if(cname == "current_setting") {
					temp.ptr = current_setting.get();
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
				if(cname == "spacer") {
					temp.ptr = make_gamerules_spacer(state);
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
	texture_key = win_data.texture;
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
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "previous_setting") {
			previous_setting = std::make_unique<gamerules_gamerule_item_previous_setting_t>();
			previous_setting->parent = this;
			auto cptr = previous_setting.get();
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
		if(child_data.name == "next_setting") {
			next_setting = std::make_unique<gamerules_gamerule_item_next_setting_t>();
			next_setting->parent = this;
			auto cptr = next_setting.get();
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
		if(child_data.name == "current_setting") {
			current_setting = std::make_unique<gamerules_gamerule_item_current_setting_t>();
			current_setting->parent = this;
			auto cptr = current_setting.get();
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
// BEGIN gamerule_item::create
// END
}
std::unique_ptr<ui::element_base> make_gamerules_gamerule_item(sys::state& state) {
	auto ptr = std::make_unique<gamerules_gamerule_item_t>();
	ptr->on_create(state);
	return ptr;
}
void gamerules_spacer_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
}
void gamerules_spacer_t::on_update(sys::state& state) noexcept {
	gamerules_main_t& main = *((gamerules_main_t*)(parent->parent)); 
// BEGIN spacer::update
// END
}
void gamerules_spacer_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("gamerules::spacer"));
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
		pending_children.pop_back();
	}
// BEGIN spacer::create
// END
}
std::unique_ptr<ui::element_base> make_gamerules_spacer(sys::state& state) {
	auto ptr = std::make_unique<gamerules_spacer_t>();
	ptr->on_create(state);
	return ptr;
}
// LOST-CODE
}
