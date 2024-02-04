#pragma once

#include "dcon_generated.hpp"
#include "gui_graphics.hpp"
#include "gui_element_base.hpp"
#include "opengl_wrapper.hpp"
#include "sound.hpp"
#include "system_state.hpp"
#include "text.hpp"
#include "texture.hpp"
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <variant>
#include <vector>

namespace ui {

template<typename T, typename ...Params>
std::unique_ptr<element_base> make_element_by_type(sys::state& state, std::string_view name, Params&&... params) { // also bypasses global creation hooks
	auto it = state.ui_state.defs_by_name.find(name);
	if(it != state.ui_state.defs_by_name.end()) {
		auto res = std::make_unique<T>(std::forward<Params>(params)...);
		std::memcpy(&(res->base_data), &(state.ui_defs.gui[it->second.definition]), sizeof(ui::element_data));
		make_size_from_graphics(state, res->base_data);
		res->on_create(state);
		return res;
	}
	return std::unique_ptr<element_base>{};
}
template<typename T, typename ...Params>
std::unique_ptr<T> make_element_by_type(sys::state& state, dcon::gui_def_id id, Params&&... params) { // also bypasses global creation hooks
	auto res = std::make_unique<T>(std::forward<Params>(params)...);
	std::memcpy(&(res->base_data), &(state.ui_defs.gui[id]), sizeof(ui::element_data));
	make_size_from_graphics(state, res->base_data);
	res->on_create(state);
	return res;
}

ogl::color_modification get_color_modification(bool is_under_mouse, bool is_disabled, bool is_interactable);
ogl::color3f get_text_color(text::text_color text_color);

class container_base : public element_base {
public:
	std::vector<std::unique_ptr<element_base>> children;

	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override;
	message_result impl_on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept final;
	void impl_on_update(sys::state& state) noexcept override;
	message_result impl_set(sys::state& state, Cyto::Any& payload) noexcept final;
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override;
	void impl_on_reset_text(sys::state& state) noexcept override;

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
	void on_create(sys::state& state) noexcept override;

	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(has_tooltip(state) == tooltip_behavior::no_tooltip)
			return message_result::unseen;
		return type == mouse_probe_type::tooltip ? message_result::consumed : message_result::unseen;
	}
};

class invisible_element : public element_base {
public:
	void on_create(sys::state& state) noexcept override {
		set_visible(state, false);
	}
};

class tinted_image_element_base : public image_element_base {
public:
	uint32_t color = 0;
	tinted_image_element_base() : color(0) { }
	tinted_image_element_base(uint32_t c) : color(c) { }

	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(has_tooltip(state) == tooltip_behavior::no_tooltip)
			return message_result::unseen;
		return type == mouse_probe_type::tooltip ? message_result::consumed : message_result::unseen;
	}
};

class opaque_element_base : public image_element_base {
public:
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(type == mouse_probe_type::click || type == mouse_probe_type::tooltip)
			return message_result::consumed;
		else
			return message_result::unseen;
	}
	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		return message_result::consumed;
	}
	message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		return message_result::consumed;
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::no_tooltip;
	}
};

class partially_transparent_image : public opaque_element_base {
	uint8_t* texture = nullptr;
	int32_t size_x = 0, size_y = 0;
public:
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(type == mouse_probe_type::click || type == mouse_probe_type::tooltip) {
			if( // texture memory layout RGBA accessed through uint8_t pointer
				texture[
					(
						((x * (int32_t)state.user_settings.ui_scale) % size_x)
						+
						((y * (int32_t)state.user_settings.ui_scale) * size_x)
					) * 4 + 3
				] == 0x00
			) {
				return message_result::unseen;
			}
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}

	void on_create(sys::state& state) noexcept override {
		opaque_element_base::on_create(state);
		dcon::gfx_object_id gid;
		if(base_data.get_element_type() == element_type::image) {
			gid = base_data.data.image.gfx_object;
		} else if(base_data.get_element_type() == element_type::button) {
			gid = base_data.data.button.button_image;
		}
		assert(gid);
		dcon::texture_id tid = state.ui_defs.gfx[gid].primary_texture_handle;
		auto& texhandle = state.open_gl.asset_textures[tid];
		texture = texhandle.data;
		size_x = texhandle.size_x;
		size_y = texhandle.size_y;
		assert(texture);
	}

	// MAYBE this function has to be changed when make_element_by_type() is changed
	static std::unique_ptr<partially_transparent_image> make_element_by_type_alias(sys::state& state, dcon::gui_def_id id) {
		auto res = std::make_unique<partially_transparent_image>();
		std::memcpy(&(res->base_data), &(state.ui_defs.gui[id]), sizeof(ui::element_data));

		dcon::gfx_object_id gfx_handle;

		if(res->base_data.get_element_type() == ui::element_type::image) {
			gfx_handle = res->base_data.data.image.gfx_object;
		} else if(res->base_data.get_element_type() == ui::element_type::button) {
			gfx_handle = res->base_data.data.button.button_image;
		}

		if(gfx_handle) {
			auto tex_handle = state.ui_defs.gfx[gfx_handle].primary_texture_handle;
			if(tex_handle) {
				state.ui_defs.gfx[gfx_handle].flags |= ui::gfx_object::do_transparency_check;;
			}
		}

		make_size_from_graphics(state, res->base_data);
		res->on_create(state);
		return res;
	}
};


class progress_bar : public opaque_element_base {
public:
	float progress = 0.f;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
};

class vertical_progress_bar : public progress_bar {
public:
	void on_create(sys::state& state) noexcept override {
		std::swap(base_data.size.x, base_data.size.y);
		base_data.position.x -= base_data.size.x;
		base_data.position.y -= (base_data.size.y - base_data.size.x);
	}
};

class button_element_base : public opaque_element_base {
protected:

	std::string stored_text;
	float text_offset = 0.0f;
	bool black_text = true;

public:
	button_element_base() {
		interactable = true;
	}

	void set_button_text(sys::state& state, std::string const& new_text);
	void on_reset_text(sys::state& state) noexcept override;

	virtual void button_action(sys::state& state) noexcept { }
	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		if(state.user_settings.left_mouse_click_hold_and_release) {
			if(!disabled) {
				//ToDo: Make button change appearance while pressed
				//disabled = true;
			}
		} else if(!disabled) {
			sound::play_interface_sound(state, sound::get_click_sound(state),
						state.user_settings.interface_volume * state.user_settings.master_volume);
			button_action(state);
		}
		return message_result::consumed;
	}
	message_result on_lbutton_up(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods, bool under_mouse) noexcept override {
		if(state.user_settings.left_mouse_click_hold_and_release) {
			if(under_mouse) {
				//disabled = false;
				sound::play_interface_sound(state, sound::get_click_sound(state),
						state.user_settings.interface_volume * state.user_settings.master_volume);
				button_action(state);
			} else {
				//ToDo: Make button revert appearance when released
				//disabled = false;
			}
		}
		return message_result::consumed;
	}
	message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept override {
		if(!disabled && base_data.get_element_type() == element_type::button && base_data.data.button.shortcut == key) {
			sound::play_interface_sound(state, sound::get_click_sound(state),
					state.user_settings.interface_volume * state.user_settings.master_volume);
			button_action(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
	void on_create(sys::state& state) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
};

class tinted_button_element_base : public button_element_base {
public:
	uint32_t color = 0;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
};

class right_click_button_element_base : public button_element_base {
public:
	virtual void button_right_action(sys::state& state) noexcept { }
	message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept final {
		if(!disabled) {
			sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
			button_right_action(state);
		}
		return message_result::consumed;
	}
};

class tinted_right_click_button_element_base : public tinted_button_element_base {
public:
	virtual void button_right_action(sys::state& state) noexcept { }
	message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept final {
		if(!disabled) {
			sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
			button_right_action(state);
		}
		return message_result::consumed;
	}
};

class shift_button_element_base : public button_element_base {
public:
	virtual void button_shift_action(sys::state& state) noexcept { }
	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept final {
		if(state.user_settings.left_mouse_click_hold_and_release) {
			if(!disabled) {
				//ToDo: Make button change appearance while pressed
				//disabled = true;
			}
		} else if(!disabled) {
			sound::play_interface_sound(state, sound::get_click_sound(state),
					state.user_settings.interface_volume * state.user_settings.master_volume);
			if(mods == sys::key_modifiers::modifiers_shift)
				button_shift_action(state);
			else
				button_action(state);
		}
		return message_result::consumed;
	}
	message_result on_lbutton_up(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods, bool under_mouse) noexcept override {
		if(state.user_settings.left_mouse_click_hold_and_release) {
			if(under_mouse) {
				sound::play_interface_sound(state, sound::get_click_sound(state),
					state.user_settings.interface_volume * state.user_settings.master_volume);
				if(mods == sys::key_modifiers::modifiers_shift)
					button_shift_action(state);
				else
					button_action(state);
			} else {
				//ToDo: Make button revert appearance when released
				//disabled = false;
			}
		}
		return message_result::consumed;
	}
	message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept final {
		if(!disabled && base_data.get_element_type() == element_type::button && base_data.data.button.shortcut == key) {
			sound::play_interface_sound(state, sound::get_click_sound(state),
					state.user_settings.interface_volume * state.user_settings.master_volume);
			if(mods == sys::key_modifiers::modifiers_shift)
				button_shift_action(state);
			else
				button_action(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};


class ctrl_shift_button_element_base : public button_element_base {
public:
	virtual void button_shift_action(sys::state& state) noexcept { }
	virtual void button_ctrl_action(sys::state& state) noexcept { };
	virtual void buttons_ctrl_shift_action(sys::state& state) noexcept { };

	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept final {
		if(state.user_settings.left_mouse_click_hold_and_release) {
			if(!disabled) {
				//ToDo: Make button change appearance while pressed
				//disabled = true;
			}
		} else if(!disabled) {
			sound::play_interface_sound(state, sound::get_click_sound(state),
					state.user_settings.interface_volume * state.user_settings.master_volume);
			if(mods == sys::key_modifiers::modifiers_ctrl_shift)
				buttons_ctrl_shift_action(state);
			else if(mods == sys::key_modifiers::modifiers_shift)
				button_shift_action(state);
			else if(mods == sys::key_modifiers::modifiers_ctrl)
				button_ctrl_action(state);
			else
				button_action(state);
		}
		return message_result::consumed;
	}
	message_result on_lbutton_up(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods, bool under_mouse) noexcept override {
		if(state.user_settings.left_mouse_click_hold_and_release) {
			if(under_mouse) {
				sound::play_interface_sound(state, sound::get_click_sound(state),
					state.user_settings.interface_volume * state.user_settings.master_volume);
				if(mods == sys::key_modifiers::modifiers_ctrl_shift)
					buttons_ctrl_shift_action(state);
				else if(mods == sys::key_modifiers::modifiers_shift)
					button_shift_action(state);
				else if(mods == sys::key_modifiers::modifiers_ctrl)
					button_ctrl_action(state);
				else
					button_action(state);
			} else {
				//ToDo: Make button revert appearance when released
				//disabled = false;
			}
		}
		return message_result::consumed;
	}
	message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept final {
		if(!disabled && base_data.get_element_type() == element_type::button && base_data.data.button.shortcut == key) {
			sound::play_interface_sound(state, sound::get_click_sound(state),
					state.user_settings.interface_volume * state.user_settings.master_volume);
			if(mods == sys::key_modifiers::modifiers_ctrl_shift)
				buttons_ctrl_shift_action(state);
			else if(mods == sys::key_modifiers::modifiers_shift)
				button_shift_action(state);
			else if(mods == sys::key_modifiers::modifiers_ctrl)
				button_ctrl_action(state);
			else
				button_action(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class shift_right_button_element_base : public shift_button_element_base {
public:
	virtual void button_right_action(sys::state& state) noexcept { }
	virtual void button_shift_right_action(sys::state& state) noexcept { }
	message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept final {
		if(!disabled) {
			sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
			if(mods == sys::key_modifiers::modifiers_shift)
				button_shift_right_action(state);
			else
				button_right_action(state);
		}
		return message_result::consumed;
	}
};

class line_graph : public element_base {
private:
	ogl::lines lines;
public:
	bool is_coloured = false;
	float r = 0.f;
	float g = 0.f;
	float b = 0.f;

	const uint32_t count;

	line_graph(uint32_t sz) : lines(sz), count(sz) {
		is_coloured = false;
		r = 0.f;
		g = 0.f;
		b = 0.f;
	}

	void set_data_points(sys::state& state, std::vector<float> const& datapoints) noexcept;
	void set_data_points(sys::state& state, std::vector<float> const& datapoints, float min, float max) noexcept;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;

	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(has_tooltip(state) == tooltip_behavior::no_tooltip)
			return message_result::unseen;
		return type == mouse_probe_type::tooltip ? message_result::consumed : message_result::unseen;
	}
};

class simple_text_element_base : public element_base {
protected:
	std::string stored_text;
	float text_offset = 0.0f;

public:
	bool black_text = true;
	int32_t data = 0;
	int32_t casualties = 0;

	void set_text(sys::state& state, std::string const& new_text);
	void on_reset_text(sys::state& state) noexcept override;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;

	std::string_view get_text(sys::state& state) const {
		return stored_text;
	}

	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(has_tooltip(state) == tooltip_behavior::no_tooltip)
			return message_result::unseen;
		return type == mouse_probe_type::tooltip ? message_result::consumed : message_result::unseen;
	}
};

class color_text_element : public simple_text_element_base {
public:
	text::text_color color = text::text_color::black;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
};

class simple_body_text : public simple_text_element_base {
public:
	void on_create(sys::state& state) noexcept override;
	void set_text(sys::state& state, std::string const& new_text);
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	void on_reset_text(sys::state& state) noexcept override;
};

class edit_box_element_base : public simple_text_element_base {
protected:
	int32_t edit_index = 0;

public:
	virtual void edit_box_tab(sys::state& state, std::string_view s) noexcept { }
	virtual void edit_box_enter(sys::state& state, std::string_view s) noexcept { }
	virtual void edit_box_update(sys::state& state, std::string_view s) noexcept { }
	virtual void edit_box_up(sys::state& state) noexcept { }
	virtual void edit_box_down(sys::state& state) noexcept { }
	virtual void edit_box_esc(sys::state& state) noexcept { }
	virtual void edit_box_backtick(sys::state& state) noexcept { }
	virtual void edit_index_position(sys::state& state, int32_t index) noexcept {
		edit_index = index;
	}
	void on_reset_text(sys::state& state) noexcept override;
	void on_create(sys::state& state) noexcept override;

	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		return message_result::consumed;
	}
	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept override;
	void on_text(sys::state& state, char ch) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
};

class tool_tip : public element_base {
public:
	text::layout internal_layout;
	tool_tip() { }
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
	void on_create(sys::state& state) noexcept override;
	void on_drag(sys::state& state, int32_t oldx, int32_t oldy, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;

	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(has_tooltip(state) == tooltip_behavior::no_tooltip)
			return message_result::unseen;
		return type == mouse_probe_type::tooltip ? message_result::consumed : message_result::unseen;
	}
};

class main_window_element_base : public window_element_base {
public:
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		return message_result::consumed;
	}
};

template<class TabT>
class generic_tabbed_window : public main_window_element_base {
public:
	TabT active_tab = TabT();
};

class generic_close_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			parent->set_visible(state, false);
		}
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

template<class RowConT>
class wrapped_listbox_row_content {
public:
	RowConT content;
	wrapped_listbox_row_content() {
		content = RowConT{};
	}
	wrapped_listbox_row_content(RowConT con) {
		content = con;
	}
};

template<class ItemWinT, class ItemConT>
class overlapping_listbox_element_base : public window_element_base {
private:
	int16_t subwindow_width = 0;

protected:
	std::vector<ItemWinT*> windows{};

	virtual std::string_view get_row_element_name() {
		return std::string_view{};
	}

	virtual void update_subwindow(sys::state& state, ItemWinT& subwindow, ItemConT content) {
		send(state, &subwindow, wrapped_listbox_row_content<ItemConT>(content));
		subwindow.impl_on_update(state);
	}

public:
	std::vector<ItemConT> row_contents{};
	void update(sys::state& state);
};

class flag_button : public button_element_base {
protected:
	GLuint flag_texture_handle = 0;

public:
	virtual dcon::national_identity_id get_current_nation(sys::state& state) noexcept;
	virtual dcon::rebel_faction_id get_current_rebel_faction(sys::state& state) noexcept;
	virtual void set_current_nation(sys::state& state, dcon::national_identity_id identity) noexcept;
	void button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
};

class flag_button2 : public button_element_base {
public:
	GLuint flag_texture_handle = 0;

	void button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
};

class overlapping_flags_flag_button : public flag_button {
private:
	dcon::national_identity_id stored_identity{};

public:
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		return stored_identity;
	}
	void set_current_nation(sys::state& state, dcon::national_identity_id identity) noexcept override {
		stored_identity = identity;
		flag_button::set_current_nation(state, identity);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
};

class overlapping_flags_box : public overlapping_listbox_element_base<overlapping_flags_flag_button, dcon::national_identity_id> {
protected:
	dcon::nation_id current_nation{};

	virtual void populate_flags(sys::state& state) { }

public:
	std::string_view get_row_element_name() override;
	void update_subwindow(sys::state& state, overlapping_flags_flag_button& subwindow, dcon::national_identity_id content) override;
	void on_update(sys::state& state) noexcept override;
};

class overlapping_sphere_flags : public overlapping_flags_box {
protected:
	void populate_flags(sys::state& state) override;
};

class overlapping_friendly_flags : public overlapping_flags_box {
protected:
	void populate_flags(sys::state& state) override;
};

class overlapping_cordial_flags : public overlapping_flags_box {
protected:
	void populate_flags(sys::state& state) override;
};

class overlapping_puppet_flags : public overlapping_flags_box {
protected:
	void populate_flags(sys::state& state) override;
};

class overlapping_ally_flags : public overlapping_flags_box {
protected:
	void populate_flags(sys::state& state) override;
};

class overlapping_enemy_flags : public overlapping_flags_box {
protected:
	void populate_flags(sys::state& state) override;
};


class overlapping_truce_flag_button : public flag_button {
public:
	dcon::nation_id n;
	sys::date until;

	void upate_truce(sys::state& state, dcon::nation_id i, sys::date u) {
		n = i;
		until = u;
	}
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		return state.world.nation_get_identity_from_identity_holder(n);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
};

struct truce_pair {
	dcon::nation_id n;
	sys::date until;
};

class overlapping_truce_flags : public overlapping_listbox_element_base<overlapping_truce_flag_button, truce_pair> {
public:
	dcon::nation_id current_nation{};
	std::string_view get_row_element_name() override;
	void update_subwindow(sys::state& state, overlapping_truce_flag_button& subwindow, truce_pair content) override;
	void on_update(sys::state& state) noexcept override;
};

template<class TabT>
class generic_tab_button : public checkbox_button {
public:
	bool is_active(sys::state& state) noexcept final {
		return parent && static_cast<generic_tabbed_window<TabT>*>(parent)->active_tab == target;
	}

	void button_action(sys::state& state) noexcept final {
		send(state, parent, target);
	}

	TabT target = TabT();
};

template<class TabT>
class generic_opaque_checkbox_button : public checkbox_button {
public:
	bool is_active(sys::state& state) noexcept final {
		return parent && static_cast<generic_tabbed_window<TabT>*>(parent)->active_tab == target;
	}

	void button_action(sys::state& state) noexcept final {
		send(state, parent, target);
	}

	void on_create(sys::state& state) noexcept final{};

	TabT target = TabT();
};

template<class T>
class piechart : public element_base {
public:

	static constexpr int32_t resolution = 200;
	static constexpr size_t channels = 3;
	
	struct entry {
		float value;
		T key;
		uint8_t slices;
		entry(T key, float value) : value(value), key(key), slices(0) { }
		entry() : value(0.0f), key(), slices(0) { }
	};

	ogl::data_texture data_texture{ resolution, channels };
	std::vector<entry> distribution;
	float radius = 0.f;

	void update_chart(sys::state& state);

	void on_create(sys::state& state) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	virtual void populate_tooltip(sys::state& state, T t, float percentage, text::columnar_layout& contents) noexcept;

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::position_sensitive_tooltip;
	}
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(type == mouse_probe_type::scroll)
			return message_result::unseen;

		float dx = float(x) - radius;
		float dy = float(y) - radius;
		auto dist = sqrt(dx * dx + dy * dy);
		return dist <= radius ? message_result::consumed : message_result::unseen;
	}
	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		return message_result::consumed;
	}
	message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		return message_result::consumed;
	}
};

template<class SrcT, class DemoT>
class demographic_piechart : public piechart<DemoT> {
public:
	void on_update(sys::state& state) noexcept override;
	virtual void for_each_demo(sys::state& state, std::function<void(DemoT)> fun) { }
};

template<class SrcT>
class culture_piechart : public demographic_piechart<SrcT, dcon::culture_id> {
protected:
	void for_each_demo(sys::state& state, std::function<void(dcon::culture_id)> fun) override {
		state.world.for_each_culture(fun);
	}
};

template<class SrcT>
class workforce_piechart : public demographic_piechart<SrcT, dcon::pop_type_id> {
protected:
	void for_each_demo(sys::state& state, std::function<void(dcon::pop_type_id)> fun) override {
		state.world.for_each_pop_type(fun);
	}
};

template<class SrcT>
class ideology_piechart : public demographic_piechart<SrcT, dcon::ideology_id> {
protected:
	void for_each_demo(sys::state& state, std::function<void(dcon::ideology_id)> fun) override {
		state.world.for_each_ideology(fun);
	}
};

class scrollbar_left : public shift_right_button_element_base {
public:
	int32_t step_size = 1;
	bool hold_continous = false;
	void button_action(sys::state& state) noexcept final;
	void button_shift_action(sys::state& state) noexcept final;
	void button_shift_right_action(sys::state& state) noexcept final;
	message_result set(sys::state& state, Cyto::Any& payload) noexcept final;
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "alice_slider_controls");
	}
};

class scrollbar_right : public shift_right_button_element_base {
public:
	int32_t step_size = 1;
	bool hold_continous = false;
	void button_action(sys::state& state) noexcept final;
	void button_shift_action(sys::state& state) noexcept final;
	void button_shift_right_action(sys::state& state) noexcept final;
	message_result set(sys::state& state, Cyto::Any& payload) noexcept final;
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "alice_slider_controls");
	}
};

class scrollbar_track : public opaque_element_base {
public:
	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept final;
	tooltip_behavior has_tooltip(sys::state& state) noexcept final;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept final;
};

class scrollbar_slider : public opaque_element_base {
public:
	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept final;
	void on_drag(sys::state& state, int32_t oldx, int32_t oldy, int32_t x, int32_t y, sys::key_modifiers mods) noexcept final;
	void on_drag_finish(sys::state& state) noexcept final;
};

struct mutable_scrollbar_settings {
	int32_t lower_value = 0;
	int32_t upper_value = 100;
	int32_t lower_limit = 0;
	int32_t upper_limit = 0;
	bool using_limits = false;
};

struct scrollbar_settings {
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

protected:
	scrollbar_left* left = nullptr;
	scrollbar_right* right = nullptr;
	scrollbar_slider* slider = nullptr;

public:
	image_element_base* left_limit = nullptr;
	image_element_base* right_limit = nullptr;
	int32_t stored_value = 0;

	scrollbar_settings settings;

	scrollbar_track* track = nullptr;
	virtual void on_value_change(sys::state& state, int32_t v) noexcept { }

	void update_raw_value(sys::state& state, int32_t v);
	void update_scaled_value(sys::state& state, float v);

	float scaled_value() const;
	int32_t raw_value() const;

	void change_settings(sys::state& state, mutable_scrollbar_settings const& settings_s);

	void on_create(sys::state& state) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept final;
};

class multiline_text_element_base : public element_base {
public:
	float line_height = 0.f;
	int32_t current_line = 0;
	int32_t visible_lines = 0;
	bool black_text = true;
	text::layout internal_layout;

	void on_create(sys::state& state) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override;
};

class multiline_button_element_base : public button_element_base {
public:
	float line_height = 0.f;
	int32_t current_line = 0;
	int32_t visible_lines = 0;
	text::layout internal_layout;

	void on_create(sys::state& state) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
};

struct multiline_text_scroll_event {
	int32_t new_value;
};

class autoscaling_scrollbar : public scrollbar {
public:
	void on_create(sys::state& state) noexcept override;
	void scale_to_parent();
};

class multiline_text_scrollbar : public autoscaling_scrollbar {
public:
	void on_value_change(sys::state& state, int32_t v) noexcept override;
};

class scrollable_text : public window_element_base {
protected:
	multiline_text_scrollbar* text_scrollbar = nullptr;

public:
	multiline_text_element_base* delegate = nullptr;
	void on_create(sys::state& state) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
	void calibrate_scrollbar(sys::state& state) noexcept;
	message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override;
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(type == mouse_probe_type::scroll)
			return message_result::consumed;
		else
			return message_result::unseen;
	}
};

class single_multiline_text_element_base : public multiline_text_element_base {
public:
	dcon::text_sequence_id text_id{};

	void on_update(sys::state& state) noexcept override {
		auto layout = text::create_endless_layout(internal_layout,
				text::layout_parameters{0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y),
						base_data.data.text.font_handle, 0, text::alignment::left, black_text ? text::text_color::black : text::text_color::white, false});
		auto box = text::open_layout_box(layout, 0);
		text::add_to_layout_box(state, layout, box, text_id);
		text::close_layout_box(layout, box);
	}
};

template<class RowWinT, class RowConT>
class standard_listbox_scrollbar : public autoscaling_scrollbar {
public:
	void on_value_change(sys::state& state, int32_t v) noexcept override;
};

template<class RowConT>
class listbox_row_element_base : public window_element_base {
protected:
	RowConT content{};

public:
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
};

template<class RowConT>
class listbox_row_button_base : public button_element_base {
protected:
	RowConT content{};

public:
	virtual void update(sys::state& state) noexcept { }
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
};

template<class RowWinT, class RowConT>
class listbox_element_base : public container_base {
protected:
	std::vector<RowWinT*> row_windows{};

	virtual std::string_view get_row_element_name() {
		return std::string_view{};
	}
	virtual bool is_reversed() {
		return false;
	}

public:
	standard_listbox_scrollbar<RowWinT, RowConT>* list_scrollbar = nullptr;
	std::vector<RowConT> row_contents{};

	void update(sys::state& state);
	message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	void scroll_to_bottom(sys::state& state);
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		return type == mouse_probe_type::scroll ? message_result::consumed : message_result::unseen;
	}
};

class listbox2_scrollbar : public autoscaling_scrollbar {
public:
	void on_value_change(sys::state& state, int32_t v) noexcept override;
};

class listbox2_row_element : public window_element_base {
public:
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
};

struct listbox2_scroll_event {
};
struct listbox2_row_view {
	listbox2_row_element* row = nullptr;
};

template<typename contents_type>
class listbox2_base : public container_base {
public:
	std::vector<ui::element_base*> row_windows{};
	listbox2_scrollbar* list_scrollbar = nullptr;
	int32_t stored_index = 0;
	int32_t visible_row_count = 0;
	bool scrollbar_is_internal = false;

	listbox2_base() { }
	listbox2_base(bool scrollbar_is_internal) : scrollbar_is_internal(scrollbar_is_internal) { }

	virtual std::unique_ptr<element_base> make_row(sys::state& state) noexcept = 0;

	std::vector<contents_type> row_contents;

	message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;

	void on_create(sys::state& state) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;

	void resize(sys::state& state, int32_t height);

	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		return type == mouse_probe_type::scroll ? message_result::consumed : message_result::unseen;
	}
};

template<typename T>
struct element_selection_wrapper {
	T data{};
};


enum class outline_color : uint8_t {
	gray = 0, gold = 1, blue = 2, cyan = 3, red = 4
};

using unit_var = std::variant<std::monostate, dcon::army_id, dcon::navy_id>;

class unit_frame_bg : public shift_button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		frame = int32_t(retrieve<outline_color>(state, parent));
	}
	void button_action(sys::state& state) noexcept override {
		auto u = retrieve< unit_var>(state, parent);
		if(std::holds_alternative<dcon::army_id>(u)) {
			auto a = std::get<dcon::army_id>(u);
			if(state.world.army_get_controller_from_army_control(a) == state.local_player_nation) {
				state.selected_armies.clear();
				state.selected_navies.clear();
				state.select(a);
			}
		} else if(std::holds_alternative<dcon::navy_id>(u)) {
			auto a = std::get<dcon::navy_id>(u);
			if(state.world.navy_get_controller_from_navy_control(a) == state.local_player_nation) {
				state.selected_armies.clear();
				state.selected_navies.clear();
				state.select(a);
			}
		}
	}
	void button_shift_action(sys::state& state) noexcept override {
		auto u = retrieve< unit_var>(state, parent);
		if(std::holds_alternative<dcon::army_id>(u)) {
			auto a = std::get<dcon::army_id>(u);
			if(state.world.army_get_controller_from_army_control(a) == state.local_player_nation) {
				if(!state.is_selected(a))
					state.select(a);
				else
					state.deselect(a);
			}
		} else if(std::holds_alternative<dcon::navy_id>(u)) {
			auto a = std::get<dcon::navy_id>(u);
			if(state.world.navy_get_controller_from_navy_control(a) == state.local_player_nation) {
				if(!state.is_selected(a))
					state.select(a);
				else
					state.deselect(a);
			}
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
};

class unit_org_bar : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto u = retrieve< unit_var>(state, parent);
		float avg = 0;
		if(std::holds_alternative<dcon::army_id>(u)) {
			float total = 0;
			float count = 0;
			auto a = std::get<dcon::army_id>(u);
			for(auto r : state.world.army_get_army_membership(a)) {
				total += r.get_regiment().get_org();
				++count;
			}
			if(count > 0) {
				avg = total / count;
			}
		} else if(std::holds_alternative<dcon::navy_id>(u)) {
			float total = 0;
			float count = 0;
			auto a = std::get<dcon::navy_id>(u);
			for(auto r : state.world.navy_get_navy_membership(a)) {
				total += r.get_ship().get_org();
				++count;
			}
			if(count > 0) {
				avg = total / count;
			}
		}
		progress = avg;
	}
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		return message_result::unseen;
	}
};

inline int32_t status_frame(sys::state& state, dcon::army_id a) {
	auto is_understr = [&]() {
		for(auto m : state.world.army_get_army_membership(a)) {
			if(m.get_regiment().get_strength() < 1.0f)
				return true;
		}
		return false;
	};
	if(state.world.army_get_black_flag(a)) {
		return 1;
	} else if(state.world.army_get_is_retreating(a)) {
		return 2;
	} else if(state.world.army_get_battle_from_army_battle_participation(a)) {
		return 6;
	} else if(state.world.army_get_navy_from_army_transport(a)) {
		return 5;
	} else if(state.world.army_get_arrival_time(a)) {
		return 7;
	} else if(military::will_recieve_attrition(state, a)) {
		return 3;
	} else if(is_understr()) {
		return 4;
	} else {
		return 0;
	}
}
inline int32_t status_frame(sys::state& state, dcon::navy_id a) {
	auto trange = state.world.navy_get_army_transport(a);

	auto is_understr = [&]() {
		for(auto m : state.world.navy_get_navy_membership(a)) {
			if(m.get_ship().get_strength() < 1.0f)
				return true;
		}
		return false;
	};

	auto nb_level = state.world.province_get_building_level(state.world.navy_get_location_from_navy_location(a), economy::province_building_type::naval_base);
	if(state.world.navy_get_is_retreating(a)) {
		return 2;
	} else if(state.world.navy_get_battle_from_navy_battle_participation(a)) {
		return 6;
	} else if(military::will_recieve_attrition(state, a)) {
		return 3;
	} else if(trange.begin() != trange.end()) {
		return 5;
	} else if(!(state.world.navy_get_arrival_time(a)) && nb_level > 0 && is_understr()) {
		return 4;
	} else if(state.world.navy_get_arrival_time(a)) {
		return 7;
	} else {
		return 0;
	}
}

class unit_status_image : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto u = retrieve< unit_var>(state, parent);

		if(std::holds_alternative<dcon::army_id>(u)) {
			auto a = std::get<dcon::army_id>(u);
			frame = status_frame(state, a);
		} else if(std::holds_alternative<dcon::navy_id>(u)) {
			auto a = std::get<dcon::navy_id>(u);
			frame = status_frame(state, a);
		}
	}
};

class unit_strength : public color_text_element {
public:
	void on_update(sys::state& state) noexcept override {
		auto u = retrieve< unit_var>(state, parent);
		color = text::text_color::gold;
		

		if(std::holds_alternative<dcon::army_id>(u)) {
			int64_t total = 0;
			auto a = std::get<dcon::army_id>(u);

			for(auto m : state.world.army_get_army_membership(a)) {
				total += int64_t(floor(m.get_regiment().get_strength() * state.defines.pop_size_per_regiment));
			}
			set_text(state, text::prettify(total));
		} else if(std::holds_alternative<dcon::navy_id>(u)) {
			float total = 0.0f;
			auto a = std::get<dcon::navy_id>(u);

			for(auto m : state.world.navy_get_navy_membership(a)) {
				total += m.get_ship().get_strength();
			}
			set_text(state, text::format_float(total, 1));
		}

	}
};

class unit_dig_in_pips_image : public image_element_base {
public:
	bool visible = false;

	void on_update(sys::state& state) noexcept override {
		auto u = retrieve< unit_var>(state, parent);

		if(std::holds_alternative<dcon::army_id>(u)) {
			auto a = std::get<dcon::army_id>(u);
			auto dig_in = state.world.army_get_dig_in(a);
			frame = dig_in;
			visible = true;
		} else {
			visible = false;
		}

	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible) {
			image_element_base::render(state, x, y);
		}
	}
};

class unit_most_prevalent : public image_element_base {
public:
	bool visible = true;

	void on_update(sys::state& state) noexcept override {
		static std::vector<int32_t> by_icon_count;
		for(auto& i : by_icon_count)
			i = 0;

		auto u = retrieve< unit_var>(state, parent);

		visible = true;
		if(std::holds_alternative<dcon::army_id>(u)) {
			auto a = std::get<dcon::army_id>(u);
			if(state.world.army_get_controller_from_army_control(a) != state.local_player_nation) {
				visible = false;
				return;
			}

			for(auto m : state.world.army_get_army_membership(a)) {
				auto icon = state.military_definitions.unit_base_definitions[m.get_regiment().get_type()].icon - 1;
				if(uint32_t(icon) >= by_icon_count.size()) {
					by_icon_count.resize(icon + 1, 0);
				}
				++(by_icon_count[icon]);
			}
		} else if(std::holds_alternative<dcon::navy_id>(u)) {
			auto a = std::get<dcon::navy_id>(u);
			if(state.world.navy_get_controller_from_navy_control(a) != state.local_player_nation) {
				visible = false;
				return;
			}

			for(auto m : state.world.navy_get_navy_membership(a)) {
				auto icon = state.military_definitions.unit_base_definitions[m.get_ship().get_type()].icon - 1;
				if(uint32_t(icon) >= by_icon_count.size()) {
					by_icon_count.resize(icon + 1, 0);
				}
				++(by_icon_count[icon]);
			}
		}

		int32_t max_index = 0;
		for(uint32_t i = 1; i < by_icon_count.size(); ++i) {
			if(by_icon_count[i] > by_icon_count[max_index])
				max_index = int32_t(i);
		}

		frame = max_index;
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
	}
};

class unit_second_most_prevalent : public image_element_base {
public:
	bool visible = true;

	void on_update(sys::state& state) noexcept override {
		static std::vector<int32_t> by_icon_count;
		for(auto& i : by_icon_count)
			i = 0;

		auto u = retrieve< unit_var>(state, parent);

		visible = true;
		if(std::holds_alternative<dcon::army_id>(u)) {
			auto a = std::get<dcon::army_id>(u);
			if(state.world.army_get_controller_from_army_control(a) != state.local_player_nation) {
				visible = false;
				return;
			}

			for(auto m : state.world.army_get_army_membership(a)) {
				auto icon = state.military_definitions.unit_base_definitions[m.get_regiment().get_type()].icon - 1;
				if(uint32_t(icon) >= by_icon_count.size()) {
					by_icon_count.resize(icon + 1, 0);
				}
				++(by_icon_count[icon]);
			}
		} else if(std::holds_alternative<dcon::navy_id>(u)) {
			auto a = std::get<dcon::navy_id>(u);
			if(state.world.navy_get_controller_from_navy_control(a) != state.local_player_nation) {
				visible = false;
				return;
			}

			for(auto m : state.world.navy_get_navy_membership(a)) {
				auto icon = state.military_definitions.unit_base_definitions[m.get_ship().get_type()].icon - 1;
				if(uint32_t(icon) >= by_icon_count.size()) {
					by_icon_count.resize(icon + 1, 0);
				}
				++(by_icon_count[icon]);
			}
		}

		if(by_icon_count.size() == 0) {
			visible = false;
			return;
		}

		{
			int32_t max_index = 0;
			for(uint32_t i = 1; i < by_icon_count.size(); ++i) {
				if(by_icon_count[i] > by_icon_count[max_index])
					max_index = int32_t(i);
			}
			by_icon_count[max_index] = 0;
		}
		{
			int32_t max_index = 0;
			for(uint32_t i = 1; i < by_icon_count.size(); ++i) {
				if(by_icon_count[i] > by_icon_count[max_index])
					max_index = int32_t(i);
			}
			if(by_icon_count[max_index] == 0) {
				visible = false;
			} else {
				visible = true;
				frame = max_index;
			}
		}
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
	}
};

class unit_controller_flag : public flag_button2 {
public:
	bool visible = true;

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			auto u = retrieve< unit_var>(state, parent);
			if(std::holds_alternative<dcon::army_id>(u)) {
				auto a = std::get<dcon::army_id>(u);
				payload.emplace<dcon::nation_id>(state.world.army_get_controller_from_army_control(a));
			} else if(std::holds_alternative<dcon::navy_id>(u)) {
				auto a = std::get<dcon::navy_id>(u);
				payload.emplace<dcon::nation_id>(state.world.navy_get_controller_from_navy_control(a));
			} else {
				payload.emplace<dcon::nation_id>(dcon::nation_id{});
			}
			return message_result::consumed;
		} else if(payload.holds_type<dcon::rebel_faction_id>()) {
			auto u = retrieve< unit_var>(state, parent);
			if(std::holds_alternative<dcon::army_id>(u)) {
				auto a = std::get<dcon::army_id>(u);
				payload.emplace<dcon::rebel_faction_id>(state.world.army_get_controller_from_army_rebel_control(a));
			} else {
				payload.emplace<dcon::rebel_faction_id>(dcon::rebel_faction_id{});
			}
			return message_result::consumed;
		}
		return message_result::unseen;
	}
	void on_update(sys::state& state) noexcept override {

		visible = true;
		auto u = retrieve< unit_var>(state, parent);

		if(std::holds_alternative<dcon::army_id>(u)) {
			auto a = std::get<dcon::army_id>(u);
			if(state.world.army_get_controller_from_army_control(a) == state.local_player_nation) {
				visible = false;
				return;
			}

		} else if(std::holds_alternative<dcon::navy_id>(u)) {
			auto a = std::get<dcon::navy_id>(u);
			if(state.world.navy_get_controller_from_navy_control(a) == state.local_player_nation) {
				visible = false;
				return;
			}
		} else {
			visible = false;
			return;
		}

		flag_button2::on_update(state);
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			flag_button2::render(state, x, y);
	}
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(visible)
			return flag_button2::impl_probe_mouse(state, x, y, type);
		else
			return mouse_probe{ nullptr, ui::xy_pair{} };
	}
};

inline outline_color to_color(sys::state& state, unit_var display_unit) {
	dcon::nation_id controller;
	bool selected = false;
	if(std::holds_alternative<dcon::army_id>(display_unit)) {
		controller = state.world.army_get_controller_from_army_control(std::get<dcon::army_id>(display_unit));
		selected = state.is_selected(std::get<dcon::army_id>(display_unit));
	} else if(std::holds_alternative<dcon::navy_id>(display_unit)) {
		controller = state.world.navy_get_controller_from_navy_control(std::get<dcon::navy_id>(display_unit));
		selected = state.is_selected(std::get<dcon::navy_id>(display_unit));
	} else {
		return outline_color::gray;
	}

	if(selected) {
		return outline_color::gold;
	} else if(controller == state.local_player_nation) {
		return outline_color::blue;
	} else if(!controller || military::are_at_war(state, controller, state.local_player_nation)) {
		return outline_color::red;
	} else if(military::are_allied_in_war(state, controller, state.local_player_nation)) {
		return outline_color::cyan;
	} else {
		return outline_color::gray;
	}
}

inline bool color_equivalent(outline_color a, outline_color b) {
	switch(a) {
		case outline_color::blue:
		case outline_color::gold:
			return b == outline_color::gold || b == outline_color::blue;
		default:
			return a == b;
	}
}
inline bool color_less(outline_color a, outline_color b) {
	if(a == outline_color::gray)
		return false;
	if(b == outline_color::gray)
		return true;
	return int32_t(a) < int32_t(b);
}

class base_unit_container : public window_element_base {
public:
	unit_var display_unit;
	outline_color color = outline_color::gray;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "frame_bg") {
			return make_element_by_type<unit_frame_bg>(state, id);
		} else if(name == "org_bar") {
			return make_element_by_type<unit_org_bar>(state, id);
		} else if(name == "status") {
			return make_element_by_type<unit_status_image>(state, id);
		} else if(name == "dig_in") {
			return make_element_by_type<unit_dig_in_pips_image>(state, id);
		} else if(name == "strength") {
			return make_element_by_type<unit_strength>(state, id);
		} else if(name == "unit_2") {
			return make_element_by_type<unit_second_most_prevalent>(state, id);
		} else if(name == "unit_1") {
			return make_element_by_type<unit_most_prevalent>(state, id);
		} else if(name == "controller_flag") {
			return make_element_by_type<unit_controller_flag>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		color = to_color(state, display_unit);
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<unit_var>()) {
			payload.emplace<unit_var>(display_unit);
			return message_result::consumed;
		} else if(payload.holds_type<outline_color>()) {
			payload.emplace<outline_color>(color);
			return message_result::consumed;
		}
		return message_result::unseen;
	}

	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(!std::holds_alternative<std::monostate>(display_unit)) {
			window_element_base::impl_render(state, x, y);
		}
	}

	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(!std::holds_alternative<std::monostate>(display_unit)) {
			return window_element_base::impl_probe_mouse(state, x, y, type);
		} else {
			return mouse_probe{ nullptr, ui::xy_pair{} };
		}
	}
};

using grid_row = std::array<unit_var, 4>;

inline bool unit_var_ordering(sys::state& state, unit_var a, unit_var b) {
	if(std::holds_alternative<std::monostate>(a))
		return false;
	if(std::holds_alternative<std::monostate>(b))
		return true;
	if(std::holds_alternative<dcon::navy_id>(a) && std::holds_alternative<dcon::army_id>(b))
		return true;
	if(std::holds_alternative<dcon::army_id>(a) && std::holds_alternative<dcon::navy_id>(b))
		return false;

	auto a_color = to_color(state, a);
	auto b_color = to_color(state, b);

	if(!color_equivalent(a_color, b_color)) {
		return color_less(a_color, b_color);
	}

	if(std::holds_alternative<dcon::army_id>(a)) {
		auto aa = std::get<dcon::army_id>(a);
		auto ba = std::get<dcon::army_id>(b);
		auto aar = state.world.army_get_army_membership(aa);
		auto bar = state.world.army_get_army_membership(ba);
		auto acount = int32_t(aar.end() - aar.begin());
		auto bcount = int32_t(bar.end() - bar.begin());
		if(acount != bcount)
			return acount > bcount;
		return aa.index() < ba.index();
	} else {
		auto aa = std::get<dcon::navy_id>(a);
		auto ba = std::get<dcon::navy_id>(b);
		auto aar = state.world.navy_get_navy_membership(aa);
		auto bar = state.world.navy_get_navy_membership(ba);
		auto acount = int32_t(aar.end() - aar.begin());
		auto bcount = int32_t(bar.end() - bar.begin());
		if(acount != bcount)
			return acount > bcount;
		return aa.index() < ba.index();
	}

}

class unit_grid_row : public listbox_row_element_base<grid_row> {
public:
	std::array< base_unit_container*, 4> grid_items;

	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base<grid_row>::on_create(state);

		{
			auto win = make_element_by_type<base_unit_container>(state, state.ui_state.defs_by_name.find("alice_base_unit_icon")->second.definition);
			win->base_data.position.x = int16_t(3 + 67 * 0);
			win->base_data.position.y = int16_t(3);
			grid_items[0] = win.get();
			add_child_to_front(std::move(win));
		}
		{
			auto win = make_element_by_type<base_unit_container>(state, state.ui_state.defs_by_name.find("alice_base_unit_icon")->second.definition);
			win->base_data.position.x = int16_t(3 + 67 * 1);
			win->base_data.position.y = int16_t(3);
			grid_items[1] = win.get();
			add_child_to_front(std::move(win));
		}
		{
			auto win = make_element_by_type<base_unit_container>(state, state.ui_state.defs_by_name.find("alice_base_unit_icon")->second.definition);
			win->base_data.position.x = int16_t(3 + 67 * 2);
			win->base_data.position.y = int16_t(3);
			grid_items[2] = win.get();
			add_child_to_front(std::move(win));
		}
		{
			auto win = make_element_by_type<base_unit_container>(state, state.ui_state.defs_by_name.find("alice_base_unit_icon")->second.definition);
			win->base_data.position.x = int16_t(3 + 67 * 3);
			win->base_data.position.y = int16_t(3);
			grid_items[3] = win.get();
			add_child_to_front(std::move(win));
		}
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		return nullptr;
	}

	void on_update(sys::state& state) noexcept override {
		grid_items[0]->display_unit = content[0];
		grid_items[1]->display_unit = content[1];
		grid_items[2]->display_unit = content[2];
		grid_items[3]->display_unit = content[3];
	}
};


class unit_grid_lb : public listbox_element_base<unit_grid_row, grid_row> {
public:
	std::string_view get_row_element_name() override {
		return "alice_grid_row";
	}
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();

		static std::vector<unit_var> base_array;
		base_array.clear();

		auto prov = retrieve<dcon::province_id>(state, parent);
		bool as_port = retrieve<bool>(state, parent);

		if(as_port || (prov.index() >= state.province_definitions.first_sea_province.index())) {
			for(auto n : state.world.province_get_navy_location(prov)) {
				base_array.emplace_back(n.get_navy().id);
			}
		}
		if(as_port) {
			for(auto a : state.world.province_get_army_location(prov)) {
				if(a.get_army().get_navy_from_army_transport()) {
					base_array.emplace_back(a.get_army().id);
				}
			}
		} else if(prov.index() >= state.province_definitions.first_sea_province.index()) {
			for(auto a : state.world.province_get_army_location(prov)) {
				base_array.emplace_back(a.get_army().id);
			}
		} else {
			for(auto a : state.world.province_get_army_location(prov)) {
				if(!(a.get_army().get_navy_from_army_transport())) {
					base_array.emplace_back(a.get_army().id);
				}
			}
		}
		for(uint32_t i = 0; i < base_array.size(); i += 4) {
			grid_row new_row;
			for(uint32_t j = 0; j < 4 && (i + j) < base_array.size(); ++j) {
				new_row[j] = base_array[i + j];
			}
			row_contents.push_back(new_row);
		}

		update(state);
	}
};

class grid_box : public window_element_base {
public:
	bool as_port = false;
	dcon::province_id for_province;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "grid_listbox") {
			return make_element_by_type<unit_grid_lb>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::province_id>()) {
			payload.emplace<dcon::province_id>(for_province);
			return message_result::consumed;
		} else if(payload.holds_type<bool>()) {
			payload.emplace<bool>(as_port);
			return message_result::consumed;
		}
		return message_result::unseen;
	}

	void open(sys::state& state, ui::xy_pair location, ui::xy_pair source_size, dcon::province_id p, bool port) {
		if(!as_port) {
			if(p.index() < state.province_definitions.first_sea_province.index()) {
				auto arange = state.world.province_get_army_location(p);
				if(int32_t(arange.end() - arange.begin()) <= 1)
					return;
			} else {
				auto arange = state.world.province_get_army_location(p);
				auto nrange = state.world.province_get_navy_location(p);
				if(int32_t(nrange.end() - nrange.begin()) <= 1 && arange.begin() == arange.end())
					return;
			}
		}

		as_port = port;
		for_province = p;

		auto mx = int32_t(state.mouse_x_position / state.user_settings.ui_scale);
		auto my = int32_t(state.mouse_y_position / state.user_settings.ui_scale);
		if(state.ui_state.mouse_sensitive_target)
			state.ui_state.mouse_sensitive_target->set_visible(state, false);

		if(location.y + source_size.y + 3 + base_data.size.y < state.ui_state.root->base_data.size.y) { // position below
			auto desired_x = location.x + source_size.x / 2 - base_data.size.x / 2;
			auto actual_x = std::clamp(desired_x, 0, state.ui_state.root->base_data.size.x - base_data.size.x);
			base_data.position.x = int16_t(actual_x);
			base_data.position.y = int16_t(location.y + source_size.y + 3);

			state.ui_state.target_ul_bounds.x = int16_t(std::min(mx - 4, actual_x));
			state.ui_state.target_ul_bounds.y = int16_t(std::min(my - 4, location.y + source_size.y + 3));
			state.ui_state.target_lr_bounds.x = int16_t(std::max(mx + 4, actual_x + base_data.size.x));
			state.ui_state.target_lr_bounds.y = int16_t(std::max(my + 4, location.y + source_size.y + 3 + base_data.size.y));

		} else if(location.x + source_size.x + 3 + base_data.size.x < state.ui_state.root->base_data.size.x) { // position right
			auto desired_y = location.y + source_size.y / 2 - base_data.size.y / 2;

			auto actual_y = std::clamp(desired_y, 0, state.ui_state.root->base_data.size.y - base_data.size.y);
			base_data.position.x = int16_t(location.x + source_size.x + 3);
			base_data.position.y = int16_t(actual_y);

			state.ui_state.target_ul_bounds.x = int16_t(std::min(mx - 4, location.x + source_size.x + 3));
			state.ui_state.target_ul_bounds.y = int16_t(std::min(my - 4, actual_y));
			state.ui_state.target_lr_bounds.x = int16_t(std::max(mx + 4, location.x + source_size.x + 3 + base_data.size.x));
			state.ui_state.target_lr_bounds.y = int16_t(std::max(my + 4, actual_y + base_data.size.y));
		} else { //position left
			auto desired_y = location.y + source_size.y / 2 - base_data.size.y / 2;

			auto actual_y = std::clamp(desired_y, 0, state.ui_state.root->base_data.size.y - base_data.size.y);
			base_data.position.x = int16_t(location.x - base_data.size.x - 3);
			base_data.position.y = int16_t(actual_y);

			state.ui_state.target_ul_bounds.x = int16_t(std::min(mx - 4, location.x - base_data.size.x - 3));
			state.ui_state.target_ul_bounds.y = int16_t(std::min(my - 4, actual_y));
			state.ui_state.target_lr_bounds.x = int16_t(std::max(mx + 4, location.x - base_data.size.x - 3));
			state.ui_state.target_lr_bounds.y = int16_t(std::max(my + 4, actual_y + base_data.size.y));
		}


		state.ui_state.mouse_sensitive_target = this;
		set_visible(state, true);
	}
};

void populate_shortcut_tooltip(sys::state& state, ui::element_base& elm, text::columnar_layout& contents) noexcept;

} // namespace ui
