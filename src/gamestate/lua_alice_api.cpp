#include <string>
#include "lua_alice_api.hpp"
#include "system_state.hpp"
#include "lua.hpp"
#include "text.hpp"
#include "commands.hpp"

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
	command::move_army(*alice_state_ptr, alice_state_ptr->local_player_nation, dcon::army_id{ unit }, dcon::province_id{ target }, reset);
}
void command_move_navy(int32_t unit, int32_t target, bool reset) {
	command::move_navy(*alice_state_ptr, alice_state_ptr->local_player_nation, dcon::navy_id{ unit }, dcon::province_id{ target }, reset);
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
		OutputDebugStringA(error.c_str());
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
void set_state(sys::state* state_ptr) {
	alice_state_ptr = state_ptr;
}
}
