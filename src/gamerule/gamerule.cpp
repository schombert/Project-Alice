#include "system_state.hpp"
#include "gamerule.hpp"



namespace gamerule {

void load_hardcoded_gamerules(sys::state& state) {
	// create gamerule for spherelings declaring war on spherelord
	{
		auto game_rule = state.world.create_gamerule();
		state.world.gamerule_set_name(game_rule, text::find_or_add_key(state, "Allow spherelings to declare war on spherelord", false));
		state.world.gamerule_set_default_setting(game_rule, uint8_t(sphereling_declare_war_settings::yes));
		gamerule::set_gamerule(state, game_rule, uint8_t(sphereling_declare_war_settings::yes));
		state.world.gamerule_set_setting_description(game_rule, uint8_t(sphereling_declare_war_settings::no), text::find_or_add_key(state, "No", false));
		state.world.gamerule_set_setting_description(game_rule, uint8_t(sphereling_declare_war_settings::yes), text::find_or_add_key(state, "Yes", false));
		state.world.gamerule_set_tooltip_explain(game_rule, text::find_or_add_key(state, "Specifies whether spherelings are allowed to declare on their spherelord,or any nation allied to, or in the same sphere as the spherelord.\n Also affects joining wars against the side the spherelord is on", false));
		state.world.gamerule_set_settings_count(game_rule, 2);
		state.hardcoded_gamerules.sphereling_can_declare_spherelord = game_rule;
	}
}

void restore_gamerule_ui_settings(sys::state& state) {
	state.ui_state.gamerule_ui_settings.clear();
	for(const auto& gamerule : state.world.in_gamerule) {
		state.ui_state.gamerule_ui_settings.insert_or_assign(gamerule.id, gamerule.get_current_setting());
	}
}
void set_gamerule(sys::state& state, dcon::gamerule_id gamerule, uint8_t new_setting) {
	state.world.gamerule_set_current_setting(gamerule, new_setting);
	state.ui_state.gamerule_ui_settings.insert_or_assign(gamerule, new_setting);
}

bool check_gamerule(sys::state& state, dcon::gamerule_id gamerule, uint8_t setting) {
	return state.world.gamerule_get_current_setting(gamerule) == setting;
}

}
