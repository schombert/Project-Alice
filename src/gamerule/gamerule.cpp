#include "system_state.hpp"
#include "gamerule.hpp"



namespace gamerule {

void load_hardcoded_gamerules(sys::state& state) {
	// create gamerule for spherelings declaring war on spherelord
	{
		auto game_rule = state.world.create_gamerule();
		state.world.gamerule_set_name(game_rule, text::find_or_add_key(state, "Allow spherelings to declare war on spherelord", false));
		state.world.gamerule_set_default_setting(game_rule, 1);
		state.world.gamerule_set_current_setting(game_rule, 1);
		state.world.gamerule_set_setting_description(game_rule, uint8_t(sphereling_declare_war_settings::no), text::find_or_add_key(state, "No", false));
		state.world.gamerule_set_setting_description(game_rule, uint8_t(sphereling_declare_war_settings::yes), text::find_or_add_key(state, "Yes", false));
		state.world.gamerule_set_tooltip_explain(game_rule, text::find_or_add_key(state, "Specifies whether spherelings are allowed to declare on their spherelord,or any nation allied to, or in the same sphere as the spherelord.\n Also affects joining wars against the side the spherelord is on", false));
		state.world.gamerule_set_settings_count(game_rule, 2);
		state.hardcoded_gamerules.sphereling_can_declare_spherelord = game_rule;
	}
}
}
