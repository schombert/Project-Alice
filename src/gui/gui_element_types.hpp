#pragma once

#include "dcon_generated_ids.hpp"
#include "gui_graphics.hpp"
#include "gui_element_base.hpp"
#include "opengl_wrapper.hpp"
#include "sound.hpp"
#include "system_state_forward.hpp"
#include "text.hpp"
#include "texture.hpp"
#include <cstdint>
#include <vector>
#include "demographics.hpp"

namespace window {
struct text_services_object;
}

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
	virtual bool get_horizontal_flip(sys::state& state) noexcept;
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override;
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
};

class partially_transparent_image : public opaque_element_base {
	dcon::texture_id texture_id;
public:
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override;
	void on_create(sys::state& state) noexcept override;
	// MAYBE this function has to be changed when make_element_by_type() is changed
	static std::unique_ptr<partially_transparent_image> make_element_by_type_alias(sys::state& state, dcon::gui_def_id id);
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

	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	message_result on_lbutton_up(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods, bool under_mouse) noexcept override;
	message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept override;

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
	message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept final;
};

class tinted_right_click_button_element_base : public tinted_button_element_base {
public:
	virtual void button_right_action(sys::state& state) noexcept override { }
	message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept final;
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

class u16_text_element_base : public element_base {
protected:
	std::u16string cached_text;
	text::layout internal_layout;
public:
	bool black_text = true;

	void set_text(sys::state& state, std::u16string const& new_text);
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	void format_text(sys::state& state);

	std::u16string_view get_text(sys::state& state) const {
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


class edit_box_element_base : public u16_text_element_base {
protected:

	struct undo_item {
		std::u16string contents;
		int16_t anchor = 0;
		int16_t cursor = 0;
		bool valid = false;

		bool operator==(undo_item const& o) const noexcept {
			return contents == o.contents;
		}
		bool operator!=(undo_item const& o) const noexcept {
			return !(*this == o);
		}
	};

	struct undo_buffer {
		constexpr static int32_t total_size = 16;

		undo_item interal_buffer[total_size] = {};
		int32_t buffer_position = 0;

		std::optional<undo_item> undo(undo_item current_state) {
			push_state(current_state);

			auto temp_back = buffer_position - 1;
			if(temp_back < 0) {
				temp_back += total_size;
			}

			if(interal_buffer[temp_back].valid) {
				buffer_position = temp_back;
				return interal_buffer[temp_back];
			}
			return std::optional<undo_item>{};
		}
		std::optional<undo_item> redo(undo_item current_state) {
			if(interal_buffer[buffer_position] == current_state) {
				auto temp_back = buffer_position + 1;
				if(temp_back >= total_size) {
					temp_back -= total_size;
				}
				if(interal_buffer[temp_back].valid) {
					buffer_position = temp_back;
					return interal_buffer[buffer_position];
				}
			}
			return std::optional<undo_item>{};
		}
		void push_state(undo_item state) {
			if(interal_buffer[buffer_position].valid == false || interal_buffer[buffer_position] != state) {
				++buffer_position;
				if(buffer_position >= total_size) {
					buffer_position -= total_size;
				}
				interal_buffer[buffer_position] = state;
				interal_buffer[buffer_position].valid = true;

				auto temp_next = buffer_position + 1;
				if(temp_next >= total_size) {
					temp_next -= total_size;
				}
				interal_buffer[temp_next].valid = false;
			}
		}
	} edit_undo_buffer;

	text::layout_details glyph_details;
	std::chrono::time_point<std::chrono::steady_clock> activation_time;
	window::text_services_object* ts_obj = nullptr;

	int32_t cursor_position = 0;
	int32_t anchor_position = 0;
	int32_t mouse_entry_position = 0;
	int32_t temp_selection_start = 0;
	int32_t temp_selection_end = 0;
	std::chrono::steady_clock::time_point last_activated;

	bool multiline = false;
	bool changes_made = false;

	void insert_codepoint(sys::state& state, uint32_t codepoint, sys::key_modifiers mods);
	void internal_on_text_changed(sys::state& state);
	void internal_on_selection_changed(sys::state& state);
	void internal_move_cursor_to_point(sys::state& state, int32_t x, int32_t y, bool extend_selection);
	int32_t best_cursor_fit_on_line(int32_t line, int32_t xpos);
	int32_t visually_left_on_line(int32_t line);
	int32_t visually_right_on_line(int32_t line);
	ui::urect get_edit_bounds(sys::state& state) const;
public:
	int32_t template_id = -1;
	bool disabled = false;

	void set_text(sys::state& state, std::u16string const& new_text);

	virtual void edit_box_update(sys::state& state, std::u16string_view s) noexcept { }

	void on_reset_text(sys::state& state) noexcept override;
	void on_create(sys::state& state) noexcept override;

	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		return message_result::consumed;
	}
	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept override;
	void on_text(sys::state& state, char32_t ch) noexcept override;
	void on_edit_command(sys::state& state, edit_command command, sys::key_modifiers mods) noexcept override;
	bool edit_consume_mouse_event(sys::state& state, int32_t x, int32_t y, uint32_t buttons) noexcept override;
	message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	focus_result on_get_focus(sys::state& state) noexcept override;
	void on_lose_focus(sys::state& state) noexcept override;
	void set_cursor_visibility(sys::state& state, bool visible) noexcept override;
	void edit_move_cursor_to_screen_point(sys::state& state, int32_t x, int32_t y, bool extend_selection) noexcept override;
	sys::text_mouse_test_result detailed_text_mouse_test(sys::state& state, int32_t x, int32_t y) noexcept override;
	void set_temporary_selection(sys::state& state, int32_t start, int32_t end) noexcept override;
	void register_composition_result(sys::state& state) noexcept override;
	std::u16string_view text_content() noexcept override { return cached_text; }
	std::pair<int32_t, int32_t> text_selection() noexcept override;
	std::pair<int32_t, int32_t> temporary_text_range() noexcept override;
	void set_text_selection(sys::state& state, int32_t cursor, int32_t anchor) noexcept override;
	void insert_text(sys::state& state, int32_t position_start, int32_t position_end, std::u16string_view content, insertion_source source) noexcept override;
	bool position_is_ltr(int32_t position) noexcept override;
	ui::urect text_bounds(sys::state& state, int32_t position_start, int32_t position_end) noexcept override;
	void on_hover(sys::state& state) noexcept override;
	void on_hover_end(sys::state& state) noexcept override;

	~edit_box_element_base() override;
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
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override;
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

} // namespace ui
