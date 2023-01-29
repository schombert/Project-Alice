#pragma once

#include "gui_graphics.hpp"
#include "gui_element_base.hpp"

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


	element_base* impl_probe_mouse(sys::state& state, int32_t x, int32_t y) noexcept final;
	message_result impl_on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept final;
	message_result impl_on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept final;
	message_result impl_on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept final;
	message_result impl_on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept final;
	void impl_on_update(sys::state& state) noexcept final;
	void impl_on_resize(sys::state& state, int32_t x, int32_t y, sys::window_state win_state) noexcept final;
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
			// TODO: button click sound
			button_action(state);
		}
		return message_result::consumed;
	}
	message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept final {
		if(!disabled && base_data.get_element_type() == element_type::button && base_data.data.button.shortcut == key) {
			// TODO: button click sound
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
	void on_create(sys::state& state) noexcept final;
	void on_drag(sys::state& state, int32_t oldx, int32_t oldy, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
};

class generic_close_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent)
			parent->set_visible(state, false);
	}
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
	scrollbar_left* left = nullptr;
	scrollbar_right* right = nullptr;
	scrollbar_track* track = nullptr;
	scrollbar_slider* slider = nullptr;
	image_element_base* left_limit = nullptr;
	image_element_base* right_limit = nullptr;
	scrollbar_settings settings;
	int32_t stored_value = 0;
public:
	virtual void on_value_change(sys::state& state, int32_t v) noexcept { }

	void update_raw_value(int32_t v);
	void update_scaled_value(float v);

	float scaled_value() const;

	void change_settings(sys::state& state, mutable_scrollbar_settings const& settings_s);

	void on_create(sys::state& state) noexcept final;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept final;
};

}
