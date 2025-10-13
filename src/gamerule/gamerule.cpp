#include "system_state.hpp"
#include "gamerule.hpp"



namespace gamerule {

dcon::gamerule_id create_hardcoded_gamerule(parsers::scenario_building_context& context, std::string_view name, const std::vector<std::string>& settings, uint8_t default_setting ) {
	auto game_rule = context.state.world.create_gamerule();
	context.state.world.gamerule_set_name(game_rule, text::find_or_add_key(context.state, name, false));
	context.state.world.gamerule_set_tooltip_explain(game_rule, text::find_or_add_key(context.state, std::string(name) + "_desc", false));
	for(size_t i = 0; i < settings.size(); i++) {
		context.map_of_gamerule_options.insert_or_assign(settings[i], parsers::scanned_gamerule_option{ game_rule, uint8_t(i) });
		auto& options = context.state.world.gamerule_get_options(game_rule);
		options[uint8_t(i)].name = text::find_or_add_key(context.state, settings[i], false);
	}
	context.state.world.gamerule_set_default_setting(game_rule, default_setting);
	context.state.world.gamerule_set_current_setting(game_rule, default_setting);
	context.state.world.gamerule_set_settings_count(game_rule, uint8_t(settings.size()));
	context.map_of_gamerules.insert_or_assign(std::string(name), game_rule);
	return game_rule;
}

void load_hardcoded_gamerules(parsers::scenario_building_context& context) {
	// create gamerule for spherelings declaring war on spherelord
	{
		std::vector<std::string> options = { "alice_gamerule_allow_sphereling_declare_war_on_spherelord_opt_no", "alice_gamerule_allow_sphereling_declare_war_on_spherelord_opt_yes" };
		context.state.hardcoded_gamerules.sphereling_can_declare_spherelord = create_hardcoded_gamerule(context, "alice_gamerule_allow_sphereling_declare_war_on_spherelord", options, uint8_t(context.state.defines.alice_can_goto_war_against_spherelord_default_setting));
	}
}

void restore_gamerule_ui_settings(sys::state& state) {
	state.ui_state.gamerule_ui_settings.clear();
	for(const auto& gamerule : state.world.in_gamerule) {
		state.ui_state.gamerule_ui_settings.insert_or_assign(gamerule.id, gamerule.get_current_setting());
	}
}
void set_gamerule(sys::state& state, dcon::gamerule_id gamerule, uint8_t new_setting) {
	if(check_gamerule(state, gamerule, new_setting)) {
		return;
	}
	auto old_setting = state.world.gamerule_get_current_setting(gamerule);
	auto& options = state.world.gamerule_get_options(gamerule);
	auto on_deselect_effect = options[old_setting].on_deselect;
	if(on_deselect_effect) {
		effect::execute(state, on_deselect_effect, 0, 0, 0, uint32_t(state.current_date.value), uint32_t(gamerule.index() << 4 ^ new_setting));
	}
	auto on_select_effect = options[new_setting].on_select;
	if(on_select_effect) {
		effect::execute(state, on_select_effect, 0, 0, 0, uint32_t(state.current_date.value), uint32_t(gamerule.index() << 4 ^ new_setting));
	}
	state.world.gamerule_set_current_setting(gamerule, new_setting);
	state.ui_state.gamerule_ui_settings.insert_or_assign(gamerule, new_setting);
	// if its being called from somewhere not in a command, set new_game to false
	state.network_state.is_new_game = false;
}



bool check_gamerule(sys::state& state, dcon::gamerule_id gamerule, uint8_t setting) {
	return state.world.gamerule_get_current_setting(gamerule) == setting;
}

}
