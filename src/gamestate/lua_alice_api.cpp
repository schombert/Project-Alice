#include <string>
#include "lua_alice_api.hpp"
#include "system_state.hpp"
#include "lua.hpp"
#include "text.hpp"
#include "commands.hpp"

static sys::state* alice_state_ptr;

#include "lua_dcon_generated.cpp"

// how to add new lua ffi functions (calls from LUA to C++)
// write their definition inside extern "C" somewhere in executable
// provide their definition
// add ffi declaration somewhere in lua folder of assets

extern "C" {
	DCON_LUADLL_API bool alice_at_war(int32_t nation_a, int32_t nation_b);
	DCON_LUADLL_API sys::unit_variable_stats* alice_get_unit_stats(int32_t unit_id);
	DCON_LUADLL_API void call_daily(const char function_name[]);
	DCON_LUADLL_API void remove_daily(const char function_name[]);
	DCON_LUADLL_API void call_daily_battle(const char function_name[]);
	DCON_LUADLL_API void remove_daily_battle(const char function_name[]);
	DCON_LUADLL_API void call_battle_end(const char function_name[]);
	DCON_LUADLL_API void remove_battle_end(const char function_name[]);

	DCON_LUADLL_API void set_text(const char text[]);

	DCON_LUADLL_API int32_t local_player_nation();

	DCON_LUADLL_API void command_move_army(int32_t unit, int32_t target, bool reset);
	DCON_LUADLL_API void command_move_navy(int32_t unit, int32_t target, bool reset);
}

int32_t local_player_nation() {
	return alice_state_ptr->local_player_nation.index();
}

void command_move_army(int32_t unit, int32_t target, bool reset) {
	command::move_army(*alice_state_ptr, alice_state_ptr->local_player_nation, dcon::army_id{ uint16_t(unit) }, dcon::province_id{ uint16_t(target) }, reset);
}
void command_move_navy(int32_t unit, int32_t target, bool reset) {
	command::move_navy(*alice_state_ptr, alice_state_ptr->local_player_nation, dcon::navy_id{ uint16_t(unit) }, dcon::province_id{ uint16_t(target) }, reset);
}

namespace ui {
void lua_scripted_element::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, alice_ui::state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void lua_scripted_element::on_reset_text(sys::state& state) noexcept  {
}
void lua_scripted_element::on_create(sys::state& state) noexcept  {
	auto found = alice_state_ptr->lua_registered_ui_functions.find(on_update_lname);
	if(found != alice_state_ptr->lua_registered_ui_functions.end()) {
		on_update_lref = found->second;
	} else {
		auto stack_size_at_start = lua_gettop(alice_state_ptr->lua_ui_environment);
		lua_getfield(alice_state_ptr->lua_ui_environment, LUA_GLOBALSINDEX, "alice"); // [alice
		lua_getfield(alice_state_ptr->lua_ui_environment, -1, on_update_lname.c_str()); // [alice, function
		lua_remove(alice_state_ptr->lua_ui_environment, -2); // [function
		on_update_lref = luaL_ref(alice_state_ptr->lua_ui_environment, LUA_REGISTRYINDEX); // [
		assert(lua_gettop(alice_state_ptr->lua_ui_environment) == stack_size_at_start);
		alice_state_ptr->lua_registered_ui_functions[on_update_lname] = on_update_lref;
	}
}
void lua_scripted_element::render(sys::state& state, int32_t x, int32_t y)  noexcept  {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 16);
	auto linesz = state.font_collection.line_height(state, fh);
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false);
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)), fh, ui::get_text_color(state, text_color), cmod);
	}
}
ui::tooltip_behavior lua_scripted_element::has_tooltip(sys::state& state) noexcept  {
	return ui::tooltip_behavior::no_tooltip;
}
ui::message_result lua_scripted_element::test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept  {
	return ui::message_result::consumed;
}
void lua_scripted_element::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept  {

}
void lua_scripted_element::on_update(sys::state& state) noexcept  {
	state.current_lua_element = this;
	auto start = lua_gettop(alice_state_ptr->lua_ui_environment);
	lua_rawgeti(alice_state_ptr->lua_ui_environment, LUA_REGISTRYINDEX, on_update_lref);
	auto result = lua_pcall(alice_state_ptr->lua_ui_environment, 0, 0, 0);
	if(result) {
		std::string error = lua_tostring(alice_state_ptr->lua_ui_environment, -1);
		set_text(*alice_state_ptr, error);
#ifdef _WIN32
		OutputDebugStringA(error.c_str());
#endif
		lua_settop(alice_state_ptr->lua_ui_environment, 0);
	}
	assert(lua_gettop(alice_state_ptr->lua_ui_environment) == start);
}
}

void set_text(const char text[]) {
	alice_state_ptr->current_lua_element->set_text(*alice_state_ptr, text);
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

void add_on_action(const char function_name[], std::vector<int>& container) {
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

	container.push_back(index);
}

void remove_on_action(const char function_name[], std::vector<int>& container) {
	std::string name = function_name;
	auto found = alice_state_ptr->lua_registered_functions.find(name);
	if(found == alice_state_ptr->lua_registered_functions.end()) {
		return;
	}
	auto place = std::find(alice_state_ptr->lua_on_daily_tick.begin(), alice_state_ptr->lua_on_daily_tick.end(), found->second);
	if(place == alice_state_ptr->lua_on_daily_tick.end()) {
		return;
	}
	container.erase(place);
}

void call_daily(const char function_name[]){
	add_on_action(function_name, alice_state_ptr->lua_on_daily_tick);
}

void remove_daily(const char function_name[]) {
	remove_on_action(function_name, alice_state_ptr->lua_on_daily_tick);
}
void call_daily_battle(const char function_name[]) {
	add_on_action(function_name, alice_state_ptr->lua_on_battle_tick);
}
void remove_daily_battle(const char function_name[]) {
	remove_on_action(function_name, alice_state_ptr->lua_on_battle_tick);
}
void call_battle_end(const char function_name[]) {
	add_on_action(function_name, alice_state_ptr->lua_on_battle_end);
}
void remove_battle_end(const char function_name[]) {
	remove_on_action(function_name, alice_state_ptr->lua_on_battle_end);
}


namespace lua_alice_api {

void setup_gameloop_environment(sys::state& state) {
	// Setup LUA game loop environment

	state.lua_game_loop_environment = luaL_newstate();
	luaL_openlibs(state.lua_game_loop_environment); // Load precalculated LUA environment into the LUA state

	{
		lua_newtable(state.lua_game_loop_environment);
		lua_setglobal(state.lua_game_loop_environment, "alice");
		assert(lua_gettop(state.lua_game_loop_environment) == 0);
	}
}

// example of providing LUA API if someone would ever need it for something
static int draw_rectangle(lua_State* L) {
	// get amount of arguments
	int n = lua_gettop(L);

	// validation
	if(n != 4) {
		lua_pushstring(L, "incorrect count of arguments");
		lua_error(L);
	}
	for(int i = 1; i <= n; i++) {
		if(!lua_isnumber(L, i)) {
			lua_pushstring(L, "incorrect argument");
			lua_error(L);
		}
	}


	auto x = lua_tonumber(L, 1);
	auto y = lua_tonumber(L, 2);
	auto width = lua_tonumber(L, 3);
	auto height = lua_tonumber(L, 4);

	lua_getfield(L, LUA_GLOBALSINDEX, "alice_state");
	sys::state* alice_state = (sys::state*)(lua_touserdata(L, -1));

	ogl::render_simple_rect(*alice_state, (float)x, (float)y, (float)width, (float)height, ui::rotation::upright, false, false);

	// return number of results
	return 0;
}

void setup_ui_environment(sys::state& state) {
	// LOAD LUA UI Environment
	state.lua_ui_environment = luaL_newstate();
	luaL_openlibs(state.lua_ui_environment); // Load precalculated LUA environment into the LUA state

	// pointer to alice state
	lua_pushlightuserdata(state.lua_ui_environment, (void*)(&state));
	lua_setfield(state.lua_ui_environment, LUA_GLOBALSINDEX, "alice_state");

	// alice table
	{
		lua_newtable(state.lua_ui_environment);
		lua_setglobal(state.lua_ui_environment, "alice");
		assert(lua_gettop(state.lua_ui_environment) == 0);
	}

	// Expose graphics functions to LUA

	// graphics subsystem
	lua_getfield(state.lua_ui_environment, LUA_GLOBALSINDEX, "alice"); // [alice
	lua_newtable(state.lua_ui_environment); // [alice, table
	lua_setfield(state.lua_ui_environment, -2, "graphics"); // [alice
	lua_remove(state.lua_ui_environment, -1); // [

	assert(lua_gettop(state.lua_ui_environment) == 0);

	// rectangle
	lua_getfield(state.lua_ui_environment, LUA_GLOBALSINDEX, "alice"); // [alice
	lua_getfield(state.lua_ui_environment, -1, "graphics"); // [alice, graphics
	lua_remove(state.lua_ui_environment, -2); // [graphics
	lua_pushcfunction(state.lua_ui_environment, draw_rectangle); // [graphics, draw_rectangle
	lua_setfield(state.lua_ui_environment, -2, "rect"); // [graphics,
	lua_remove(state.lua_ui_environment, -1); // [

	assert(lua_gettop(state.lua_ui_environment) == 0);
}

void set_state(sys::state* state_ptr) {
	alice_state_ptr = state_ptr;
}

bool has_named_function(sys::state& state, const char function_name[]) {
	std::string name = function_name;
	auto found = state.lua_registered_functions.find(name);
	int index;

	if(found != state.lua_registered_functions.end()) {
		return true;
	} else {
		auto stack_size_at_start = lua_gettop(state.lua_game_loop_environment);

		lua_getfield(state.lua_game_loop_environment, LUA_GLOBALSINDEX, "alice"); // [alice
		lua_getfield(state.lua_game_loop_environment, -1, function_name); // [alice, function

		if(lua_isfunction(state.lua_game_loop_environment, -1)) {
			lua_remove(state.lua_game_loop_environment, -2); // [function
			index = luaL_ref(state.lua_game_loop_environment, LUA_REGISTRYINDEX); // [
			state.lua_registered_functions[name] = index;
			assert(lua_gettop(state.lua_game_loop_environment) == stack_size_at_start);

			return true;
		} else {
			lua_pop(state.lua_game_loop_environment, 2);
			assert(lua_gettop(state.lua_game_loop_environment) == stack_size_at_start);

			return false;
		}
	}
}

void call_named_function(sys::state& state, const char function_name[]) {
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
	//lua_pushnumber(state.lua_game_loop_environment, b.index());
	//lua_pushboolean(state.lua_game_loop_environment, result != battle_result::indecisive);
	//lua_pushboolean(state.lua_game_loop_environment, result == battle_result::attacker_won);
	auto pcall_result = lua_pcall(state.lua_game_loop_environment, 0, 0, 0);
	if(pcall_result) {
		state.lua_notification(lua_tostring(state.lua_game_loop_environment, -1));
		lua_settop(state.lua_game_loop_environment, 0);
	}
	assert(lua_gettop(state.lua_game_loop_environment) == 0);
}

void call_named_function(sys::state& state, const char function_name[], dcon::province_id prov) {
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
	lua_pushnumber(state.lua_game_loop_environment, prov.index());
	auto pcall_result = lua_pcall(state.lua_game_loop_environment, 1, 0, 0);
	if(pcall_result) {
		state.lua_notification(lua_tostring(state.lua_game_loop_environment, -1));
		lua_settop(state.lua_game_loop_environment, 0);
	}
	assert(lua_gettop(state.lua_game_loop_environment) == 0);
}

void call_named_function_safe(sys::state& state, const char function_name[], dcon::province_id prov) {
	std::string name = function_name;
	auto found = state.lua_registered_functions.find(name);
	int index = LUA_REFNIL;

	if(found != state.lua_registered_functions.end()) {
		index = found->second;
	} else {
		auto stack_size_at_start = lua_gettop(state.lua_game_loop_environment);

		lua_getfield(state.lua_game_loop_environment, LUA_GLOBALSINDEX, "alice"); // [alice
		lua_getfield(state.lua_game_loop_environment, -1, function_name); // [alice, function

		if(lua_isfunction(state.lua_game_loop_environment, -1)) {
			lua_remove(state.lua_game_loop_environment, -2); // [function
			index = luaL_ref(state.lua_game_loop_environment, LUA_REGISTRYINDEX); // [
		} else {
			lua_pop(state.lua_game_loop_environment, 2);
		}

		assert(lua_gettop(state.lua_game_loop_environment) == stack_size_at_start);

		state.lua_registered_functions[name] = index;
	}

	if(index != LUA_REFNIL) {
		lua_rawgeti(state.lua_game_loop_environment, LUA_REGISTRYINDEX, index);
		lua_pushnumber(state.lua_game_loop_environment, prov.index());
		auto pcall_result = lua_pcall(state.lua_game_loop_environment, 1, 0, 0);
		if(pcall_result) {
			state.lua_notification(lua_tostring(state.lua_game_loop_environment, -1));
			lua_settop(state.lua_game_loop_environment, 0);
		}
	}
	assert(lua_gettop(state.lua_game_loop_environment) == 0);
}

}
