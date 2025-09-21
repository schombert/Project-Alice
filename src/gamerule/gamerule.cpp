#include "system_state.hpp"
#include "gamerule.hpp"



namespace gamerule {

dcon::gamerule_id create_hardcoded_gamerule(sys::state& state, std::string_view name, std::string_view desc, std::array<sys::gamerule_option, sys::max_gamerule_settings>&& settings, uint8_t default_setting, uint8_t setting_count ) {
	auto game_rule = state.world.create_gamerule();
	state.world.gamerule_set_name(game_rule, text::find_or_add_key(state, name, false));
	state.world.gamerule_set_tooltip_explain(game_rule, text::find_or_add_key(state, desc, false));
	state.world.gamerule_set_options(game_rule, settings);
	state.world.gamerule_set_default_setting(game_rule, default_setting);
	state.world.gamerule_set_current_setting(game_rule, default_setting);
	state.world.gamerule_set_settings_count(game_rule, setting_count);
	return game_rule;
}

void load_hardcoded_gamerules(sys::state& state) {
	// create gamerule for spherelings declaring war on spherelord
	{
		std::array<sys::gamerule_option, sys::max_gamerule_settings> options;
		options[uint8_t(sphereling_declare_war_settings::no)].name = text::find_or_add_key(state, "alice_gamerule_allow_sphereling_declare_war_on_spherelord_opt_1", false);
		options[uint8_t(sphereling_declare_war_settings::yes)].name = text::find_or_add_key(state, "alice_gamerule_allow_sphereling_declare_war_on_spherelord_opt_2", false);
		state.hardcoded_gamerules.sphereling_can_declare_spherelord = create_hardcoded_gamerule(state, "alice_gamerule_allow_sphereling_declare_war_on_spherelord_name", "alice_gamerule_allow_sphereling_declare_war_on_spherelord_desc", std::move(options), uint8_t(state.defines.alice_can_goto_war_against_spherelord_default_setting), uint8_t(2));
	}
}

void restore_gamerule_ui_settings(sys::state& state) {
	state.ui_state.gamerule_ui_settings.clear();
	for(const auto& gamerule : state.world.in_gamerule) {
		state.ui_state.gamerule_ui_settings.insert_or_assign(gamerule.id, gamerule.get_current_setting());
	}
}
void set_gamerule(sys::state& state, dcon::gamerule_id gamerule, uint8_t new_setting) {
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
}



bool check_gamerule(sys::state& state, dcon::gamerule_id gamerule, uint8_t setting) {
	return state.world.gamerule_get_current_setting(gamerule) == setting;
}

}
