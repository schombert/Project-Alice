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

template<typename ... dcon_args>
void call_named_function(sys::state& state, const char function_name[], dcon_args ... id_params) {
	std::string name = function_name;
	auto found = state.lua_registered_functions.find(name);
	int index;

	if(found != state.lua_registered_functions.end()) {
		index = found->second;
	} else {
		auto stack_size_at_start = lua_gettop(state.lua_game_loop_environment);

		lua_getfield(state.lua_game_loop_environment, LUA_GLOBALSINDEX, "alice"); // [alice
		lua_getfield(state.lua_game_loop_environment, -1, function_name); // [alice, function
		lua_remove(state.lua_game_loop_environment, -2); // [function
		index = luaL_ref(state.lua_game_loop_environment, LUA_REGISTRYINDEX); // [

		assert(lua_gettop(state.lua_game_loop_environment) == stack_size_at_start);

		state.lua_registered_functions[name] = index;
	}

	lua_rawgeti(state.lua_game_loop_environment, LUA_REGISTRYINDEX, index);

	// "loop" over the varadic template arguments in loop. Taken from https://stackoverflow.com/questions/7230621/how-can-i-iterate-over-a-packed-variadic-template-argument-list
	([&] {

	
		lua_pushnumber(state.lua_game_loop_environment, id_params.index());

	} (), ...);
	auto pcall_result = lua_pcall(state.lua_game_loop_environment, 1, 0, 0);
	if(pcall_result) {
		state.lua_notification(lua_tostring(state.lua_game_loop_environment, -1));
		lua_settop(state.lua_game_loop_environment, 0);
	}
	assert(lua_gettop(state.lua_game_loop_environment) == 0);
}




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
