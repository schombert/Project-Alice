#pragma once

#include "dcon_generated.hpp"
#include "gui_graphics.hpp"
#include "gui_element_base.hpp"
#include "sound.hpp"
#include "text.hpp"
#include <variant>

namespace ui {

template<typename T>
std::unique_ptr<element_base> make_element_by_type(sys::state& state, std::string_view name) { // also bypasses global creation hooks
	auto it = state.ui_state.defs_by_name.find(name);
	if(it != state.ui_state.defs_by_name.end()) {
		auto res = std::make_unique<T>();
		std::memcpy(&(res->base_data), &(state.ui_defs.gui[it->second.defintion]), sizeof(ui::element_data));
		make_size_from_graphics(state, res->base_data);
		res->on_create(state);
		return res;
	}
	return std::unique_ptr<element_base>{};
}
template<typename T>
std::unique_ptr<T> make_element_by_type(sys::state& state, dcon::gui_def_id id) { // also bypasses global creation hooks
	auto res = std::make_unique<T>();
	std::memcpy(&(res->base_data), &(state.ui_defs.gui[id]), sizeof(ui::element_data));
	make_size_from_graphics(state, res->base_data);
	res->on_create(state);
	return res;
}

class container_base : public element_base {
public:
	std::vector<std::unique_ptr<element_base>> children;


	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y) noexcept final;
	message_result impl_on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept final;
	void impl_on_update(sys::state& state) noexcept final;
	message_result impl_set(sys::state& state, Cyto::Any& payload) noexcept final;
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override;

	std::unique_ptr<element_base> remove_child(element_base* child) noexcept final;
	void move_child_to_front(element_base* child) noexcept final;
	void move_child_to_back(element_base* child) noexcept final;
	void add_child_to_front(std::unique_ptr<element_base> child) noexcept final;
	void add_child_to_back(std::unique_ptr<element_base> child) noexcept final;
	element_base* get_child_by_name(sys::state const& state, std::string_view name) noexcept final;
	element_base* get_child_by_index(sys::state const& state, int32_t index) noexcept final;
};

class image_element_base : public element_base {
public:
	int32_t frame = 0; // if it is from an icon strip
	bool disabled = false;
	bool interactable = false;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
};

class opaque_element_base : public image_element_base {
public:
	message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
		return message_result::consumed;
	}
	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		return message_result::consumed;
	}
	message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		return message_result::consumed;
	}
	message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override {
		return message_result::consumed;
	}
	tooltip_behavior has_tooltip(sys::state& state, int32_t x, int32_t y) noexcept override {
		return tooltip_behavior::no_tooltip;
	}
};

class button_element_base : public opaque_element_base {
private:
	std::string stored_text;
	float text_offset = 0.0f;
	bool black_text = true;
	int32_t font_id = 1;
	int32_t font_size = 14;
public:
	button_element_base() {
		interactable = true;
	}

	void set_button_text(sys::state& state, std::string const& new_text);

	virtual void button_action(sys::state& state) noexcept { }
	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept final {
		if(!disabled) {
			sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
			button_action(state);
		}
		return message_result::consumed;
	}
	message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept final {
		if(!disabled && base_data.get_element_type() == element_type::button && base_data.data.button.shortcut == key) {
			sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
			button_action(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
	void on_create(sys::state& state) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
};

class simple_text_element_base : public element_base {
private:
	std::string stored_text;
	float text_offset = 0.0f;
	bool black_text = true;
	int32_t font_id = 1;
	int32_t font_size = 14;
public:
	void set_text(sys::state& state, std::string const& new_text);
	void on_create(sys::state& state) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;

	std::string_view get_text(sys::state& state) const {
		return stored_text;
	}
};

class edit_box_element_base : public simple_text_element_base {
	int32_t edit_index = 0;
public:
	virtual void edit_box_enter(sys::state& state, std::string_view s) noexcept { }
	virtual void edit_box_update(sys::state& state, std::string_view s) noexcept { }

	message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
		return message_result::consumed;
	}
	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept override;
	void on_text(sys::state& state, char ch) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
};

struct multiline_text_section {
	std::string_view stored_text;
	float x_offset = 0.f;
	float y_offset = 0.f;
	text::text_color color = text::text_color::black;
};

using text_substitution = std::variant<std::string_view, dcon::text_key, dcon::nation_id, dcon::province_id, dcon::state_definition_id>;

struct hyperlink {
	text_substitution link_type;
	int32_t x = 0;
	int32_t y = 0;
	int32_t width = 0;
	int32_t height = 0;
};

class multiline_text_element_base : public element_base {
private:
	std::vector<multiline_text_section> sections = {};
	std::vector<hyperlink> hyperlinks = {};
	std::vector<text_substitution> substitutions = {};
	int32_t font_id = 1;
	int32_t font_size = 14;
	float vertical_spacing = 0.f;
	float line_height = 0.f;
	int32_t line_count = 0;
	int32_t current_line = 0;
	int32_t visible_lines = 0;

	void generate_sections(sys::state& state) noexcept;
	void add_text_section(sys::state& state, std::string_view text, float& current_x, float& current_y,  text::text_color color) noexcept;
	std::string_view get_substitute(sys::state& state, text::variable_type var_type) noexcept;
public:
	void on_create(sys::state& state) noexcept override;
	void update_substitutions(sys::state& state, std::vector<text_substitution> subs);
	void update_text(sys::state& state, dcon::text_sequence_id seq_id);
	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
		return message_result::consumed;
	}
	int32_t get_scroll_width() {
		return line_count - visible_lines;
	}
	void set_scroll_pos(int32_t pos) {
		current_line = std::min(pos, get_scroll_width());
	}
};

class draggable_target : public opaque_element_base {
public:
	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
};

class window_element_base : public container_base {
public:
	virtual std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept {
		return nullptr;
	}
	void on_create(sys::state& state) noexcept override;
	void on_drag(sys::state& state, int32_t oldx, int32_t oldy, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
};

template<class TabT>
class generic_tabbed_window : public window_element_base {
public:
	TabT active_tab = TabT();
};

class generic_close_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent)
			parent->set_visible(state, false);
	}
};

class checkbox_button : public button_element_base {
public:
	virtual bool is_active(sys::state& state) noexcept {
		return false;
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		frame = int32_t(is_active(state));
		button_element_base::render(state, x, y);
	}
};

class flag_button : public button_element_base {
private:
	GLuint flag_texture_handle = 0;

public:
	dcon::nation_id get_current_nation(sys::state& state) noexcept;
	void on_update(sys::state& state) noexcept override;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
};

template<class TabT>
class generic_tab_button : public checkbox_button {
public:
	bool is_active(sys::state& state) noexcept final {
		return parent && static_cast<generic_tabbed_window<TabT>*>(parent)->active_tab == target;
	}

	void button_action(sys::state& state) noexcept final {
		if(parent) {
			Cyto::Any payload = target;
			parent->impl_get(state, payload);
		}
	}

	TabT target = TabT();
};

class scrollbar_left : public button_element_base {
public:
	int32_t step_size = 1;
	void button_action(sys::state& state) noexcept final;
};

class scrollbar_right : public button_element_base {
public:
	int32_t step_size = 1;
	void button_action(sys::state& state) noexcept final;
};

class scrollbar_track : public opaque_element_base {
public:
	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept final;
};

class scrollbar_slider : public opaque_element_base {
public:
	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept final;
	void on_drag(sys::state& state, int32_t oldx, int32_t oldy, int32_t x, int32_t y, sys::key_modifiers mods) noexcept final;
};


struct mutable_scrollbar_settings {
	int32_t lower_value = 0;
	int32_t upper_value = 100;
	int32_t lower_limit = 0;
	int32_t upper_limit = 0;
	bool using_limits = false;
};

struct scrollbar_settings  {
	int32_t lower_value = 0;
	int32_t upper_value = 100;
	int32_t lower_limit = 0;
	int32_t upper_limit = 0;
	
	int32_t buttons_size = 20;
	int32_t track_size = 180;
	int32_t scaling_factor = 0;

	bool using_limits = false;
	bool vertical = false;
};

struct value_change {
	int32_t new_value = 0;
	bool move_slider = false;
	bool is_relative = false;
};

class scrollbar : public container_base {
	image_element_base* left_limit = nullptr;
	image_element_base* right_limit = nullptr;
	int32_t stored_value = 0;
protected:
	scrollbar_left* left = nullptr;
	scrollbar_right* right = nullptr;
	scrollbar_track* track = nullptr;
	scrollbar_slider* slider = nullptr;
	scrollbar_settings settings;
public:
	virtual void on_value_change(sys::state& state, int32_t v) noexcept { }

	void update_raw_value(sys::state& state, int32_t v);
	void update_scaled_value(sys::state& state, float v);

	float scaled_value() const;

	void change_settings(sys::state& state, mutable_scrollbar_settings const& settings_s);

	void on_create(sys::state& state) noexcept final;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept final;
};

template<class RowWinT, class RowConT>
class standard_listbox_scrollbar : public scrollbar {
public:
	void scale_to_parent();
	void on_value_change(sys::state& state, int32_t v) noexcept override;
};

template<class RowConT>
class listbox_row_element_base : public window_element_base {
protected:
	RowConT content{};

public:
	virtual void update(sys::state& state) noexcept { }
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
	message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override;
};

template<class RowConT>
class listbox_row_button_base : public button_element_base {
protected:
	RowConT content{};

public:
	virtual void update(sys::state& state) noexcept { }
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
	message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override;
};

template<class RowWinT, class RowConT>
class listbox_element_base : public container_base {
private:
	standard_listbox_scrollbar<RowWinT, RowConT>* list_scrollbar = nullptr;

protected:
	std::vector<RowWinT*> row_windows{};

	virtual std::string_view get_row_element_name() {
		return std::string_view{};
	}
	virtual bool is_reversed() {
		return false;
	}

public:
	std::vector<RowConT> row_contents{};

	void update(sys::state& state);
	message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
		return message_result::consumed;
	}
};

}
