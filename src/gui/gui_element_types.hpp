#pragma once

#include "dcon_generated.hpp"
#include "gui_graphics.hpp"
#include "gui_element_base.hpp"
#include "sound.hpp"
#include "text.hpp"
#include "texture.hpp"
#include <functional>
#include <unordered_map>
#include <variant>
#include <vector>

namespace ui {

template<typename T>
std::unique_ptr<element_base> make_element_by_type(sys::state& state, std::string_view name) { // also bypasses global creation hooks
	auto it = state.ui_state.defs_by_name.find(name);
	if(it != state.ui_state.defs_by_name.end()) {
		auto res = std::make_unique<T>();
		std::memcpy(&(res->base_data), &(state.ui_defs.gui[it->second.definition]), sizeof(ui::element_data));
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
};

class tinted_image_element_base : public image_element_base {
private:
	uint32_t color = 0;

public:
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	void on_update(sys::state& state) noexcept override;
	virtual uint32_t get_tint_color(sys::state& state) noexcept {
		return 0;
	}
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
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::no_tooltip;
	}
};

class progress_bar : public opaque_element_base {
public:
	float progress = 0.f;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;

};

class button_element_base : public opaque_element_base {
private:
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
protected:
	std::string stored_text;
	float text_offset = 0.0f;
	bool black_text = true;
public:
	void set_text(sys::state& state, std::string const& new_text);
	void on_reset_text(sys::state& state) noexcept override;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;

	std::string_view get_text(sys::state& state) const {
		return stored_text;
	}
	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		return message_result::consumed;
	}
	message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		return message_result::consumed;
	}
};

class edit_box_element_base : public simple_text_element_base {
	int32_t edit_index = 0;
public:
	virtual void edit_box_enter(sys::state& state, std::string_view s) noexcept { }
	virtual void edit_box_update(sys::state& state, std::string_view s) noexcept { }
	virtual void edit_box_esc(sys::state& state) noexcept { }
	virtual void edit_box_backtick(sys::state& state) noexcept { }
	void on_reset_text(sys::state& state) noexcept override;
	void on_create(sys::state& state) noexcept override;

	message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
		return message_result::consumed;
	}
	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept override;
	void on_text(sys::state& state, char ch) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
};

class multiline_text_element_base : public element_base {
private:
	float line_height = 0.f;
	int32_t current_line = 0;
	int32_t visible_lines = 0;
	
public:
	bool black_text = true;
	text::layout internal_layout;

	void on_create(sys::state& state) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
		return message_result::consumed;
	}
};

class single_multiline_text_element_base : public multiline_text_element_base {
public:
	dcon::text_sequence_id text_id{};

	void on_update(sys::state& state) noexcept override {
		auto layout = text::create_endless_layout(
			internal_layout,
			text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), base_data.data.text.font_handle, 0, text::alignment::left, text::text_color::black }
		);
		auto box = text::open_layout_box(layout, 0);
		text::add_to_layout_box(layout, state, box, text_id);
		text::close_layout_box(layout, box);
	}
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

template<class ItemWinT, class ItemConT>
class overlapping_listbox_element_base : public window_element_base {
private:
	int16_t subwindow_width = 0;

protected:
	std::vector<ItemWinT*> windows{};

	virtual std::string_view get_row_element_name() {
		return std::string_view{};
	}
	virtual void update_subwindow(sys::state& state, ItemWinT* subwindow, ItemConT content) { }

public:
	std::vector<ItemConT> contents{};

	void update(sys::state& state);
};

class flag_button : public button_element_base {
protected:
	GLuint flag_texture_handle = 0;
	xy_pair flag_position{};
	xy_pair flag_size{};

public:
	virtual dcon::national_identity_id get_current_nation(sys::state& state) noexcept;
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
};

class overlapping_flags_box : public overlapping_listbox_element_base<overlapping_flags_flag_button, dcon::national_identity_id> {
protected:
	dcon::nation_id current_nation{};

	virtual void populate_flags(sys::state& state) { }

public:
	std::string_view get_row_element_name() override {
		return "flag_list_flag";
	}

	void update_subwindow(sys::state& state, overlapping_flags_flag_button* subwindow, dcon::national_identity_id content) override {
		subwindow->set_current_nation(state, content);
	}

	void on_update(sys::state& state) noexcept override;
	message_result set(sys::state& state, Cyto::Any& payload) noexcept override;
};

class overlapping_sphere_flags : public overlapping_flags_box {
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

class overlapping_protected_flags : public overlapping_flags_box {
protected:
	void populate_flags(sys::state& state) override;
};

class overlapping_truce_flags : public overlapping_flags_box {
protected:
	void populate_flags(sys::state& state) override;
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

template<class TabT>
class generic_opaque_checkbox_button : public checkbox_button {
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

    void on_create(sys::state& state) noexcept final {};

    TabT target = TabT();
};


class piechart_element_base : public element_base {
protected:
	static constexpr size_t resolution = 200;
	static constexpr size_t channels = 3;
	bool enabled = true;
	ogl::data_texture data_texture{resolution, channels};

public:
	float radius = 0.f;
	void generate_data_texture(sys::state& state, std::vector<uint8_t>& colors);
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
};

template<class T>
    class piechart : public piechart_element_base {
protected:
	virtual std::unordered_map<typename T::value_base_t, float> get_distribution(sys::state& state) noexcept {
		std::unordered_map<typename T::value_base_t, float> out{};
		out[static_cast<typename T::value_base_t>(-1)] = 1.f;
		return out;
	}

private:
	std::unordered_map<typename T::value_base_t, float> distribution{};
	std::vector<T> spread = std::vector<T>(resolution);

public:
	void on_create(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::position_sensitive_tooltip;
	}
	message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
		float dx = float(x) - radius;
		float dy = float(y) - radius;
		auto dist = sqrt(dx * dx + dy * dy);
		return dist <= radius ? message_result::consumed : message_result::unseen;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
};

template<class SrcT, class DemoT>
class demographic_piechart : public piechart<DemoT> {
protected:
	std::unordered_map<typename DemoT::value_base_t, float> get_distribution(sys::state& state) noexcept override;
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
