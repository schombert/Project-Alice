#pragma once

#include <variant>
#include "gui_element_types.hpp"

namespace ui {

template <bool Left>
class event_lr_button : public button_element_base {
public:
	void on_create(sys::state &state) noexcept override {
		button_element_base::on_create(state);
		frame = Left ? 0 : 1;
	}

	void button_action(sys::state &state) noexcept override {
		if (parent) {
			Cyto::Any payload = element_selection_wrapper<bool>{Left};
			parent->impl_get(state, payload);
		}
	}
};

struct option_taken_notification {
	int a = 0;
};

//
// National events
//
typedef std::variant<
    event::pending_human_n_event,
    event::pending_human_f_n_event>
    national_event_data_wrapper;
class national_event_option_button : public add_tooltip<button_element_base> {
public:
	uint8_t index = 0;
	void on_update(sys::state &state) noexcept override;
	void update_tooltip(sys::state &state, int32_t x, int32_t y, text::columnar_layout &contents) noexcept override;
	void button_action(sys::state &state) noexcept override;
};
class national_event_image : public image_element_base {
public:
	void on_update(sys::state &state) noexcept override;
};
class national_event_desc_text : public multiline_text_element_base {
public:
	void on_create(sys::state &state) noexcept override;
	void on_update(sys::state &state) noexcept override;
};
class national_event_name_text : public multiline_text_element_base {
public:
	void on_update(sys::state &state) noexcept override;
};
template <bool IsMajor>
class national_event_window : public window_element_base {
	element_base *option_buttons[sys::max_event_options];
	simple_text_element_base *count_text = nullptr;
	int32_t index = 0;

public:
	std::vector<national_event_data_wrapper> events;

	void on_create(sys::state &state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state &state, std::string_view name, dcon::gui_def_id id) noexcept override;
	void on_update(sys::state &state) noexcept override;
	message_result get(sys::state &state, Cyto::Any &payload) noexcept override;
};

//
// Provincial events
//
typedef std::variant<
    event::pending_human_p_event,
    event::pending_human_f_p_event>
    provincial_event_data_wrapper;
class provincial_event_option_button : public add_tooltip<button_element_base> {
public:
	uint8_t index = 0;
	void on_update(sys::state &state) noexcept override;
	void update_tooltip(sys::state &state, int32_t x, int32_t y, text::columnar_layout &contents) noexcept override;
	void button_action(sys::state &state) noexcept override;
};
class provincial_event_desc_text : public multiline_text_element_base {
public:
	void on_create(sys::state &state) noexcept override;
	void on_update(sys::state &state) noexcept override;
};
class provincial_event_name_text : public multiline_text_element_base {
public:
	void on_update(sys::state &state) noexcept override;
};
class provincial_event_window : public window_element_base {
	element_base *option_buttons[sys::max_event_options];
	simple_text_element_base *count_text = nullptr;
	int32_t index = 0;

public:
	std::vector<provincial_event_data_wrapper> events;

	void on_create(sys::state &state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state &state, std::string_view name, dcon::gui_def_id id) noexcept override;
	void on_update(sys::state &state) noexcept override;
	message_result get(sys::state &state, Cyto::Any &payload) noexcept override;
};

} // namespace ui
