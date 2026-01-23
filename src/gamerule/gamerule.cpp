#include "system_state.hpp"
#include "gamerule.hpp"
#include "parsers_declarations.hpp"
#include "lua_alice_api.hpp"
#include "lua_alice_api_templates.hpp"



namespace gamerule {

dcon::gamerule_id create_hardcoded_gamerule(parsers::scenario_building_context& context, std::string_view name, const std::vector<sys::gamerule_option>& settings, uint8_t default_setting, parsers::error_handler& err) {
	auto game_rule = context.state.world.create_gamerule();
	context.state.world.gamerule_set_name(game_rule, text::find_or_add_key(context.state, name, false));
	context.state.world.gamerule_set_tooltip_explain(game_rule, text::find_or_add_key(context.state, std::string(name) + "_desc", false));
	for(size_t i = 0; i < settings.size(); i++) {
		std::string option_name = text::produce_simple_string(context.state, settings[i].name);
		context.map_of_gamerule_options.insert_or_assign(option_name, parsers::scanned_gamerule_option{ game_rule, uint8_t(i) });
		auto& options = context.state.world.gamerule_get_options(game_rule);
		std::string select_lua_fun_name = text::produce_simple_string(context.state, settings[i].on_select_lua_function);
		std::string deselect_lua_fun_name = text::produce_simple_string(context.state, settings[i].on_deselect_lua_function);
		options[uint8_t(i)] = settings[i];
		// Check that the lua functions exist, otherwise remove them
		if(!lua_alice_api::has_named_function(context.state, select_lua_fun_name.c_str())){
			options[i].on_select_lua_function = dcon::text_key{ };
			err.accumulated_errors += "Lua function " + select_lua_fun_name + " for hardecoded gamerule " + std::string(name) + " is missing\n";
		}
		if(!lua_alice_api::has_named_function(context.state, deselect_lua_fun_name.c_str())) {
			options[i].on_deselect_lua_function = dcon::text_key{ };
			err.accumulated_errors += "Lua function " + deselect_lua_fun_name + " for hardecoded gamerule " + std::string(name) + " is missing\n";
		}
	}
	context.state.world.gamerule_set_default_setting(game_rule, default_setting);
	context.state.world.gamerule_set_current_setting(game_rule, default_setting);
	context.state.world.gamerule_set_settings_count(game_rule, uint8_t(settings.size()));
	context.map_of_gamerules.insert_or_assign(std::string(name), game_rule);
	return game_rule;
}

void load_hardcoded_gamerules(parsers::scenario_building_context& context, parsers::error_handler& err) {
	// create gamerule for spherelings declaring war on spherelord
	{
		std::string base_name = "alice_gamerule_allow_sphereling_declare_war_on_spherelord";
		std::vector<sys::gamerule_option> options  =
			{ {text::find_or_add_key(context.state, base_name + "_opt_no", false), text::find_or_add_key(context.state, base_name + "_opt_no_on_select", false), text::find_or_add_key(context.state, base_name + "_opt_no_on_deselect", false) },
			{ text::find_or_add_key(context.state, base_name + "_opt_yes", false), text::find_or_add_key(context.state, base_name + "_opt_yes_on_select", false), text::find_or_add_key(context.state, base_name + "_opt_yes_on_deselect", false) },
		};
		context.state.hardcoded_gamerules.sphereling_can_declare_spherelord = create_hardcoded_gamerule(context, base_name, options, uint8_t(context.state.defines.alice_can_goto_war_against_spherelord_default_setting), err);
	}
	{
		std::string base_name = "alice_gamerule_allow_partial_retreat";
		std::vector<sys::gamerule_option> options =
			{ {text::find_or_add_key(context.state, base_name + "_opt_disabled", false), text::find_or_add_key(context.state, base_name + "_opt_disabled_on_select", false), text::find_or_add_key(context.state, base_name + "_opt_disabled_on_deselect", false) },
			{ text::find_or_add_key(context.state, base_name + "_opt_enabled", false), text::find_or_add_key(context.state, base_name + "_opt_enabled_on_select", false), text::find_or_add_key(context.state, base_name + "_opt_enabled_on_deselect", false) },
		};
		context.state.hardcoded_gamerules.allow_partial_retreat = create_hardcoded_gamerule(context, base_name, options, uint8_t(context.state.defines.alice_allow_partial_retreat_default_setting), err);
	}
	{
		std::string base_name = "alice_gamerule_fog_of_war";
		std::vector<sys::gamerule_option> options =
			{ {text::find_or_add_key(context.state, base_name + "_opt_disabled", false), text::find_or_add_key(context.state, base_name + "_opt_disabled_on_select", false), text::find_or_add_key(context.state, base_name + "_opt_disabled_on_deselect", false) },
			{ text::find_or_add_key(context.state, base_name + "_opt_enabled", false), text::find_or_add_key(context.state, base_name + "_opt_enabled_on_select", false), text::find_or_add_key(context.state, base_name + "_opt_enabled_on_deselect", false) },
			{ text::find_or_add_key(context.state, base_name + "_opt_disabled_for_observer", false), text::find_or_add_key(context.state, base_name + "_opt_disabled_for_observer_on_select", false), text::find_or_add_key(context.state, base_name + "_opt_disabled_for_observer_on_deselect", false) },
		};
		context.state.hardcoded_gamerules.fog_of_war = create_hardcoded_gamerule(context, base_name, options, uint8_t(context.state.defines.alice_fog_of_war_default_setting), err);
	}
	{
		std::string base_name = "alice_gamerule_auto_concession_peace";
		std::vector<sys::gamerule_option> options =
			{ {text::find_or_add_key(context.state, base_name + "_opt_cannot_reject", false), text::find_or_add_key(context.state, base_name + "_opt_cannot_reject_on_select", false), text::find_or_add_key(context.state, base_name + "_opt_cannot_reject_on_deselect", false) },
			{ text::find_or_add_key(context.state, base_name + "_opt_can_reject", false), text::find_or_add_key(context.state, base_name + "_opt_can_reject_on_select", false), text::find_or_add_key(context.state, base_name + "_opt_can_reject_on_deselect", false) },
		};
		context.state.hardcoded_gamerules.auto_concession_peace = create_hardcoded_gamerule(context, base_name, options, uint8_t(context.state.defines.alice_auto_concession_peace_default_setting), err);
	}
	{
		std::string base_name = "alice_gamerule_command_units";
		std::vector<sys::gamerule_option> options =
			{ {text::find_or_add_key(context.state,  base_name + "_opt_disabled", false), text::find_or_add_key(context.state, base_name + "_opt_disabled_on_select", false), text::find_or_add_key(context.state, base_name + "_opt_disabled_on_deselect", false) },
			{ text::find_or_add_key(context.state, base_name + "_opt_enabled", false), text::find_or_add_key(context.state, base_name + "_opt_enabled_on_select", false), text::find_or_add_key(context.state, base_name + "_opt_enabled_on_deselect", false) },
		};
		context.state.hardcoded_gamerules.command_units = create_hardcoded_gamerule(context, base_name, options, uint8_t(context.state.defines.alice_command_units_default_setting), err);
	}
}

void restore_gamerule_ui_settings(sys::state& state) {
	state.ui_state.gamerule_ui_settings.clear();
	for(auto gamerule : state.world.in_gamerule) {
		state.ui_state.gamerule_ui_settings.insert_or_assign(gamerule.id, gamerule.get_current_setting());
	}
}

void set_gamerule_no_lua_exec(sys::state& state, dcon::gamerule_id gamerule, uint8_t new_setting) {
	state.world.gamerule_set_current_setting(gamerule, new_setting);
	state.ui_state.gamerule_ui_settings.insert_or_assign(gamerule, new_setting);
	// if its being called from somewhere not in a command, set new_game to false
	state.network_state.is_new_game = false;
}

void set_gamerule(sys::state& state, dcon::gamerule_id gamerule, uint8_t new_setting) {
	if(check_gamerule(state, gamerule, new_setting)) {
		return;
	}
	auto old_setting = state.world.gamerule_get_current_setting(gamerule);
	set_gamerule_no_lua_exec(state, gamerule, new_setting);

	auto& options = state.world.gamerule_get_options(gamerule);
	auto on_deselect_lua_fun = options[old_setting].on_deselect_lua_function;
	if(on_deselect_lua_fun) {
		lua_alice_api::call_named_function(state, text::produce_simple_string(state, on_deselect_lua_fun).c_str(), gamerule);
	}
	auto on_select_lua_fun = options[new_setting].on_select_lua_function;
	if(on_select_lua_fun) {
		auto func_name = text::produce_simple_string(state, on_select_lua_fun);
		lua_alice_api::call_named_function(state, func_name.c_str(), gamerule);
	}

}


dcon::gamerule_id get_gamerule_id_by_name(const sys::state& state, std::string_view gamerule_name) {
	auto iterator = state.gamerules_map.find(std::string(gamerule_name));
	if(iterator == state.gamerules_map.end()) {
		return dcon::gamerule_id{ };
	} else {
		return iterator->second;
	}
}

uint8_t get_gamerule_option_id_by_name(const sys::state& state, std::string_view gamerule_option_name) {
	auto iterator = state.gamerule_options_map.find(std::string(gamerule_option_name));
	if(iterator == state.gamerule_options_map.end()) {
		return 0;
	} else {
		return int32_t(iterator->second);
	}
}

uint8_t get_active_gamerule_option(const sys::state& state, dcon::gamerule_id gamerule) {
	assert(gamerule);
	return state.world.gamerule_get_current_setting(gamerule);
}



bool check_gamerule(sys::state& state, dcon::gamerule_id gamerule, uint8_t setting) {
	return state.world.gamerule_get_current_setting(gamerule) == setting;
}

}
