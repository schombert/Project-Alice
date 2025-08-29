#include "gui_scripted_elements.hpp"
#include "dcon_generated.hpp"
#include "triggers.hpp"
#include "effects.hpp"

namespace ui {

int32_t frame_from_datamodel(sys::state& state, ui::datamodel datamodel) {
	// US29AC3 US29AC3 When an icon has `datamodel="state_religion"`, it always displays the state religion of the player
	if(datamodel == ui::datamodel::state_religion) {
		auto country = state.local_player_nation;
		auto fat_id = dcon::fatten(state.world, state.world.nation_get_religion(country));
		return int32_t(fat_id.get_icon() - 1);
	}

	return 0;
}

dcon::scripted_interaction_id get_scripted_element_by_gui_def(sys::state& state, dcon::gui_def_id def) {
	for(auto sel : state.world.in_scripted_interaction) {
		if(sel.get_gui_element() == def) {
			return sel;
		}
	}

	return dcon::scripted_interaction_id{};
}

void province_script_button::button_action(sys::state& state) noexcept {
	auto p = retrieve<dcon::province_id>(state, parent);
	auto sel = get_scripted_element_by_gui_def(state, base_definition);
	if(p && state.local_player_nation)
		command::use_province_button(state, state.local_player_nation, sel, p);
}
void province_script_button::on_update(sys::state& state) noexcept {
	disabled = false;
	auto& def = state.ui_defs.gui[base_definition];
	auto sel = get_scripted_element_by_gui_def(state, base_definition);

	if(def.get_element_type() != ui::element_type::button) {
		disabled = true;
		return;
	}
	if(def.data.button.get_button_scripting() != ui::button_scripting::province) {
		disabled = true;
		return;
	}
	auto p = retrieve<dcon::province_id>(state, parent);
	if(!p) {
		disabled = true;
		return;
	}
	disabled = !command::can_use_province_button(state, state.local_player_nation, sel, p);
	auto new_visible = command::can_see_province_button(state, state.local_player_nation, sel, p);

	if(visible != new_visible) {
		visible = new_visible;
		set_visible(state, visible);
	}
}
void province_script_button::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto& def = state.ui_defs.gui[base_definition];
	auto sel = get_scripted_element_by_gui_def(state, base_definition);

	if(def.get_element_type() != ui::element_type::button)
		return;
	if(def.data.button.get_button_scripting() != ui::button_scripting::province)
		return;
	auto p = retrieve<dcon::province_id>(state, parent);
	if(!p)
		return;
	if(!state.local_player_nation)
		return;

	auto name = state.to_string_view(def.name);
	auto tt_name = std::string{ name } + "_tooltip";
	if(state.key_is_localized(tt_name)) {
		text::add_line(state, contents, std::string_view{ tt_name }, text::variable_type::province, p, text::variable_type::nation, state.world.province_get_nation_from_province_ownership(p), text::variable_type::player, state.local_player_nation);
		text::add_line_break_to_layout(state, contents);
	}

	auto allow = state.world.scripted_interaction_get_allow(sel);
	auto effect = state.world.scripted_interaction_get_effect(sel);
	if(allow) {
		text::add_line(state, contents, "allow_reform_cond"); // Requirements:
		ui::trigger_description(state, contents, allow, trigger::to_generic(p), trigger::to_generic(p), trigger::to_generic(state.local_player_nation));
		text::add_line_break_to_layout(state, contents);
	}
	if(effect) {
		text::add_line(state, contents, "msg_decision_2");
		ui::effect_description(state, contents, effect, trigger::to_generic(p), trigger::to_generic(p), trigger::to_generic(state.local_player_nation), uint32_t(state.current_date.value), uint32_t(p.index() ^ (base_definition.index() << 4)));
	}
}
void nation_script_button::button_action(sys::state& state) noexcept {
	auto n = retrieve<dcon::nation_id>(state, parent);
	auto sel = get_scripted_element_by_gui_def(state, base_definition);

	if(n && state.local_player_nation) {
		command::use_nation_button(state, state.local_player_nation, sel, n);
	} else if(state.local_player_nation) {
		command::use_nation_button(state, state.local_player_nation, sel, state.local_player_nation);
	}
}
void nation_script_button::on_update(sys::state& state) noexcept {
	disabled = false;
	auto& def = state.ui_defs.gui[base_definition];
	auto sel = get_scripted_element_by_gui_def(state, base_definition);

	if(def.get_element_type() != ui::element_type::button) {
		disabled = true;
		return;
	}
	if(def.data.button.get_button_scripting() != ui::button_scripting::nation) {
		disabled = true;
		return;
	}
	auto n = retrieve<dcon::nation_id>(state, parent);
	if(!state.local_player_nation) {
		disabled = true;
		return;
	}
	disabled = !command::can_use_nation_button(state, state.local_player_nation, sel, n ? n : state.local_player_nation);
	auto new_visible = command::can_see_nation_button(state, state.local_player_nation, sel, n ? n : state.local_player_nation);

	if(visible != new_visible) {
		visible = new_visible;
		set_visible(state, visible);
	}
}
void nation_script_button::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto& def = state.ui_defs.gui[base_definition];
	auto sel = get_scripted_element_by_gui_def(state, base_definition);

	if(def.get_element_type() != ui::element_type::button)
		return;
	if(def.data.button.get_button_scripting() != ui::button_scripting::nation)
		return;
	auto n = retrieve<dcon::nation_id>(state, parent);
	if(!n)
		n = state.local_player_nation;
	if(!state.local_player_nation)
		return;

	auto name = state.to_string_view(def.name);
	auto tt_name = std::string{ name } + "_tooltip";
	if(state.key_is_localized(tt_name)) {
		text::add_line(state, contents, std::string_view{ tt_name }, text::variable_type::nation, n, text::variable_type::player, state.local_player_nation);
		text::add_line_break_to_layout(state, contents);
	}

	auto visibletrigger = state.world.scripted_interaction_get_visible(sel);
	auto allow = state.world.scripted_interaction_get_allow(sel);
	auto effect = state.world.scripted_interaction_get_effect(sel);

	if(visibletrigger) {
		text::add_line(state, contents, "visible_cond");
		ui::trigger_description(state, contents, visibletrigger, trigger::to_generic(n), trigger::to_generic(n), trigger::to_generic(state.local_player_nation));
		text::add_line_break_to_layout(state, contents);
	}
	if(allow) {
		text::add_line(state, contents, "allow_reform_cond");
		ui::trigger_description(state, contents, allow, trigger::to_generic(n), trigger::to_generic(n), trigger::to_generic(state.local_player_nation));
		text::add_line_break_to_layout(state, contents);
	}
	if(effect) {
		text::add_line(state, contents, "msg_decision_2");
		ui::effect_description(state, contents, effect, trigger::to_generic(n), trigger::to_generic(n), trigger::to_generic(state.local_player_nation), uint32_t(state.current_date.value), uint32_t(n.index() ^ (base_definition.index() << 4)));
	}
}

void ui_variable_toggle_button::on_update(sys::state& state) noexcept {
	disabled = false;
	auto& def = state.ui_defs.gui[base_definition];

	if(def.get_element_type() != ui::element_type::button) {
		disabled = true;
		return;
	}
	auto n = retrieve<dcon::nation_id>(state, parent);
	if(!state.local_player_nation) {
		disabled = true;
		return;
	}
	// US28AC5 US28AC6 UI variable toggle buttons can have nation-level scripting (visible, allow)
	if(def.data.button.get_button_scripting() == ui::button_scripting::nation) {
		auto sel = get_scripted_element_by_gui_def(state, base_definition);

		disabled = !command::can_use_nation_button(state, state.local_player_nation, sel, n ? n : state.local_player_nation);
		auto new_visible = command::can_see_nation_button(state, state.local_player_nation, sel, n ? n : state.local_player_nation);

		if(visible != new_visible) {
			visible = new_visible;
			set_visible(state, visible);
		}
	}

	// US29AC4
	if(base_data.datamodel != ui::datamodel::none) {
		frame = frame_from_datamodel(state, base_data.datamodel);
	}
}

void ui_variable_toggle_button::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto& def = state.ui_defs.gui[base_definition];
	auto sel = get_scripted_element_by_gui_def(state, base_definition);

	if(def.get_element_type() != ui::element_type::button)
		return;
	if(def.data.button.get_button_scripting() != ui::button_scripting::nation)
		return;
	auto n = retrieve<dcon::nation_id>(state, parent);
	if(!n)
		n = state.local_player_nation;
	if(!state.local_player_nation)
		return;

	auto name = state.to_string_view(def.name);
	auto tt_name = std::string{ name } + "_tooltip";
	if(state.key_is_localized(tt_name)) {
		text::add_line(state, contents, std::string_view{ tt_name }, text::variable_type::nation, n, text::variable_type::player, state.local_player_nation);
		text::add_line_break_to_layout(state, contents);
	}

	auto visibletrigger = state.world.scripted_interaction_get_visible(sel);
	auto allow = state.world.scripted_interaction_get_allow(sel);

	if(visibletrigger) {
		text::add_line(state, contents, "visible_cond");
		ui::trigger_description(state, contents, visibletrigger, trigger::to_generic(n), trigger::to_generic(n), trigger::to_generic(state.local_player_nation));
		text::add_line_break_to_layout(state, contents);
	}
	if(allow) {
		text::add_line(state, contents, "allow_reform_cond");
		ui::trigger_description(state, contents, allow, trigger::to_generic(n), trigger::to_generic(n), trigger::to_generic(state.local_player_nation));
		text::add_line_break_to_layout(state, contents);
	}
}

} // namespace ui
