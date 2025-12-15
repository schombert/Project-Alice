#pragma once

#include "gui_element_types.hpp"
#include "system_state_forward.hpp"

namespace ui {
void open_chat_before_game(sys::state& state);
void open_chat_during_game(sys::state& state);
class player_name_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override;
};

class player_kick_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override;
	void button_action(sys::state& state) noexcept override;
	tooltip_behavior has_tooltip(sys::state& state) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
};

class player_ban_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override;
	void button_action(sys::state& state) noexcept override;
	tooltip_behavior has_tooltip(sys::state& state) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
};

std::unique_ptr<element_base> make_chat_window(sys::state& state, std::string_view name);
std::unique_ptr<element_base> make_compact_chat_messages_list(sys::state& state, std::string_view name);
}
