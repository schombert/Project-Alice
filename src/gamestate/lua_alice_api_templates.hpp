#pragma once

#include "system_state.hpp"

namespace lua_alice_api {
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
