#pragma once
#include "gui_element_base.hpp"
#include "system_state.hpp"
#include "economy_government.hpp"
#include "price.hpp"
#include "construction.hpp"
#include "graphics/color.hpp"
#include "province_templates.hpp"
#include "rebels.hpp"
#include "gui_population_window.hpp"
#include "national_budget.hpp"
#include "container_types_ui.hpp"

namespace alice_ui {

bool state_is_rtl(sys::state& state);

struct measure_result {
	int32_t x_space;
	int32_t y_space;
	enum class special {
		none, space_consumer, end_line, end_page, no_break
	} other;
};

enum class layout_type : uint8_t {
	single_horizontal,
	single_vertical,
	overlapped_horizontal,
	overlapped_vertical,
	mulitline_horizontal,
	multiline_vertical
};
enum class layout_line_alignment : uint8_t {
	leading, trailing, centered
};
enum class glue_type : uint8_t {
	standard,
	at_least,
	line_break,
	page_break,
	glue_don_t_break
};

struct layout_control {
	ui::element_base* ptr;
	int16_t abs_x = 0;
	int16_t abs_y = 0;
	bool absolute_position = false;
	bool fill_x = false;
	bool fill_y = false;
};
struct layout_window {
	std::unique_ptr<ui::element_base> ptr;
	int16_t abs_x = 0;
	int16_t abs_y = 0;
	bool absolute_position = false;
	bool fill_x = false;
	bool fill_y = false;
};
struct layout_glue {
	glue_type type = glue_type::standard;
	int16_t amount = 0;
};

class layout_generator {
public:
	virtual measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) = 0;
	virtual size_t item_count() = 0;
	virtual void reset_pools() = 0;
	virtual ~layout_generator() { }
};

struct generator_instance {
	layout_generator* generator;
};

struct layout_level;
struct sub_layout {
	std::unique_ptr<layout_level> layout;

	sub_layout() noexcept = default;
	sub_layout(sub_layout const& o) noexcept {
		std::abort();
	}
	sub_layout(sub_layout&& o) noexcept = default;
	sub_layout& operator=(sub_layout&& o) noexcept = default;
	~sub_layout() = default;
};

enum class layout_item_types : uint8_t {
	control, window, glue, generator, layout, texture_layer, control2, window2, generator2
};

enum class background_type : uint8_t {
	none,
	texture,
	bordered_texture,
	existing_gfx,
	linechart,
	stackedbarchart,
	colorsquare,
	flag,
	table_columns,
	table_headers,
	progress_bar,
	icon_strip,
	doughnut,
	border_texture_repeat,
	textured_corners
};

struct texture_layer {
	std::string texture;
	dcon::texture_id texture_id;
	background_type texture_type = background_type::texture;
};

using layout_item = std::variant<std::monostate, layout_control, layout_window, layout_glue, generator_instance, sub_layout, texture_layer>;

class page_buttons;
class layout_window_element;

enum class animation_type : uint8_t {
	none,
	page_left,
	page_right,
	page_up,
	page_middle,
};

struct page_info {
	uint16_t last_index = 0;
};
struct layout_level {
	std::vector<layout_item> contents;
	std::unique_ptr<page_buttons> page_controls;
	std::vector<page_info> page_starts;
	int16_t current_page = 0;
	int16_t size_x = -1;
	int16_t size_y = -1;
	int16_t margin_top = 0;
	int16_t margin_bottom = -1;
	int16_t margin_left = -1;
	int16_t margin_right = -1;
	int16_t resolved_x_pos = 0;
	int16_t resolved_y_pos = 0;
	int16_t resolved_x_size = 0;
	int16_t resolved_y_size = 0;
	int16_t template_id = -1;
	layout_line_alignment line_alignment = layout_line_alignment::leading;
	layout_line_alignment line_internal_alignment = layout_line_alignment::leading;
	layout_type type = layout_type::single_horizontal;
	animation_type page_animation;
	uint8_t interline_spacing = 0;
	bool paged = false;

	void change_page(sys::state& state, layout_window_element& container, int32_t new_page);
};

struct layout_window;

class page_buttons : public ui::element_base {
public:
	text::layout text_layout;

	layout_level* for_layout = nullptr;
	int16_t last_page = -1;
	int16_t last_size = -1;

	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	ui::tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return ui::tooltip_behavior::no_tooltip;
	}
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		if(type == ui::mouse_probe_type::click && last_size > 1) {
			return ui::message_result::consumed;
		} else {
			return ui::message_result::unseen;
		}
	}

	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		return ui::message_result::consumed;
	}
	void on_update(sys::state& state) noexcept override;
};

struct layout_iterator;

struct positioned_texture {
	int16_t x; int16_t y; int16_t w; int16_t h;
	std::string_view texture;
	dcon::texture_id texture_id;
	background_type texture_type = background_type::texture;
};

inline constexpr int32_t mouse_over_animation_ms = 150;

class template_icon_button : public ui::element_base {
public:
	int32_t template_id = -1;
	int32_t icon = -1;
	dcon::text_key default_tooltip;
	std::chrono::steady_clock::time_point last_activated;
	bool disabled = false;

	void on_create(sys::state& state) noexcept override { }
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	ui::tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return default_tooltip ? ui::tooltip_behavior::tooltip : ui::tooltip_behavior::no_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		if(type == ui::mouse_probe_type::scroll)
			return ui::message_result::unseen;
		return ui::message_result::consumed;
	}
	virtual bool button_action(sys::state& state) noexcept {
		return false;
	}
	virtual bool button_right_action(sys::state& state) noexcept {
		return false;
	}
	virtual bool button_shift_action(sys::state& state) noexcept {
		return button_action(state);
	}
	virtual bool button_shift_right_action(sys::state& state) noexcept {
		return button_right_action(state);
	}
	virtual bool button_ctrl_action(sys::state& state) noexcept {
		return button_action(state);
	}
	virtual bool button_ctrl_right_action(sys::state& state) noexcept {
		return button_right_action(state);
	}
	virtual bool button_ctrl_shift_action(sys::state& state) noexcept {
		return button_action(state);
	}
	virtual bool button_ctrl_shift_right_action(sys::state& state) noexcept {
		return button_right_action(state);
	}
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		if(!disabled) {
			bool result = false;
			if(mods == sys::key_modifiers::modifiers_shift)
				result = button_shift_action(state);
			else if(mods == sys::key_modifiers::modifiers_ctrl)
				result = button_ctrl_action(state);
			else
				result = button_action(state);
			if(result)
				sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
		}
		return ui::message_result::consumed;
	}
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		if(!disabled) {
			bool result = false;
			if(mods == sys::key_modifiers::modifiers_shift)
				result = button_shift_right_action(state);
			else if(mods == sys::key_modifiers::modifiers_ctrl)
				result = button_ctrl_right_action(state);
			else
				result = button_right_action(state);
			
		}
		return ui::message_result::consumed;
	}
	ui::message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept override {
		if(!disabled && base_data.get_element_type() == ui::element_type::button && base_data.data.button.shortcut == key) {
			bool result = false;
			if(mods == sys::key_modifiers::modifiers_shift)
				result = button_shift_action(state);
			else if(mods == sys::key_modifiers::modifiers_ctrl)
				result = button_ctrl_action(state);
			else
				result = button_action(state);

			if(result) {
				sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
				return ui::message_result::consumed;
			}
		}
		return ui::message_result::unseen;
	}
	void on_hover(sys::state& state) noexcept final;
	void on_hover_end(sys::state& state) noexcept final;
	virtual void button_on_hover(sys::state& state) noexcept { }
	virtual void button_on_hover_end(sys::state& state) noexcept { }
};


class template_icon_graphic : public ui::element_base {
public:
	int32_t template_id = -1;
	ogl::color3f color;
	dcon::text_key default_tooltip;

	void on_create(sys::state& state) noexcept override {
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	ui::tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return default_tooltip ? ui::tooltip_behavior::tooltip : ui::tooltip_behavior::no_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		if(has_tooltip(state) == ui::tooltip_behavior::no_tooltip)
			return ui::message_result::unseen;
		return type == ui::mouse_probe_type::tooltip ? ui::message_result::consumed : ui::message_result::unseen;
	}
};
class template_bg_graphic : public ui::element_base {
public:
	int32_t template_id = -1;
	dcon::text_key default_tooltip;

	void on_create(sys::state& state) noexcept override {
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	ui::tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return default_tooltip ? ui::tooltip_behavior::tooltip : ui::tooltip_behavior::no_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		if(has_tooltip(state) == ui::tooltip_behavior::no_tooltip)
			return ui::message_result::unseen;
		return type == ui::mouse_probe_type::tooltip ? ui::message_result::consumed : ui::message_result::unseen;
	}
};

class auto_close_button : public template_icon_button {
public:
	ui::message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept override;
	bool button_action(sys::state& state) noexcept override;
};

class template_label : public ui::element_base {
protected:
	std::string cached_text;
	text::layout internal_layout;
public:
	int32_t template_id = -1;
	dcon::text_key default_text;
	dcon::text_key default_tooltip;

	void set_text(sys::state& state, std::string_view new_text);
	void on_reset_text(sys::state& state) noexcept override;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;

	std::string_view get_text(sys::state& state) const {
		return cached_text;
	}

	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		if(has_tooltip(state) == ui::tooltip_behavior::no_tooltip)
			return ui::message_result::unseen;
		return type == ui::mouse_probe_type::tooltip ? ui::message_result::consumed : ui::message_result::unseen;
	}
	ui::tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return default_tooltip ? ui::tooltip_behavior::tooltip : ui::tooltip_behavior::no_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
};

class template_text_button : public ui::element_base {
protected:
	std::string cached_text;
	text::layout internal_layout;
public:
	int32_t template_id = -1;
	std::chrono::steady_clock::time_point last_activated;
	dcon::text_key default_text;
	dcon::text_key default_tooltip;
	bool disabled = false;

	void set_text(sys::state& state, std::string_view new_text);
	void on_reset_text(sys::state& state) noexcept override;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;

	std::string_view get_text(sys::state& state) const {
		return cached_text;
	}

	ui::tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return default_tooltip ? ui::tooltip_behavior::tooltip : ui::tooltip_behavior::no_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		if(type == ui::mouse_probe_type::scroll)
			return ui::message_result::unseen;
		return ui::message_result::consumed;
	}
	virtual bool button_action(sys::state& state) noexcept {
		return false;
	}
	virtual bool button_right_action(sys::state& state) noexcept {
		return false;
	}
	virtual bool button_shift_action(sys::state& state) noexcept {
		return button_action(state);
	}
	virtual bool button_shift_right_action(sys::state& state) noexcept {
		return button_right_action(state);
	}
	virtual bool button_ctrl_action(sys::state& state) noexcept {
		return button_action(state);
	}
	virtual bool button_ctrl_right_action(sys::state& state) noexcept {
		return button_right_action(state);
	}
	virtual bool button_ctrl_shift_action(sys::state& state) noexcept {
		return button_action(state);
	}
	virtual bool button_ctrl_shift_right_action(sys::state& state) noexcept {
		return button_right_action(state);
	}
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		if(!disabled) {
			bool result = false;
			if(mods == sys::key_modifiers::modifiers_shift)
				result = button_shift_action(state);
			else if(mods == sys::key_modifiers::modifiers_ctrl)
				result = button_ctrl_action(state);
			else
				result = button_action(state);
			if(result)
				sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
		}
		return ui::message_result::consumed;
	}
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		if(!disabled) {
			bool result = false;
			if(mods == sys::key_modifiers::modifiers_shift)
				result = button_shift_right_action(state);
			else if(mods == sys::key_modifiers::modifiers_ctrl)
				result = button_ctrl_right_action(state);
			else
				result = button_right_action(state);

		}
		return ui::message_result::consumed;
	}
	ui::message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept override {
		if(!disabled && base_data.get_element_type() == ui::element_type::button && base_data.data.button.shortcut == key) {
			bool result = false;
			if(mods == sys::key_modifiers::modifiers_shift)
				result = button_shift_action(state);
			else if(mods == sys::key_modifiers::modifiers_ctrl)
				result = button_ctrl_action(state);
			else
				result = button_action(state);

			if(result) {
				sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
				return ui::message_result::consumed;
			}
		}
		return ui::message_result::unseen;
	}
	void on_hover(sys::state& state) noexcept final;
	void on_hover_end(sys::state& state) noexcept final;
	virtual void button_on_hover(sys::state& state) noexcept { }
	virtual void button_on_hover_end(sys::state& state) noexcept { }
};


class template_mixed_button : public ui::element_base {
protected:
	std::string cached_text;
	text::layout internal_layout;
public:
	int32_t template_id = -1;
	std::chrono::steady_clock::time_point last_activated;
	dcon::text_key default_text;
	dcon::text_key default_tooltip;
	int16_t icon_id = -1;
	bool disabled = false;

	void set_text(sys::state& state, std::string_view new_text);
	void on_reset_text(sys::state& state) noexcept override;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;

	std::string_view get_text(sys::state& state) const {
		return cached_text;
	}
	ui::tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return default_tooltip ? ui::tooltip_behavior::tooltip : ui::tooltip_behavior::no_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		if(type == ui::mouse_probe_type::scroll)
			return ui::message_result::unseen;
		return ui::message_result::consumed;
	}
	virtual bool button_action(sys::state& state) noexcept {
		return false;
	}
	virtual bool button_right_action(sys::state& state) noexcept {
		return false;
	}
	virtual bool button_shift_action(sys::state& state) noexcept {
		return button_action(state);
	}
	virtual bool button_shift_right_action(sys::state& state) noexcept {
		return button_right_action(state);
	}
	virtual bool button_ctrl_action(sys::state& state) noexcept {
		return button_action(state);
	}
	virtual bool button_ctrl_right_action(sys::state& state) noexcept {
		return button_right_action(state);
	}
	virtual bool button_ctrl_shift_action(sys::state& state) noexcept {
		return button_action(state);
	}
	virtual bool button_ctrl_shift_right_action(sys::state& state) noexcept {
		return button_right_action(state);
	}
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		if(!disabled) {
			bool result = false;
			if(mods == sys::key_modifiers::modifiers_shift)
				result = button_shift_action(state);
			else if(mods == sys::key_modifiers::modifiers_ctrl)
				result = button_ctrl_action(state);
			else
				result = button_action(state);
			if(result)
				sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
		}
		return ui::message_result::consumed;
	}
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		if(!disabled) {
			bool result = false;
			if(mods == sys::key_modifiers::modifiers_shift)
				result = button_shift_right_action(state);
			else if(mods == sys::key_modifiers::modifiers_ctrl)
				result = button_ctrl_right_action(state);
			else
				result = button_right_action(state);

		}
		return ui::message_result::consumed;
	}
	ui::message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept override {
		if(!disabled && base_data.get_element_type() == ui::element_type::button && base_data.data.button.shortcut == key) {
			bool result = false;
			if(mods == sys::key_modifiers::modifiers_shift)
				result = button_shift_action(state);
			else if(mods == sys::key_modifiers::modifiers_ctrl)
				result = button_ctrl_action(state);
			else
				result = button_action(state);

			if(result) {
				sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
				return ui::message_result::consumed;
			}
		}
		return ui::message_result::unseen;
	}
	void on_hover(sys::state& state) noexcept final;
	void on_hover_end(sys::state& state) noexcept final;
	virtual void button_on_hover(sys::state& state) noexcept {
	}
	virtual void button_on_hover_end(sys::state& state) noexcept {
	}
};


class template_mixed_button_ci : public template_mixed_button {
public:
	ogl::color3f icon_color;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
};

class template_toggle_button : public ui::element_base {
protected:
	std::string cached_text;
	text::layout internal_layout;
private:
	bool is_active = false;
public:
	int32_t template_id = -1;
	std::chrono::steady_clock::time_point last_activated;
	dcon::text_key default_text;
	dcon::text_key default_tooltip;
	bool disabled = false;
	
	void set_text(sys::state& state, std::string_view new_text);
	void set_active(sys::state& state, bool active);
	void on_reset_text(sys::state& state) noexcept override;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;

	std::string_view get_text(sys::state& state) const {
		return cached_text;
	}

	ui::tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return default_tooltip ? ui::tooltip_behavior::tooltip : ui::tooltip_behavior::no_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		if(type == ui::mouse_probe_type::scroll)
			return ui::message_result::unseen;
		return ui::message_result::consumed;
	}
	virtual bool button_action(sys::state& state) noexcept {
		return false;
	}
	virtual bool button_right_action(sys::state& state) noexcept {
		return false;
	}
	virtual bool button_shift_action(sys::state& state) noexcept {
		return button_action(state);
	}
	virtual bool button_shift_right_action(sys::state& state) noexcept {
		return button_right_action(state);
	}
	virtual bool button_ctrl_action(sys::state& state) noexcept {
		return button_action(state);
	}
	virtual bool button_ctrl_right_action(sys::state& state) noexcept {
		return button_right_action(state);
	}
	virtual bool button_ctrl_shift_action(sys::state& state) noexcept {
		return button_action(state);
	}
	virtual bool button_ctrl_shift_right_action(sys::state& state) noexcept {
		return button_right_action(state);
	}
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		if(!disabled) {
			bool result = false;
			if(mods == sys::key_modifiers::modifiers_shift)
				result = button_shift_action(state);
			else if(mods == sys::key_modifiers::modifiers_ctrl)
				result = button_ctrl_action(state);
			else
				result = button_action(state);
			if(result)
				sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
		}
		return ui::message_result::consumed;
	}
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		if(!disabled) {
			bool result = false;
			if(mods == sys::key_modifiers::modifiers_shift)
				result = button_shift_right_action(state);
			else if(mods == sys::key_modifiers::modifiers_ctrl)
				result = button_ctrl_right_action(state);
			else
				result = button_right_action(state);

		}
		return ui::message_result::consumed;
	}
	ui::message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept override {
		if(!disabled && base_data.get_element_type() == ui::element_type::button && base_data.data.button.shortcut == key) {
			bool result = false;
			if(mods == sys::key_modifiers::modifiers_shift)
				result = button_shift_action(state);
			else if(mods == sys::key_modifiers::modifiers_ctrl)
				result = button_ctrl_action(state);
			else
				result = button_action(state);

			if(result) {
				sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
				return ui::message_result::consumed;
			}
		}
		return ui::message_result::unseen;
	}
	void on_hover(sys::state& state) noexcept final;
	void on_hover_end(sys::state& state) noexcept final;
	virtual void button_on_hover(sys::state& state) noexcept {
	}
	virtual void button_on_hover_end(sys::state& state) noexcept {
	}
};


class grid_size_window : public ui::non_owning_container_base {
public:
	int32_t grid_size = 8;
};

class pop_up_menu_container : public grid_size_window {
public:
	ui::element_base* scroll_redirect = nullptr;
	int32_t bg_template = -1;
	

	ui::message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override {
		if(scroll_redirect)
			scroll_redirect->impl_on_scroll(state, x, y, amount, mods);
		return ui::message_result::consumed;
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		return ui::message_result::consumed;
	}
	ui::message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept override {
		if(key == sys::virtual_key::ESCAPE) {
			set_visible(state, false);
			return ui::message_result::consumed;
		}
		return ui::message_result::unseen;
	}
	void on_hide(sys::state& state) noexcept override {
		scroll_redirect = nullptr;
	}
	friend struct layout_iterator;
};

class template_drop_down_control;

class drop_down_list_button : public template_mixed_button {
public:
	template_drop_down_control* owner_control = nullptr;
	int32_t list_id = 0;

	void on_update(sys::state& state) noexcept override;
	bool button_action(sys::state& state) noexcept override;
};

class drop_down_list_page_buttons : public ui::element_base {
public:
	text::layout text_layout;
	template_drop_down_control* owner_control = nullptr;

	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	ui::tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return ui::tooltip_behavior::no_tooltip;
	}
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		return ui::message_result::consumed;
	}
	void on_update(sys::state& state) noexcept override;
};

class template_drop_down_control : public ui::element_base {
public:
	std::unique_ptr<drop_down_list_page_buttons> page_controls;
	std::vector<std::unique_ptr<drop_down_list_button>> list_buttons_pool;
	ui::element_base* label_window = nullptr;

	int32_t template_id = -1;
	int32_t list_page = 0;
	int32_t items_per_page = 1;
	int32_t total_items = 0;
	int32_t selected_item = 0;

	int32_t target_page_height = -1;
	int32_t element_x_size = 1;
	int32_t element_y_size = 1;
	bool page_text_out_of_date = false;
	bool two_columns = false;

	std::chrono::steady_clock::time_point last_activated;
	bool disabled = false;

	template_drop_down_control() { }

	ui::mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept final {
		if(label_window->is_visible()) {
			auto relative_location = child_relative_location(state, *this, *label_window);
			auto res = label_window->impl_probe_mouse(state, x - relative_location.x, y - relative_location.y, type);
			if(res.under_mouse)
				return res;
		}
		return element_base::impl_probe_mouse(state, x, y, type);
	}
	ui::message_result impl_on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept final {
		return label_window->impl_on_key_down(state, key, mods);
	}
	void impl_on_update(sys::state& state) noexcept final {
		on_update(state);
		label_window->impl_on_update(state);
	}
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept final {
		render(state, x, y);
		if(label_window->is_visible()) {
			auto relative_location = child_relative_location(state, *this, *label_window);
			label_window->impl_render(state, x + relative_location.x, y + relative_location.y);
		}
	}
	void impl_on_reset_text(sys::state& state) noexcept final {
		label_window->impl_on_reset_text(state);
	}

	void on_create(sys::state& state) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		if(type == ui::mouse_probe_type::scroll)
			return ui::message_result::unseen;
		return ui::message_result::consumed;
	}
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		return ui::message_result::consumed;
	}
	void on_hover(sys::state& state) noexcept final;
	void on_hover_end(sys::state& state) noexcept final;

	void open_list(sys::state& state);
	void hide_list(sys::state& state);
	void change_page(sys::state& state, int32_t to_page);

	virtual ui::element_base* get_nth_item(sys::state& state, int32_t id, int32_t pool_id) = 0;
	virtual void on_selection(sys::state& state, int32_t id) = 0; // must change the value stored in the display item and update selected_item

	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		if(!disabled) {
			open_list(state);
			sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
		}
		return ui::message_result::consumed;
	}
	ui::message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override {
		if(total_items <= items_per_page)
			return ui::message_result::consumed;

		change_page(state, std::clamp(list_page + ((amount < 0) ? 1 : -1), 0, (total_items + items_per_page - 1) / items_per_page - 1));
		return ui::message_result::consumed;
	}
};

class legacy_commodity_icon : public ui::element_base {
public:
	dcon::commodity_id content;
	bool show_tooltip = true;
	void on_create(sys::state& state) noexcept override {
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	ui::tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return (show_tooltip && bool(content)) ? ui::tooltip_behavior::variable_tooltip : ui::tooltip_behavior::no_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		if(has_tooltip(state) == ui::tooltip_behavior::no_tooltip)
			return ui::message_result::unseen;
		return type == ui::mouse_probe_type::tooltip ? ui::message_result::consumed : ui::message_result::unseen;
	}
};

class layout_window_element : public grid_size_window {
private:
	void remake_layout_internal(layout_level& lvl, sys::state& state, int32_t x, int32_t y, int32_t w, int32_t h, bool remake_lists);
	void render_layout_internal(layout_level& lvl, sys::state& state, int32_t x, int32_t y);
	void clear_pages_internal(layout_level& lvl);
public:
	layout_level layout;
	std::unique_ptr<auto_close_button> auto_close;

	std::string_view page_left_texture_key;
	std::string_view page_right_texture_key;
	dcon::texture_id page_left_texture_id;
	dcon::texture_id page_right_texture_id;
	text::text_color page_text_color = text::text_color::black;
	int32_t window_template = -1;
	
	std::vector<positioned_texture> textures_to_render{};

	void remake_layout(sys::state& state, bool remake_lists) {
		children.clear();
		textures_to_render.clear();
		if(remake_lists)
			clear_pages_internal(layout);
		remake_layout_internal(layout, state, 0, 0, base_data.size.x, base_data.size.y, remake_lists);
		std::reverse(children.begin(), children.end());
		if(auto_close)
			children.push_back(auto_close.get());
	}
	ui::message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override;
	void impl_on_update(sys::state& state) noexcept override;
	void initialize_template(sys::state& state, int32_t id, int32_t grid_size, bool auto_close);
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		if(window_template != -1 && state.ui_templates.window_t[window_template].bg != -1)
			return (type == ui::mouse_probe_type::scroll ? ui::message_result::unseen : ui::message_result::consumed);
		else
			return ui::message_result::unseen;
	}

	friend struct layout_iterator;
};

enum class display_closure_command { default_function, return_pointer };

template<std::unique_ptr<ui::element_base>(*GEN_FN)(sys::state&) >
ui::element_base* display_at_front(sys::state& state, display_closure_command fn = display_closure_command::default_function) {
	static ui::element_base* saved_ptr = [&]() {
		auto current_root = state.current_scene.get_root(state);
		auto new_item = GEN_FN(state);
		auto ptr = new_item.get();
		current_root->add_child_to_back(std::move(new_item));
		ptr->impl_on_update(state);
		return ptr;
	}();

	if(fn == display_closure_command::default_function) {
		auto current_root = state.current_scene.get_root(state);
		if(saved_ptr->parent != current_root) {
			auto take_child = saved_ptr->parent->remove_child(saved_ptr);
			current_root->add_child_to_front(std::move(take_child));
		} else {
			current_root->move_child_to_front(saved_ptr);
		}
		saved_ptr->set_visible(state, true);
		return nullptr;
	}
	if(fn == display_closure_command::return_pointer) {
		return saved_ptr;
	}
	return nullptr;
}

namespace budget_categories {
inline constexpr int32_t diplomatic_income = 0;
inline constexpr int32_t poor_tax = 1;
inline constexpr int32_t middle_tax = 2;
inline constexpr int32_t rich_tax = 3;
inline constexpr int32_t tariffs_import = 4;
inline constexpr int32_t tariffs_export = 5;
inline constexpr int32_t gold = 6;
inline constexpr int32_t diplomatic_expenses = 7;
inline constexpr int32_t social = 8;
inline constexpr int32_t military = 9;
inline constexpr int32_t education = 10;
inline constexpr int32_t admin = 11;
inline constexpr int32_t domestic_investment = 12;
inline constexpr int32_t overseas_spending = 13;
inline constexpr int32_t subsidies = 14;
inline constexpr int32_t construction = 15;
inline constexpr int32_t army_upkeep = 16;
inline constexpr int32_t navy_upkeep = 17;
inline constexpr int32_t debt_payment = 18;
inline constexpr int32_t stockpile = 19;
inline constexpr int32_t category_count = 20;
inline static bool expanded[category_count] = { false };
}

namespace popwindow {
inline static ankerl::unordered_dense::set<int32_t> open_states;
inline static ankerl::unordered_dense::set<int32_t> open_provs;
inline static ankerl::unordered_dense::set<int32_t> excluded_types;
inline static ankerl::unordered_dense::set<int32_t> excluded_cultures;
inline static ankerl::unordered_dense::set<int32_t> excluded_states;
inline static ankerl::unordered_dense::set<int32_t> excluded_religions;
inline static bool show_non_colonial = true;
inline static bool show_colonial = true;
inline static bool show_only_pops = false;
inline static bool sort_pops = true;
inline static bool sort_states = true;
}

bool pop_passes_filter(sys::state& state, dcon::pop_id p);

std::unique_ptr<ui::element_base> make_gamerules_main(sys::state& state);
std::unique_ptr<ui::element_base> make_macrobuilder2_main(sys::state& state);
std::unique_ptr<ui::element_base> make_budgetwindow_main(sys::state& state);
std::unique_ptr<ui::element_base> make_demographicswindow_main(sys::state& state);
std::unique_ptr<ui::element_base> make_province_economy_overview_body(sys::state& state);
std::unique_ptr<ui::element_base> make_pop_details_main(sys::state& state);
std::unique_ptr<ui::element_base> make_pop_budget_details_main(sys::state& state);
std::unique_ptr<ui::element_base> make_market_trade_report_body(sys::state& state);
std::unique_ptr<ui::element_base> make_rgo_report_body(sys::state& state);
std::unique_ptr<ui::element_base> make_market_prices_report_body(sys::state& state);
std::unique_ptr<ui::element_base> make_trade_dashboard_main(sys::state& state);
std::unique_ptr<ui::element_base> make_main_menu_base(sys::state& state);

void pop_screen_sort_state_rows(sys::state& state, std::vector<dcon::state_instance_id>& state_instances, alice_ui::layout_window_element* parent);

inline int8_t cmp3(std::string_view a, std::string_view b) {
	return int8_t(std::clamp(a.compare(b), -1, 1));
}
template<typename T>
int8_t cmp3(T const& a, T const& b) {
	if(a == b) return int8_t(0);
	return (a < b) ? int8_t(-1) : int8_t(1);
}

std::string_view get_setting_text_key(int32_t type);
void describe_conversion(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_migration(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_colonial_migration(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_emigration(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_promotion(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_demotion(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_con(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_mil(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_lit(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_growth(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_assimilation(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);
void describe_money(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids);



}
