#pragma once

#include <variant>
#include "gui_element_types.hpp"

namespace ui {

template<bool Left>
class event_lr_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		frame = Left ? 0 : 1;
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<bool>{Left};
			parent->impl_get(state, payload);
		}
	}
};

struct option_taken_notification {
	int a = 0;
};

using event_data_wrapper =  std::variant< event::pending_human_n_event, event::pending_human_f_n_event, event::pending_human_p_event, event::pending_human_f_p_event>;

class event_option_button : public button_element_base {
public:
	bool visible = true;
	uint8_t index = 0;
	void on_update(sys::state& state) noexcept override;
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void button_action(sys::state& state) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			button_element_base::render(state, x, y);
	}
};
class event_image : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override;
};
class event_desc_text : public scrollable_text {
public:
	void on_create(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
class event_name_text : public multiline_text_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		auto fh = base_data.data.text.font_handle;
		auto font_index = text::font_index_from_font_id(fh);
		auto font_size = text::size_from_font_id(fh);
		auto& font = state.font_collection.fonts[font_index - 1];
		auto text_height = int32_t(std::ceil(font.line_height(font_size)));
		base_data.size.y = int16_t((text_height - 15) * 2);
	}
	void on_update(sys::state& state) noexcept override;
};
class event_requirements_icon : public image_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
};
class event_odds_icon : public image_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
};

//
// National events
//
template<bool IsMajor>
class national_event_window : public window_element_base {
	element_base* option_buttons[sys::max_event_options];
	simple_text_element_base* count_text = nullptr;
	int32_t index = 0;

public:
	std::vector<event_data_wrapper> events;

	void on_create(sys::state& state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	void on_update(sys::state& state) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
};

//
// Provincial events
//
class provincial_event_window : public window_element_base {
	element_base* option_buttons[sys::max_event_options];
	simple_text_element_base* count_text = nullptr;
	image_element_base* divider_image = nullptr;
	int32_t index = 0;

public:
	std::vector<event_data_wrapper> events;

	void on_create(sys::state& state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	void on_update(sys::state& state) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
};

void populate_event_submap(sys::state& state, text::substitution_map& sub, std::variant<event::pending_human_n_event, event::pending_human_f_n_event, event::pending_human_p_event, event::pending_human_f_p_event> const& phe) noexcept;

} // namespace ui
