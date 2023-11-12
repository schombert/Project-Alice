#pragma once

#include <variant>
#include "gui_element_types.hpp"

namespace ui {

struct option_taken_notification {
	int a = 0;
};

using event_data_wrapper =  std::variant< event::pending_human_n_event, event::pending_human_f_n_event, event::pending_human_p_event, event::pending_human_f_p_event>;

class event_option_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override;
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void button_action(sys::state& state) noexcept override;
};
class event_auto_button : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "event_auto");
	}
	void button_action(sys::state& state) noexcept override;
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
class national_event_window : public window_element_base {
public:
	static std::vector<std::unique_ptr<national_event_window>> event_pool;
	static int32_t pending_events;

	std::variant<std::monostate, event::pending_human_n_event, event::pending_human_f_n_event> event_data;

	void on_create(sys::state& state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
	void on_update(sys::state& state) noexcept override;
	static void new_event(sys::state& state, event::pending_human_n_event const& dat);
	static void new_event(sys::state& state, event::pending_human_f_n_event const& dat);
};

class national_major_event_window : public window_element_base {
public:
	static std::vector<std::unique_ptr<national_major_event_window>> event_pool;
	static int32_t pending_events;

	std::variant<std::monostate, event::pending_human_n_event, event::pending_human_f_n_event> event_data;

	void on_create(sys::state& state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
	void on_update(sys::state& state) noexcept override;
	static void new_event(sys::state& state, event::pending_human_n_event const& dat);
	static void new_event(sys::state& state, event::pending_human_f_n_event const& dat);
};

//
// Provincial events
//
class provincial_event_window : public window_element_base {
public:
	static std::vector<std::unique_ptr<provincial_event_window>> event_pool;
	static int32_t pending_events;

	std::variant<std::monostate, event::pending_human_p_event, event::pending_human_f_p_event> event_data;

	void on_create(sys::state& state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
	void on_update(sys::state& state) noexcept override;
	static void new_event(sys::state& state, event::pending_human_p_event const& dat);
	static void new_event(sys::state& state, event::pending_human_f_p_event const& dat);
};

void populate_event_submap(sys::state& state, text::substitution_map& sub, std::variant<event::pending_human_n_event, event::pending_human_f_n_event, event::pending_human_p_event, event::pending_human_f_p_event> const& phe) noexcept;

void close_expired_event_windows(sys::state& state);

} // namespace ui
