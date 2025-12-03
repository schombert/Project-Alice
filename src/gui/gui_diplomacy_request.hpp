#pragma once

#include "diplomatic_messages.hpp"

namespace ui {
class diplomatic_message_topbar_button : public button_element_base {
	std::string_view get_type_key(diplomatic_message::type_t type);
public:
	void on_update(sys::state& state) noexcept override;
	tooltip_behavior has_tooltip(sys::state& state) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void button_action(sys::state& state) noexcept override;
	// Right click must remove message
	void button_right_action(sys::state& state) noexcept override;
};

class diplomacy_request_window : public window_element_base {
	simple_text_element_base* count_text = nullptr;
	int32_t index = 0;

public:
	std::vector<diplomatic_message::message> messages;
	void on_create(sys::state& state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	void on_update(sys::state& state) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
};
}
