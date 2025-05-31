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
#include "color.hpp"
#include "demographics.hpp"

namespace ui {

void render_text_chunk(
	sys::state& state,
	text::text_chunk t,
	float x,
	float baseline_y,
	uint16_t font_id,
	ogl::color3f text_color,
	ogl::color_modification cmod
);

template<typename T, typename ...Params>
std::unique_ptr<T> make_element_by_type(sys::state& state, std::string_view name, Params&&... params) { // also bypasses global creation hooks
	auto it = state.ui_state.defs_by_name.find(state.lookup_key(name));
	if(it != state.ui_state.defs_by_name.end()) {
		auto res = std::make_unique<T>(std::forward<Params>(params)...);
		std::memcpy(&(res->base_data), &(state.ui_defs.gui[it->second.definition]), sizeof(ui::element_data));
		make_size_from_graphics(state, res->base_data);
		res->on_create(state);
		return res;
	}
	return std::unique_ptr<T>{};
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
ogl::color3f get_text_color(sys::state& state, text::text_color text_color);

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

class non_owning_container_base : public element_base {
public:
	std::vector<element_base*> children;

	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override;
	message_result impl_on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept final;
	void impl_on_update(sys::state& state) noexcept override;
	message_result impl_set(sys::state& state, Cyto::Any& payload) noexcept final;
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override;
	void impl_on_reset_text(sys::state& state) noexcept override;

	void move_child_to_front(element_base* child) noexcept final;
	void move_child_to_back(element_base* child) noexcept final;
	element_base* get_child_by_index(sys::state const& state, int32_t index) noexcept final;
};

class image_element_base : public element_base {
public:
	int32_t frame = 0; // if it is from an icon strip
	bool disabled = false;
	bool interactable = false;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	void on_create(sys::state& state) noexcept override;

	virtual bool get_horizontal_flip(sys::state& state) noexcept {
		return state.world.locale_get_native_rtl(state.font_collection.get_current_locale());
	}

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
};

class opaque_element_base : public image_element_base {
public:
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(type == mouse_probe_type::click || type == mouse_probe_type::tooltip)
			return message_result::consumed;
		return image_element_base::test_mouse(state, x, y, type);
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
	dcon::texture_id texture_id;
public:
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(type == mouse_probe_type::click || type == mouse_probe_type::tooltip) {
			auto& texhandle = state.open_gl.asset_textures[texture_id];
			uint8_t* texture = texhandle.data;
			int32_t size_x = texhandle.size_x;
			int32_t size_y = texhandle.size_y;
			int32_t channels = texhandle.channels;
			size_t size_m = (texhandle.size_x * texhandle.size_y) * 4;
			if(texture && channels == 4) {
				// texture memory layout RGBA accessed through uint8_t pointer
				size_t index = (x + (y * size_x)) * 4 + 3;
				if(index < size_m && texture[index] == 0x00) {
					return message_result::unseen;
				}
			}
			return message_result::consumed;
		}
		return message_result::unseen;
	}

	void on_create(sys::state& state) noexcept override {
		opaque_element_base::on_create(state);
		dcon::gfx_object_id gid;
		if(base_data.get_element_type() == element_type::image) {
			gid = base_data.data.image.gfx_object;
		} else if(base_data.get_element_type() == element_type::button) {
			gid = base_data.data.button.button_image;
		}
		if(gid) {
			texture_id = state.ui_defs.gfx[gid].primary_texture_handle;
		}
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
				state.ui_defs.gfx[gfx_handle].flags |= ui::gfx_object::do_transparency_check;
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
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		return image_element_base::test_mouse(state, x, y, type);
	}
	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		return image_element_base::on_lbutton_down(state, x, y, mods);
	}
	message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		return image_element_base::on_rbutton_down(state, x, y, mods);
	}
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
	text::layout internal_layout;
	std::string cached_text;
	bool black_text = true;

public:
	button_element_base() {
		interactable = true;
	}

	void set_button_text(sys::state& state, std::string const& new_text);
	void on_reset_text(sys::state& state) noexcept override;

	virtual void button_action(sys::state& state) noexcept { }
	virtual void button_right_action(sys::state& state) noexcept {
		button_action(state);
	}
	virtual void button_shift_action(sys::state& state) noexcept {
		button_action(state);
	}
	virtual void button_shift_right_action(sys::state& state) noexcept {
		button_action(state);
	}
	virtual void button_ctrl_action(sys::state& state) noexcept {
		button_action(state);
	}
	virtual void button_ctrl_right_action(sys::state& state) noexcept {
		button_action(state);
	}
	virtual void button_ctrl_shift_action(sys::state& state) noexcept {
		button_action(state);
	}
	virtual void button_ctrl_shift_right_action(sys::state& state) noexcept {
		button_action(state);
	}
	virtual sound::audio_instance& get_click_sound(sys::state& state) noexcept {
		return sound::get_click_sound(state);
	}

	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		if(!state.user_settings.left_mouse_click_hold_and_release && !disabled) {
			sound::play_interface_sound(state, get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
			if(mods == sys::key_modifiers::modifiers_shift)
				button_shift_action(state);
			else if(mods == sys::key_modifiers::modifiers_ctrl)
				button_ctrl_action(state);
			else
				button_action(state);
		}
		return message_result::consumed;
	}
	message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		if(!disabled) {
			sound::play_interface_sound(state, get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
			if(mods == sys::key_modifiers::modifiers_shift)
				button_shift_right_action(state);
			else if(mods == sys::key_modifiers::modifiers_ctrl)
				button_ctrl_right_action(state);
			else
				button_right_action(state);
		}
		return message_result::consumed;
	}
	message_result on_lbutton_up(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods, bool under_mouse) noexcept override {
		if(state.user_settings.left_mouse_click_hold_and_release && !disabled && under_mouse) {
			sound::play_interface_sound(state, get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
			if(mods == sys::key_modifiers::modifiers_shift)
				button_shift_action(state);
			else if(mods == sys::key_modifiers::modifiers_ctrl)
				button_ctrl_action(state);
			else
				button_action(state);
		}
		return message_result::consumed;
	}
	message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept final {
		if(!disabled && base_data.get_element_type() == element_type::button && base_data.data.button.shortcut == key) {
			sound::play_interface_sound(state, get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
			if(mods == sys::key_modifiers::modifiers_shift)
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

	void on_create(sys::state& state) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	void format_text(sys::state& state);
};

class tinted_button_element_base : public button_element_base {
public:
	uint32_t color = 0;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
};

class right_click_button_element_base : public button_element_base {
public:
	virtual void button_right_action(sys::state& state) noexcept override { }
	message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept final {
		if(!disabled) {
			sound::play_interface_sound(state, get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
			button_right_action(state);
		}
		return message_result::consumed;
	}
};

class tinted_right_click_button_element_base : public tinted_button_element_base {
public:
	virtual void button_right_action(sys::state& state) noexcept override { }
	message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept final {
		if(!disabled) {
			sound::play_interface_sound(state, get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
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
	std::string cached_text;
	text::layout internal_layout;
public:
	bool black_text = true;
	int32_t data = 0;
	int32_t casualties = 0;

	void set_text(sys::state& state, std::string const& new_text);
	void on_reset_text(sys::state& state) noexcept override;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	void format_text(sys::state& state);

	std::string_view get_text(sys::state& state) const {
		return cached_text;
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
	virtual void edit_box_back_slash(sys::state& state) noexcept { }
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
	void on_text(sys::state& state, char32_t ch) noexcept override;
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
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(type == mouse_probe_type::tooltip)
			return message_result::consumed;
		return opaque_element_base::test_mouse(state, x, y, type);
	}
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

template<class TabT>
class generic_tabbed_window : public window_element_base {
public:
	TabT active_tab = TabT();
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		return message_result::consumed;
	}
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

	sound::audio_instance& get_click_sound(sys::state& state) noexcept override {
		return sound::get_checkbox_sound(state);
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
	void update(sys::state& state) {
		auto spacing = int16_t(base_data.data.overlapping.spacing);
		if(base_data.get_element_type() == element_type::overlapping) {
			while(row_contents.size() > windows.size()) {
				auto ptr = make_element_by_type<ItemWinT>(state, get_row_element_name());
				if(subwindow_width <= 0) {
					subwindow_width = ptr->base_data.size.x;
				}
				windows.push_back(static_cast<ItemWinT*>(ptr.get()));
				add_child_to_front(std::move(ptr));
			}

			float size_ratio = float(row_contents.size() * (subwindow_width + spacing)) / float(base_data.size.x);
			int16_t offset = spacing + subwindow_width;
			if(size_ratio > 1.f) {
				offset = int16_t(float(subwindow_width) / size_ratio);
			}
			int16_t current_x = 0;
			if(base_data.data.overlapping.image_alignment == alignment::right) {
				current_x = base_data.size.x - subwindow_width - offset * int16_t(row_contents.size() - 1);
			}
			for(size_t i = 0; i < windows.size(); i++) {
				if(i < row_contents.size()) {
					update_subwindow(state, *windows[i], row_contents[i]);
					windows[i]->base_data.position.x = current_x;
					current_x += offset;
					windows[i]->set_visible(state, true);
				} else {
					windows[i]->set_visible(state, false);
				}
			}
		}
	}
};

class province_script_button : public button_element_base {
public:
	dcon::gui_def_id base_definition;

	province_script_button(dcon::gui_def_id base_definition) : base_definition(base_definition) { }
	void button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
};
class nation_script_button : public button_element_base {
public:
	dcon::gui_def_id base_definition;

	nation_script_button(dcon::gui_def_id base_definition) : base_definition(base_definition) { }
	void button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
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
	sound::audio_instance& get_click_sound(sys::state& state) noexcept override {
		return sound::get_subtab_sound(state);
	}

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
	void on_update(sys::state& state) noexcept override {
		this->distribution.clear();

		Cyto::Any obj_id_payload = SrcT{};
		size_t i = 0;
		if(this->parent) {
			this->parent->impl_get(state, obj_id_payload);
			float total_pops = 0.f;

			for_each_demo(state, [&](DemoT demo_id) {
				float volume = 0.f;
				if(obj_id_payload.holds_type<dcon::province_id>()) {
					auto demo_key = demographics::to_key(state, demo_id);
					auto prov_id = any_cast<dcon::province_id>(obj_id_payload);
					volume = state.world.province_get_demographics(prov_id, demo_key);
				} else if(obj_id_payload.holds_type<dcon::nation_id>()) {
					auto demo_key = demographics::to_key(state, demo_id);
					auto nat_id = any_cast<dcon::nation_id>(obj_id_payload);
					volume = state.world.nation_get_demographics(nat_id, demo_key);
				}

				if constexpr(std::is_same_v<SrcT, dcon::pop_id>) {
					if(obj_id_payload.holds_type<dcon::pop_id>()) {
						auto demo_key = pop_demographics::to_key(state, demo_id);
						auto pop_id = any_cast<dcon::pop_id>(obj_id_payload);
						volume = pop_demographics::get_demo(state, pop_id, demo_key);
					}
				}
				if(volume > 0)
					this->distribution.emplace_back(demo_id, volume);
			});
		}

		this->update_chart(state);
	}
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

class scrollbar_left : public button_element_base {
public:
	int32_t step_size = 1;
	sound::audio_instance& get_click_sound(sys::state& state) noexcept override {
		return sound::get_click_left_sound(state);
	}
	void button_action(sys::state& state) noexcept final;
	void button_shift_action(sys::state& state) noexcept final;
	void button_shift_right_action(sys::state& state) noexcept final;
	message_result set(sys::state& state, Cyto::Any& payload) noexcept final;
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::substitution_map sub;
		text::add_to_substitution_map(sub, text::variable_type::value, step_size);
		text::add_to_substitution_map(sub, text::variable_type::x, step_size * 5);
		text::localised_format_box(state, contents, box, std::string_view("alice_slider_controls"), sub);
		text::close_layout_box(contents, box);
	}
};

class scrollbar_right : public button_element_base {
public:
	int32_t step_size = 1;
	sound::audio_instance& get_click_sound(sys::state& state) noexcept override {
		return sound::get_click_right_sound(state);
	}
	void button_action(sys::state& state) noexcept final;
	void button_shift_action(sys::state& state) noexcept final;
	void button_shift_right_action(sys::state& state) noexcept final;
	message_result set(sys::state& state, Cyto::Any& payload) noexcept final;
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::substitution_map sub;
		text::add_to_substitution_map(sub, text::variable_type::value, step_size);
		text::add_to_substitution_map(sub, text::variable_type::x, step_size * 5);
		text::localised_format_box(state, contents, box, std::string_view("alice_slider_controls_2"), sub);
		text::close_layout_box(contents, box);
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
	void on_reset_text(sys::state& state) noexcept override;
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

	void on_reset_text(sys::state& state) noexcept override;
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
		return window_element_base::test_mouse(state, x, y, type);
	}
};

class single_multiline_text_element_base : public multiline_text_element_base {
public:
	dcon::text_key text_id{};

	void on_update(sys::state& state) noexcept override {
		auto layout = text::create_endless_layout(state, internal_layout,
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
		return (type == mouse_probe_type::scroll && row_contents.size() > row_windows.size()) ? message_result::consumed : message_result::unseen;
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
		return (type == mouse_probe_type::scroll && row_contents.size() > row_windows.size()) ? message_result::consumed : message_result::unseen;
	}
};

template<typename T>
struct element_selection_wrapper {
	T data{};
};

void populate_shortcut_tooltip(sys::state& state, ui::element_base& elm, text::columnar_layout& contents) noexcept;

void render_text_chunk(
	sys::state& state,
	text::text_chunk t,
	float x,
	float baseline_y,
	uint16_t font_id,
	ogl::color3f text_color,
	ogl::color_modification cmod
);


template<class T>
void piechart<T>::render(sys::state& state, int32_t x, int32_t y) noexcept {
	if(distribution.size() > 0)
		ogl::render_piechart(state, ogl::color_modification::none, float(x), float(y), float(base_data.size.x), data_texture);
}

template<class T>
void piechart<T>::on_create(sys::state& state) noexcept {
	base_data.position.x -= base_data.size.x;
	radius = float(base_data.size.x);
	base_data.size.x *= 2;
	base_data.size.y *= 2;
}

template<class T>
void piechart<T>::update_chart(sys::state& state) {
	std::sort(distribution.begin(), distribution.end(), [](auto const& a, auto const& b) { return a.value > b.value; });
	float total = 0.0f;
	for(auto& e : distribution) {
		total += e.value;
	}
	int32_t int_total = 0;

	if(total != 0.0f) {
		for(auto& e : distribution) {
			auto ivalue = int32_t(e.value * float(resolution) / total);
			e.slices = uint8_t(ivalue);
			e.value /= total;
			int_total += ivalue;
		}
	} else {
		distribution.clear();
	}

	if(int_total < resolution && distribution.size() > 0) {
		auto rem = resolution - int_total;
		while(rem > 0) {
			for(auto& e : distribution) {
				e.slices += uint8_t(1);
				rem -= 1;
				if(rem == 0)
					break;
			}
		}
	} else if(int_total > resolution) {
		assert(false);
	}

	size_t i = 0;
	for(auto& e : distribution) {
		uint32_t color = ogl::get_ui_color<T>(state, e.key);
		auto slice_count = size_t(e.slices);

		for(size_t j = 0; j < slice_count; j++) {
			data_texture.data[(i + j) * channels] = uint8_t(color & 0xFF);
			data_texture.data[(i + j) * channels + 1] = uint8_t(color >> 8 & 0xFF);
			data_texture.data[(i + j) * channels + 2] = uint8_t(color >> 16 & 0xFF);
		}

		i += slice_count;
	}
	for(; i < resolution; i++) {
		data_texture.data[i * channels] = uint8_t(0);
		data_texture.data[i * channels + 1] = uint8_t(0);
		data_texture.data[i * channels + 2] = uint8_t(0);
	}

	data_texture.data_updated = true;
}

template<class T>
void piechart<T>::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	float const PI = 3.141592653589793238463f;
	float dx = float(x) - radius;
	float dy = float(y) - radius;
	size_t index = 0;
	if(dx != 0.0f || dy != 0.0f) {
		float dist = std::sqrt(dx * dx + dy * dy);
		float angle = std::acos(-dx / dist);
		if(dy > 0.f) {
			angle = PI + (PI - angle);
		}
		index = size_t(angle / (2.f * PI) * float(resolution));
	}
	for(auto const& e : distribution) {
		if(index < size_t(e.slices)) {
			populate_tooltip(state, e.key, e.value, contents);
			return;
		}
		index -= size_t(e.slices);
	}
}

template<class T>
void piechart<T>::populate_tooltip(sys::state& state, T t, float percentage, text::columnar_layout& contents) noexcept {
		auto fat_t = dcon::fatten(state.world, t);
		auto box = text::open_layout_box(contents, 0);
		if constexpr(!std::is_same_v<dcon::nation_id, T>)
			text::add_to_layout_box(state, contents, box, fat_t.get_name(), text::substitution_map{});
		else
			text::add_to_layout_box(state, contents, box, text::get_name(state, t), text::substitution_map{});
		text::add_to_layout_box(state, contents, box, std::string(":"), text::text_color::white);
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::format_percentage(percentage, 1), text::text_color::white);
		text::close_layout_box(contents, box);
}


template<class RowWinT, class RowConT>
void listbox_element_base<RowWinT, RowConT>::update(sys::state& state) {
	auto content_off_screen = int32_t(row_contents.size() - row_windows.size());
	int32_t scroll_pos = list_scrollbar->raw_value();
	if(content_off_screen <= 0) {
		list_scrollbar->set_visible(state, false);
		scroll_pos = 0;
	} else {
		list_scrollbar->change_settings(state, mutable_scrollbar_settings{ 0, content_off_screen, 0, 0, false });
		list_scrollbar->set_visible(state, true);
		scroll_pos = std::min(scroll_pos, content_off_screen);
	}

	if(is_reversed()) {
		auto i = int32_t(row_contents.size()) - scroll_pos - 1;
		for(int32_t rw_i = int32_t(row_windows.size()) - 1; rw_i >= 0; rw_i--) {
			RowWinT* row_window = row_windows[size_t(rw_i)];
			if(i >= 0) {
				row_window->set_visible(state, true);
				auto prior_content = retrieve<RowConT>(state, row_window);
				auto new_content = row_contents[i--];

				if(prior_content != new_content) {
					send(state, row_window, wrapped_listbox_row_content<RowConT>{ new_content });
					if(!row_window->is_visible()) {
						row_window->set_visible(state, true);
					} else {
						row_window->impl_on_update(state);
					}
				} else {
					row_window->set_visible(state, true);
				}
			} else {
				row_window->set_visible(state, false);
			}
		}
	} else {
		auto i = size_t(scroll_pos);
		for(RowWinT* row_window : row_windows) {
			if(i < row_contents.size()) {
				auto prior_content = retrieve<RowConT>(state, row_window);
				auto new_content = row_contents[i++];

				if(prior_content != new_content) {
					send(state, row_window, wrapped_listbox_row_content<RowConT>{ new_content });
					if(!row_window->is_visible()) {
						row_window->set_visible(state, true);
					} else {
						row_window->impl_on_update(state);
					}
				} else {
					row_window->set_visible(state, true);
				}
			} else {
				row_window->set_visible(state, false);
			}
		}
	}
}

template<class RowWinT, class RowConT>
message_result listbox_element_base<RowWinT, RowConT>::on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept {
	if(row_contents.size() > row_windows.size()) {
		amount = is_reversed() ? -amount : amount;
		list_scrollbar->update_raw_value(state, list_scrollbar->raw_value() + (amount < 0 ? 1 : -1));
		state.ui_state.last_tooltip = nullptr; //force update of tooltip
		update(state);
		return message_result::consumed;
	}
	return message_result::unseen;
}

template<class RowWinT, class RowConT>
void listbox_element_base<RowWinT, RowConT>::scroll_to_bottom(sys::state& state) {
	uint32_t list_size = 0;
	for(auto rc : row_contents) {
		list_size++;
	}
	list_scrollbar->update_raw_value(state, list_size);
	state.ui_state.last_tooltip = nullptr; //force update of tooltip
	update(state);
}


template<class RowWinT, class RowConT>
void listbox_element_base<RowWinT, RowConT>::on_create(sys::state& state) noexcept {
	int16_t current_y = 0;
	int16_t subwindow_y_size = 0;
	while(current_y + subwindow_y_size <= base_data.size.y) {
		auto ptr = make_element_by_type<RowWinT>(state, get_row_element_name());
		row_windows.push_back(static_cast<RowWinT*>(ptr.get()));
		int16_t offset = ptr->base_data.position.y;
		ptr->base_data.position.y += current_y;
		subwindow_y_size = ptr->base_data.size.y;
		current_y += ptr->base_data.size.y + offset;
		add_child_to_front(std::move(ptr));
	}
	auto ptr = make_element_by_type<standard_listbox_scrollbar<RowWinT, RowConT>>(state, "standardlistbox_slider");
	list_scrollbar = static_cast<standard_listbox_scrollbar<RowWinT, RowConT>*>(ptr.get());
	add_child_to_front(std::move(ptr));
	list_scrollbar->scale_to_parent();

	update(state);
}

template<class RowWinT, class RowConT>
void listbox_element_base<RowWinT, RowConT>::render(sys::state& state, int32_t x, int32_t y) noexcept {
	dcon::gfx_object_id gid = base_data.data.list_box.background_image;
	if(gid) {
		auto const& gfx_def = state.ui_defs.gfx[gid];
		if(gfx_def.primary_texture_handle) {
			if(gfx_def.get_object_type() == ui::object_type::bordered_rect) {
				ogl::render_bordered_rect(state, get_color_modification(false, false, true), gfx_def.type_dependent, float(x), float(y),
					float(base_data.size.x), float(base_data.size.y),
					ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
					base_data.get_rotation(), gfx_def.is_vertically_flipped(),
					state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
			} else {
				ogl::render_textured_rect(state, get_color_modification(false, false, true), float(x), float(y), float(base_data.size.x),
					float(base_data.size.y),
					ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
					base_data.get_rotation(), gfx_def.is_vertically_flipped(),
					state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
			}
		}
	}
	container_base::render(state, x, y);
}


template<class RowConT>
message_result listbox_row_element_base<RowConT>::get(sys::state& state, Cyto::Any& payload) noexcept {
	if(payload.holds_type<RowConT>()) {
		payload.emplace<RowConT>(content);
		return message_result::consumed;
	} else if(payload.holds_type<wrapped_listbox_row_content<RowConT>>()) {
		content = any_cast<wrapped_listbox_row_content<RowConT>>(payload).content;
		impl_on_update(state);
		return message_result::consumed;
	}
	return message_result::unseen;
}

template<class RowWinT, class RowConT>
void standard_listbox_scrollbar<RowWinT, RowConT>::on_value_change(sys::state& state, int32_t v) noexcept {
	static_cast<listbox_element_base<RowWinT, RowConT>*>(parent)->update(state);
}

} // namespace ui
