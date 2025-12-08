#pragma once

#include "gui_element_base.hpp"

namespace sys {
struct state;
}

namespace lua_alice_api {

void setup_gameloop_environment(sys::state& state);
void setup_ui_environment(sys::state& state);
void set_state(sys::state* state);

bool has_named_function(sys::state& state, const char function_name[]);
void call_named_function(sys::state& state, const char function_name[]);
void call_named_function(sys::state& state, const char function_name[], dcon::province_id prov);
void call_named_function_safe(sys::state& state, const char function_name[], dcon::province_id prov);

}

namespace ui {

struct lua_scripted_element : public ui::element_base {
	// connection to lua
	int on_update_lref;
	std::string on_update_lname;
	std::string_view texture_key;
	dcon::text_key tooltip_key;

	text::layout internal_layout;
	text::text_color text_color = text::text_color::gold;
	float text_scale = 1.500000f;
	bool text_is_header = true;
	text::alignment text_alignment = text::alignment::right;
	std::string cached_text;
	void set_text(sys::state& state, std::string const& new_text);
	void on_reset_text(sys::state& state) noexcept override;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	ui::tooltip_behavior has_tooltip(sys::state& state) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
}
