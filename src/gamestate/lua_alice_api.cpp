#include <string>
#include "lua_alice_api.hpp"
#include "system_state.hpp"
#include "lua.hpp"

sys::state* alice_state_ptr;

#include "lua_dcon_generated.cpp"

// how to add new lua ffi functions
// write their definition inside extern "C" somewhere in executable
// provide their definition
// add ffi declaration somewhere in lua folder of assets

extern "C" {
	DCON_LUADLL_API bool alice_at_war(int32_t nation_a, int32_t nation_b);
	DCON_LUADLL_API sys::unit_variable_stats* alice_get_unit_stats(int32_t unit_id);
	DCON_LUADLL_API void call_daily(const char function_name[]);
	DCON_LUADLL_API void remove_daily(const char function_name[]);
}

bool alice_at_war(int32_t nation_a, int32_t nation_b) {
	return military::are_at_war(
		*alice_state_ptr,
		dcon::nation_id{ dcon::nation_id::value_base_t(nation_a) },
		dcon::nation_id{ dcon::nation_id::value_base_t(nation_b) }
	);
}

sys::unit_variable_stats* alice_get_unit_stats(int32_t unit_id) {
	dcon::unit_type_id uid = dcon::unit_type_id{ dcon::unit_type_id::value_base_t(unit_id) };
	return &(alice_state_ptr->military_definitions.unit_base_definitions[uid]);
}

void call_daily(const char function_name[]){
	std::string name = function_name;
	auto found = alice_state_ptr->lua_registered_functions.find(name);
	int index;

	if(found != alice_state_ptr->lua_registered_functions.end()) {
		index = found->second;
	} else {
		auto stack_size_at_start = lua_gettop(alice_state_ptr->lua_game_loop_environment);

		lua_getfield(alice_state_ptr->lua_game_loop_environment, LUA_GLOBALSINDEX, "alice"); // [alice
		lua_getfield(alice_state_ptr->lua_game_loop_environment, -1, function_name); // [alice, function
		lua_remove(alice_state_ptr->lua_game_loop_environment, -2); // [function
		index = luaL_ref(alice_state_ptr->lua_game_loop_environment, LUA_REGISTRYINDEX); // [
	
		assert(lua_gettop(alice_state_ptr->lua_game_loop_environment) == stack_size_at_start);

		alice_state_ptr->lua_registered_functions[name] = index;
	}

	alice_state_ptr->lua_on_daily_tick.push_back(index);
}

void remove_daily(const char function_name[]) {
	std::string name = function_name;
	auto found = alice_state_ptr->lua_registered_functions.find(name);
	if(found == alice_state_ptr->lua_registered_functions.end()) {
		return;
	}
	auto place = std::find(alice_state_ptr->lua_on_daily_tick.begin(), alice_state_ptr->lua_on_daily_tick.end(), found->second);
	if(place == alice_state_ptr->lua_on_daily_tick.end()) {
		return;
	}
	alice_state_ptr->lua_on_daily_tick.erase(place);
}

namespace lua_alice_api {
void set_state(sys::state* state_ptr) {
	alice_state_ptr = state_ptr;
}
}
