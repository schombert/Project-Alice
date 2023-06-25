#pragma once

#include "gui_element_types.hpp"
#include "military.hpp"

namespace ui {

class naval_combat_ship_lslot : public window_element_base {
	image_element_base* nav_icon_norm = nullptr;
	image_element_base* nav_icon_attack = nullptr;
	image_element_base* nav_icon_sunk = nullptr;
	image_element_base* nav_icon_retreat = nullptr;
	image_element_base* nav_icon_disengaged = nullptr;
	image_element_base* nav_icon_seeking = nullptr;
	vertical_progress_bar* strbar = nullptr;
	vertical_progress_bar* orgbar = nullptr;

public:
	military::ship_in_battle battleship;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "attacker_icon_normal") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			nav_icon_norm = ptr.get();
			return ptr;

		} else if(name == "attacker_icon_attacking") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			nav_icon_attack = ptr.get();
			return ptr;

		} else if(name == "attacker_icon_sunk") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			nav_icon_sunk = ptr.get();
			return ptr;

		} else if(name == "attacker_icon_retreat") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			nav_icon_retreat = ptr.get();
			return ptr;

		} else if(name == "attacker_icon_disengaged") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			nav_icon_disengaged = ptr.get();
			return ptr;

		} else if(name == "seeking_target") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			nav_icon_seeking = ptr.get();
			return ptr;

		} else if(name == "str") {
			auto ptr = make_element_by_type<vertical_progress_bar>(state, id);
			strbar = ptr.get();
			return ptr;

		} else if(name == "org") {
			auto ptr = make_element_by_type<vertical_progress_bar>(state, id);
			orgbar = ptr.get();
			return ptr;

		} else if(name == "mini_frame_str") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "mini_frame_org") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "torpedo") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;

		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(dcon::fatten(state.world, battleship.ship).is_valid()) {
			strbar->progress = dcon::fatten(state.world, battleship.ship).get_strength();
			orgbar->progress = dcon::fatten(state.world, battleship.ship).get_org();

			dcon::unit_type_id utid = dcon::fatten(state.world, battleship.ship).get_type();
			auto result = state.military_definitions.unit_base_definitions[utid].icon - 4;

			if((battleship.flags bitand military::ship_in_battle::mode_mask) == military::ship_in_battle::mode_approaching) {
				//nav_icon_norm->set_visible(state, false);
				nav_icon_attack->set_visible(state, false);
				nav_icon_sunk->set_visible(state, false);
				nav_icon_retreat->set_visible(state, false);
				nav_icon_disengaged->set_visible(state, false);
				nav_icon_seeking->set_visible(state, false);

				nav_icon_norm->set_visible(state, true);
				nav_icon_norm->frame = result;

			} else
			if((battleship.flags bitand military::ship_in_battle::mode_mask) == military::ship_in_battle::mode_seeking) {
				//nav_icon_norm->set_visible(state, false);
				nav_icon_attack->set_visible(state, false);
				nav_icon_sunk->set_visible(state, false);
				nav_icon_retreat->set_visible(state, false);
				nav_icon_disengaged->set_visible(state, false);
				//nav_icon_seeking->set_visible(state, false);

				nav_icon_seeking->set_visible(state, true);	// Not actually used in V2 but whatever
				nav_icon_norm->set_visible(state, true);
				nav_icon_norm->frame = result;

			} else
			if((battleship.flags bitand military::ship_in_battle::mode_mask) == military::ship_in_battle::mode_engaged) {
				nav_icon_norm->set_visible(state, false);
				//nav_icon_attack->set_visible(state, false);
				nav_icon_sunk->set_visible(state, false);
				nav_icon_retreat->set_visible(state, false);
				nav_icon_disengaged->set_visible(state, false);
				nav_icon_seeking->set_visible(state, false);

				nav_icon_attack->set_visible(state, true);
				nav_icon_attack->frame = result;

			} else
			if((battleship.flags bitand military::ship_in_battle::mode_mask) == military::ship_in_battle::mode_retreating) {
				nav_icon_norm->set_visible(state, false);
				nav_icon_attack->set_visible(state, false);
				nav_icon_sunk->set_visible(state, false);
				//nav_icon_retreat->set_visible(state, false);
				nav_icon_disengaged->set_visible(state, false);
				nav_icon_seeking->set_visible(state, false);

				nav_icon_retreat->set_visible(state, true);
				nav_icon_retreat->frame = result;

			} else
			if((battleship.flags bitand military::ship_in_battle::mode_mask) == military::ship_in_battle::mode_retreated) {
				nav_icon_norm->set_visible(state, false);
				nav_icon_attack->set_visible(state, false);
				nav_icon_sunk->set_visible(state, false);
				nav_icon_retreat->set_visible(state, false);
				//nav_icon_disengaged->set_visible(state, false);
				nav_icon_seeking->set_visible(state, false);

				nav_icon_disengaged->set_visible(state, true);
				nav_icon_disengaged->frame = result;

			} else
			if((battleship.flags bitand military::ship_in_battle::mode_mask) == military::ship_in_battle::mode_sunk) {
				nav_icon_norm->set_visible(state, false);
				nav_icon_attack->set_visible(state, false);
				//nav_icon_sunk->set_visible(state, false);
				nav_icon_retreat->set_visible(state, false);
				nav_icon_disengaged->set_visible(state, false);
				nav_icon_seeking->set_visible(state, false);

				nav_icon_sunk->set_visible(state, true);
				nav_icon_sunk->frame = result;

			}
		}
	}
};

class naval_combat_ship_rslot : public window_element_base {
	image_element_base* nav_icon_norm = nullptr;
	image_element_base* nav_icon_attack = nullptr;
	image_element_base* nav_icon_sunk = nullptr;
	image_element_base* nav_icon_retreat = nullptr;
	image_element_base* nav_icon_disengaged = nullptr;
	image_element_base* nav_icon_seeking = nullptr;
	vertical_progress_bar* strbar = nullptr;
	vertical_progress_bar* orgbar = nullptr;

public:
	military::ship_in_battle battleship;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "attacker_icon_normal") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			nav_icon_norm = ptr.get();
			return ptr;

		} else if(name == "attacker_icon_attacking") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			nav_icon_attack = ptr.get();
			return ptr;

		} else if(name == "attacker_icon_sunk") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			nav_icon_sunk = ptr.get();
			return ptr;

		} else if(name == "attacker_icon_retreat") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			nav_icon_retreat = ptr.get();
			return ptr;

		} else if(name == "attacker_icon_disengaged") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			nav_icon_disengaged = ptr.get();
			return ptr;

		} else if(name == "seeking_target") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			nav_icon_seeking = ptr.get();
			return ptr;

		} else if(name == "str") {
			auto ptr = make_element_by_type<vertical_progress_bar>(state, id);
			strbar = ptr.get();
			return ptr;

		} else if(name == "org") {
			auto ptr = make_element_by_type<vertical_progress_bar>(state, id);
			orgbar = ptr.get();
			return ptr;

		} else if(name == "mini_frame_str") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "mini_frame_org") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "torpedo") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;


		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(dcon::fatten(state.world, battleship.ship).is_valid()) {
			strbar->progress = dcon::fatten(state.world, battleship.ship).get_strength();
			orgbar->progress = dcon::fatten(state.world, battleship.ship).get_org();

			dcon::unit_type_id utid = dcon::fatten(state.world, battleship.ship).get_type();
			auto result = state.military_definitions.unit_base_definitions[utid].icon - 4;

			if((battleship.flags bitand military::ship_in_battle::mode_mask) == military::ship_in_battle::mode_approaching) {
				//nav_icon_norm->set_visible(state, false);
				nav_icon_attack->set_visible(state, false);
				nav_icon_sunk->set_visible(state, false);
				nav_icon_retreat->set_visible(state, false);
				nav_icon_disengaged->set_visible(state, false);
				nav_icon_seeking->set_visible(state, false);

				nav_icon_norm->set_visible(state, true);
				nav_icon_norm->frame = result;

			} else
			if((battleship.flags bitand military::ship_in_battle::mode_mask) == military::ship_in_battle::mode_seeking) {
				//nav_icon_norm->set_visible(state, false);
				nav_icon_attack->set_visible(state, false);
				nav_icon_sunk->set_visible(state, false);
				nav_icon_retreat->set_visible(state, false);
				nav_icon_disengaged->set_visible(state, false);
				//nav_icon_seeking->set_visible(state, false);

				nav_icon_seeking->set_visible(state, true);	// Not actually used in V2 but whatever
				nav_icon_norm->set_visible(state, true);
				nav_icon_norm->frame = result;

			} else
			if((battleship.flags bitand military::ship_in_battle::mode_mask) == military::ship_in_battle::mode_engaged) {
				nav_icon_norm->set_visible(state, false);
				//nav_icon_attack->set_visible(state, false);
				nav_icon_sunk->set_visible(state, false);
				nav_icon_retreat->set_visible(state, false);
				nav_icon_disengaged->set_visible(state, false);
				nav_icon_seeking->set_visible(state, false);

				nav_icon_attack->set_visible(state, true);
				nav_icon_attack->frame = result;

			} else
			if((battleship.flags bitand military::ship_in_battle::mode_mask) == military::ship_in_battle::mode_retreating) {
				nav_icon_norm->set_visible(state, false);
				nav_icon_attack->set_visible(state, false);
				nav_icon_sunk->set_visible(state, false);
				//nav_icon_retreat->set_visible(state, false);
				nav_icon_disengaged->set_visible(state, false);
				nav_icon_seeking->set_visible(state, false);

				nav_icon_retreat->set_visible(state, true);
				nav_icon_retreat->frame = result;

			} else
			if((battleship.flags bitand military::ship_in_battle::mode_mask) == military::ship_in_battle::mode_retreated) {
				nav_icon_norm->set_visible(state, false);
				nav_icon_attack->set_visible(state, false);
				nav_icon_sunk->set_visible(state, false);
				nav_icon_retreat->set_visible(state, false);
				//nav_icon_disengaged->set_visible(state, false);
				nav_icon_seeking->set_visible(state, false);

				nav_icon_disengaged->set_visible(state, true);
				nav_icon_disengaged->frame = result;

			} else
			if((battleship.flags bitand military::ship_in_battle::mode_mask) == military::ship_in_battle::mode_sunk) {
				nav_icon_norm->set_visible(state, false);
				nav_icon_attack->set_visible(state, false);
				//nav_icon_sunk->set_visible(state, false);
				nav_icon_retreat->set_visible(state, false);
				nav_icon_disengaged->set_visible(state, false);
				nav_icon_seeking->set_visible(state, false);

				nav_icon_sunk->set_visible(state, true);
				nav_icon_sunk->frame = result;

			}
		}
	}
};

class naval_combat_ship_unit : public listbox_row_element_base<military::ship_in_battle> {
	simple_text_element_base* combatstatus_text = nullptr;
	simple_text_element_base* attacker_ship_text = nullptr;
	simple_text_element_base* defender_ship_text = nullptr;
	naval_combat_ship_lslot* ship_lslot = nullptr;
	naval_combat_ship_rslot* ship_rslot = nullptr;
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "bg") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "highlight") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "left_slot") {
			auto ptr = make_element_by_type<naval_combat_ship_lslot>(state, id);
			ship_lslot = ptr.get();	// Its always on the left
			return ptr;

		} else if(name == "right_slot") {
			auto ptr = make_element_by_type<naval_combat_ship_rslot>(state, id);
			ship_rslot = ptr.get();	// Its always on the right
			return ptr;

		} else if(name == "attacker_name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			attacker_ship_text = ptr.get();
			return ptr;

		} else if(name == "defender_name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			defender_ship_text = ptr.get();
			return ptr;

		} else if(name == "attacker_flag") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "defender_flag") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "status") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			combatstatus_text = ptr.get();
			return ptr;

		} else if(name == "range_r") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;

		} else if(name == "range_l") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;


		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::naval_battle_id{};
			parent->impl_get(state, payload);
			auto fat = dcon::fatten(state.world, any_cast<dcon::naval_battle_id>(payload));

			if(!fat.is_valid()) {
				return;
			}

			if((content.flags bitand military::ship_in_battle::mode_mask) == military::ship_in_battle::is_attacking) {
				ship_lslot->battleship = content;
				if((content.flags bitand military::ship_in_battle::mode_approaching) not_eq 0
				|| (content.flags bitand military::ship_in_battle::mode_engaged) not_eq 0
				|| (content.flags bitand military::ship_in_battle::mode_retreating) not_eq 0) {	// TODO - needs to be verified, i think its shown but cant be sure
					ship_rslot->battleship = fat.get_slots()[content.target_slot];
				} else {
					ship_rslot->battleship = military::ship_in_battle{};
				}
			} else if((content.flags bitand military::ship_in_battle::mode_mask) != military::ship_in_battle::is_attacking) {
				ship_rslot->battleship = content;
				if((content.flags bitand military::ship_in_battle::mode_approaching) not_eq 0
				|| (content.flags bitand military::ship_in_battle::mode_engaged) not_eq 0
				|| (content.flags bitand military::ship_in_battle::mode_retreating) not_eq 0) {	// TODO - needs to be verified, i think its shown but cant be sure
					ship_lslot->battleship = fat.get_slots()[content.target_slot];
				} else {
					ship_lslot->battleship = military::ship_in_battle{};
				}
			}

			if((content.flags bitand military::ship_in_battle::mode_mask) == military::ship_in_battle::mode_approaching) {
				combatstatus_text->set_text(state, text::produce_simple_string(state, "naval_combat_status_approach"));

			} else
			if((content.flags bitand military::ship_in_battle::mode_mask) == military::ship_in_battle::mode_seeking) {
				combatstatus_text->set_text(state, text::produce_simple_string(state, "naval_combat_status_seeking"));

			} else
			if((content.flags bitand military::ship_in_battle::mode_mask) == military::ship_in_battle::mode_engaged) {
				combatstatus_text->set_text(state, text::produce_simple_string(state, "naval_combat_status_firing"));

			} else
			if((content.flags bitand military::ship_in_battle::mode_mask) == military::ship_in_battle::mode_retreating) {
				combatstatus_text->set_text(state, text::produce_simple_string(state, "naval_combat_status_retreat"));

			} else
			if((content.flags bitand military::ship_in_battle::mode_mask) == military::ship_in_battle::mode_retreated) {
				combatstatus_text->set_text(state, text::produce_simple_string(state, "naval_combat_status_disengaged"));

			} else {
				combatstatus_text->set_text(state, "STATE - INVALID");
			}
			ship_lslot->on_update(state);
			ship_rslot->on_update(state);
		}
	}
};

template<bool attacker>
class naval_slots_listbox : public listbox_element_base<naval_combat_ship_unit, military::ship_in_battle> {
protected:
	std::string_view get_row_element_name() override {
		return "ship_unit";
	}

public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::naval_battle_id{};
			parent->impl_get(state, payload);
			auto fat = dcon::fatten(state.world, any_cast<dcon::naval_battle_id>(payload));
			row_contents.clear();

			for(auto s : fat.get_slots()) {
				if constexpr(attacker == true) {
					if(attacker == true && (s.flags bitand military::ship_in_battle::is_attacking) != 0) {
						row_contents.push_back(s);
					}
				} else
				if constexpr(attacker == false) {
					if(attacker == false && (s.flags bitand military::ship_in_battle::is_attacking) == 0) {
						row_contents.push_back(s);
					}
				}
			}

			update(state);
		}
	}
};

enum class modifiertype : uint8_t {
	dice = 0,
	leader
};

class combat_modifier_win : public window_element_base {
	image_element_base* modifier_icon = nullptr;
	simple_text_element_base* modifier_text = nullptr;
public:
	modifiertype type = modifiertype::dice;
	uint8_t value = 0;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "modifier_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			modifier_icon = ptr.get();
			return ptr;
		} else if(name == "modifier_value") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			modifier_text = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		switch(type) {
			case modifiertype::dice:
				modifier_icon->frame = 0;
				break;
			case modifiertype::leader:
				modifier_icon->frame = 4;
				break;
			default:
				modifier_icon->frame = 0;
				break;
		}
		modifier_text->set_text(state, text::format_float(float(value), 0));
	}
};

class naval_combat_attacker_window : public window_element_base {

	simple_text_element_base* attacker_leadername_text = nullptr;
	simple_text_element_base* attacker_bigship_amount = nullptr;
	simple_text_element_base* attacker_smallship_amount = nullptr;
	simple_text_element_base* attacker_transship_amount = nullptr;
	image_element_base* attacker_nation_flag = nullptr;

	progress_bar* orgbar = nullptr;
	progress_bar* strbar = nullptr;
	simple_text_element_base* organisation_text = nullptr;
	simple_text_element_base* strength_text = nullptr;

	window_element_base* dice_roll = nullptr;
	window_element_base* leader_roll = nullptr;
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		{
			auto ptr = make_element_by_type<combat_modifier_win>(state, state.ui_state.defs_by_name.find("combat_modifier")->second.definition);
			dice_roll = ptr.get();
			dice_roll->base_data.position.x = 78;
			dice_roll->base_data.position.y = 39;
		}
		{
			auto ptr = make_element_by_type<combat_modifier_win>(state, state.ui_state.defs_by_name.find("combat_modifier")->second.definition);
			leader_roll = ptr.get();
			leader_roll->base_data.position.x = 78 + 31;
			leader_roll->base_data.position.y = 39;
		}
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "shield") {
			return make_element_by_type<flag_button>(state, id);

		} else if(name == "leader_icon") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "leader_name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			attacker_leadername_text = ptr.get();
			return ptr;

		} else if(name == "morale") {
			auto ptr = make_element_by_type<progress_bar>(state, id);
			orgbar = ptr.get();
			return ptr;

		} else if(name == "strength") {
			auto ptr = make_element_by_type<progress_bar>(state, id);
			strbar = ptr.get();
			return ptr;

		} else if(name == "morale_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			organisation_text = ptr.get();
			return ptr;

		} else if(name == "strength_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			strength_text = ptr.get();
			return ptr;

		} else if(name == "morale_frame") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "strength_frame") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "big_ship_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = ptr->frame - 1;
			return ptr;


		} else if(name == "small_ship_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = ptr->frame - 1;
			return ptr;


		} else if(name == "trade_ship_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = ptr->frame - 1;
			return ptr;


		} else if(name == "big_ship_value") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			attacker_bigship_amount = ptr.get();
			return ptr;

		} else if(name == "small_ship_value") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			attacker_smallship_amount = ptr.get();
			return ptr;

		} else if(name == "trade_ship_value") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			attacker_transship_amount = ptr.get();
			return ptr;

		} else if(name == "slots_list") {
			return make_element_by_type<naval_slots_listbox<true>>(state, id);

		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::naval_battle_id{};
			parent->impl_get(state, payload);
			auto fat = dcon::fatten(state.world, any_cast<dcon::naval_battle_id>(payload));

			if(fat.get_admiral_from_attacking_admiral().is_valid()) {
				attacker_leadername_text->set_text(state, text::produce_simple_string(state, state.to_string_view(fat.get_admiral_from_attacking_admiral().get_name())));
			} else {
				attacker_leadername_text->set_text(state, text::produce_simple_string(state, "no_leader"));
			}
			// TODO - add the vertical flag for the attacking side here
			attacker_bigship_amount->set_text(state, text::format_float(fat.get_attacker_big_ships(), 0));
			attacker_smallship_amount->set_text(state, text::format_float(fat.get_attacker_small_ships(), 0));
			attacker_transship_amount->set_text(state, text::format_float(fat.get_attacker_transport_ships(), 0));

			float totalorg = 0.0f;
			uint16_t totalships = 0;
			for(auto s : fat.get_slots()) {
				if((s.flags bitand military::ship_in_battle::mode_mask) == military::ship_in_battle::is_attacking) {
					totalorg += dcon::fatten(state.world, s.ship).get_org();
					totalships += 1;
				}
			}
			orgbar->progress = (totalorg / totalships);
			organisation_text->set_text(state, text::format_percentage((totalorg / totalships), 0));

			float totalstr = 0.0f;
			totalships = 0;
			for(auto s : fat.get_slots()) {
				if((s.flags bitand military::ship_in_battle::mode_mask) == military::ship_in_battle::is_attacking) {
					totalorg += dcon::fatten(state.world, s.ship).get_strength();
					totalships += 1;
				}
			}
			strbar->progress = (totalstr / totalships);
			strength_text->set_text(state, text::format_percentage((totalstr / totalships), 0));

		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			Cyto::Any d_payload = dcon::naval_battle_id{};
			parent->impl_get(state, d_payload);
			auto fat = dcon::fatten(state.world, any_cast<dcon::naval_battle_id>(d_payload));

			payload.emplace<dcon::nation_id>(fat.get_admiral_from_attacking_admiral().get_nation_from_leader_loyalty().id);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class naval_combat_defender_window : public window_element_base {
	simple_text_element_base* defender_leadername_text = nullptr;
	simple_text_element_base* defender_bigship_amount = nullptr;
	simple_text_element_base* defender_smallship_amount = nullptr;
	simple_text_element_base* defender_transship_amount = nullptr;
	image_element_base* defender_nation_flag = nullptr;

	progress_bar* orgbar = nullptr;
	progress_bar* strbar = nullptr;
	simple_text_element_base* organisation_text = nullptr;
	simple_text_element_base* strength_text = nullptr;

	window_element_base* dice_roll = nullptr;
	window_element_base* leader_roll = nullptr;
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		{
			auto ptr = make_element_by_type<combat_modifier_win>(state, state.ui_state.defs_by_name.find("combat_modifier")->second.definition);
			dice_roll = ptr.get();
			dice_roll->base_data.position.x = 78;
			dice_roll->base_data.position.y = 39;
		}
		{
			auto ptr = make_element_by_type<combat_modifier_win>(state, state.ui_state.defs_by_name.find("combat_modifier")->second.definition);
			leader_roll = ptr.get();
			leader_roll->base_data.position.x = 78 + 31;
			leader_roll->base_data.position.y = 39;
		}
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "shield") {
			return make_element_by_type<flag_button>(state, id);

		} else if(name == "leader_icon") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "leader_name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			defender_leadername_text = ptr.get();
			return ptr;

		} else if(name == "morale") {
			auto ptr = make_element_by_type<progress_bar>(state, id);
			orgbar = ptr.get();
			return ptr;

		} else if(name == "strength") {
			auto ptr = make_element_by_type<progress_bar>(state, id);
			strbar = ptr.get();
			return ptr;

		} else if(name == "morale_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			organisation_text = ptr.get();
			return ptr;

		} else if(name == "strength_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			strength_text = ptr.get();
			return ptr;

		} else if(name == "morale_frame") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "strength_frame") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "big_ship_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = ptr->frame - 1;
			return ptr;


		} else if(name == "small_ship_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = ptr->frame - 1;
			return ptr;


		} else if(name == "trade_ship_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = ptr->frame - 1;
			return ptr;

		} else if(name == "big_ship_value") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			defender_bigship_amount = ptr.get();
			return ptr;

		} else if(name == "small_ship_value") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			defender_smallship_amount = ptr.get();
			return ptr;

		} else if(name == "trade_ship_value") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			defender_transship_amount = ptr.get();
			return ptr;

		} else if(name == "slots_list") {
			return make_element_by_type<naval_slots_listbox<false>>(state, id);

		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::naval_battle_id{};
			parent->impl_get(state, payload);
			auto fat = dcon::fatten(state.world, any_cast<dcon::naval_battle_id>(payload));

			if(fat.get_admiral_from_defending_admiral().is_valid()) {
				defender_leadername_text->set_text(state, text::produce_simple_string(state, state.to_string_view(fat.get_admiral_from_defending_admiral().get_name())));
			} else {
				defender_leadername_text->set_text(state, text::produce_simple_string(state, "no_leader"));
			}
			// TODO - add vertical defender flag here
			defender_bigship_amount->set_text(state, text::format_float(fat.get_defender_big_ships(), 0));
			defender_smallship_amount->set_text(state, text::format_float(fat.get_defender_small_ships(), 0));
			defender_transship_amount->set_text(state, text::format_float(fat.get_defender_transport_ships(), 0));

			float totalorg = 0.0f;
			uint16_t totalships = 0;
			for(auto s : fat.get_slots()) {
				if((s.flags bitand military::ship_in_battle::mode_mask) != military::ship_in_battle::is_attacking) {
					totalorg += dcon::fatten(state.world, s.ship).get_org();
					totalships += 1;
				}
			}
			orgbar->progress = ((totalorg / totalships) > 0) ? (totalorg / totalships) : 0.0f;
			organisation_text->set_text(state, text::format_percentage( ((totalorg / totalships) > 0) ? (totalorg / totalships) : 0.0f, 0));

			float totalstr = 0.0f;
			totalships = 0;
			for(auto s : fat.get_slots()) {
				if((s.flags bitand military::ship_in_battle::mode_mask) != military::ship_in_battle::is_attacking) {
					totalorg += dcon::fatten(state.world, s.ship).get_strength();
					totalships += 1;
				}
			}
			strbar->progress = ((totalstr / totalships) > 0) ? (totalstr / totalships) : 0.0f;
			strength_text->set_text(state, text::format_percentage( ((totalstr / totalships) > 0) ? (totalstr / totalships) : 0.0f, 0));
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			Cyto::Any d_payload = dcon::naval_battle_id{};
			parent->impl_get(state, d_payload);
			auto fat = dcon::fatten(state.world, any_cast<dcon::naval_battle_id>(d_payload));

			payload.emplace<dcon::nation_id>(fat.get_admiral_from_defending_admiral().get_nation_from_leader_loyalty().id);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class naval_combat_window : public window_element_base {
	dcon::navy_id selectednavy;

	image_element_base* mayretreat_image = nullptr;
	simple_text_element_base* navalbattleloc_text = nullptr;
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		state.ui_state.naval_combat_window = this;
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "combat_bg") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "sort_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "sortby_type") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "sortby_stance") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "sortby_str_org") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "label_battlename") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			navalbattleloc_text = ptr.get();
			return ptr;

		} else if(name == "combat_may_retreat") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->set_visible(state, false);
			mayretreat_image = ptr.get();
			return ptr;

		} else if(name == "closebutton") {
			return make_element_by_type<generic_close_button>(state, id);

		} else if(name == "attacker") {
			return make_element_by_type<naval_combat_attacker_window>(state, id);

		} else if(name == "defender") {
			return make_element_by_type<naval_combat_defender_window>(state, id);

		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		auto battle = dcon::fatten(state.world, selectednavy).get_battle_from_navy_battle_participation();
		if(!battle.is_valid()) {
			set_visible(state, false);
		}



		mayretreat_image->set_visible(state, military::can_retreat_from_battle(state, battle.id, state.local_player_nation));
		auto fat = dcon::fatten(state.world, selectednavy);

		navalbattleloc_text->set_text(state, text::produce_simple_string(state, "battle_of") + " " +
			text::produce_simple_string(state, battle.get_location_from_naval_battle_location().get_name()));
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<element_selection_wrapper<dcon::navy_id>>()) {
			selectednavy = any_cast<element_selection_wrapper<dcon::navy_id>>(payload).data;
			return message_result::consumed;
		}
		//======================================================================================
		else if(payload.holds_type<dcon::navy_id>()) {
			payload.emplace<dcon::navy_id>(selectednavy);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::naval_battle_id>()) {
			payload.emplace<dcon::naval_battle_id>(dcon::fatten(state.world, selectednavy).get_battle_from_navy_battle_participation().id);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

//===============================================================================================================================

class naval_combat_end_popup_close_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<bool>{bool{true}};
			parent->impl_get(state, payload);
		}
	}
};

class naval_combat_end_popup : public window_element_base {
private:
	image_element_base* loss_image = nullptr;
	image_element_base* win_image = nullptr;
	simple_text_element_base* battlename_text = nullptr;
	simple_text_element_base* winorlose_text = nullptr;
	simple_text_element_base* warscore_text = nullptr;
	simple_text_element_base* ourleader_text = nullptr;
	simple_text_element_base* enemyleader_text = nullptr;

	//===============================================================

	simple_text_element_base* left_bigship_initial_text = nullptr;
	simple_text_element_base* left_bigship_casualties_text = nullptr;
	simple_text_element_base* left_bigship_survivors_text = nullptr;

	simple_text_element_base* left_smallship_initial_text = nullptr;
	simple_text_element_base* left_smallship_casualties_text = nullptr;
	simple_text_element_base* left_smallship_survivors_text = nullptr;

	simple_text_element_base* left_transship_initial_text = nullptr;
	simple_text_element_base* left_transship_casualties_text = nullptr;
	simple_text_element_base* left_transship_survivors_text = nullptr;

	//===============================================================

	simple_text_element_base* right_bigship_initial_text = nullptr;
	simple_text_element_base* right_bigship_casualties_text = nullptr;
	simple_text_element_base* right_bigship_survivors_text = nullptr;

	simple_text_element_base* right_smallship_initial_text = nullptr;
	simple_text_element_base* right_smallship_casualties_text = nullptr;
	simple_text_element_base* right_smallship_survivors_text = nullptr;

	simple_text_element_base* right_transship_initial_text = nullptr;
	simple_text_element_base* right_transship_casualties_text = nullptr;
	simple_text_element_base* right_transship_survivors_text = nullptr;

	//===============================================================

	simple_text_element_base* left_total_initial_text = nullptr;
	simple_text_element_base* left_total_lost_text = nullptr;
	simple_text_element_base* left_total_left_text = nullptr;

	simple_text_element_base* right_total_initial_text = nullptr;
	simple_text_element_base* right_total_lost_text = nullptr;
	simple_text_element_base* right_total_left_text = nullptr;

	//===============================================================
public:
	military::naval_battle_report report;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "background") {
			auto ptr = make_element_by_type<draggable_target>(state, id);
			// ptr->base_data.size = base_data.size; // Nudge
			return ptr;
		} else if(name == "combat_end_naval_lost") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			loss_image = ptr.get();
			return ptr;

		} else if(name == "combat_end_naval_won") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			win_image = ptr.get();
			return ptr;

		} else if(name == "line1") {
			auto ptr =  make_element_by_type<simple_text_element_base>(state, id);
			battlename_text = ptr.get();
			return ptr;

		} else if(name == "ourleader_photo") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "ourleader") { // Our great leader, the leader of the universe, Puffy, Puffy the cat, tremble in her might
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ourleader_text = ptr.get();
			return ptr;

		} else if(name == "prestige_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "prestige_number") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "we_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "we_number") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "initial_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "casualties_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "survivors_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "enemyleader_photo") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "enemyleader") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			enemyleader_text = ptr.get();
			return ptr;

		} else if(name == "prestige_icon2") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "prestige_number2") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "we_icon2") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "we_number2") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "initial_label2") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "casualties_label2") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "survivors_label2") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "our_unit_type_1") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = ptr->frame -1;
			return ptr;

		} else if(name == "our_unit_type_1_1_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			left_bigship_initial_text = ptr.get();
			return ptr;

		} else if(name == "our_unit_type_1_2_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			left_bigship_casualties_text = ptr.get();
			return ptr;

		} else if(name == "our_unit_type_1_3_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			left_bigship_survivors_text = ptr.get();
			return ptr;

		} else if(name == "our_unit_type_2") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = ptr->frame -1;
			return ptr;

		} else if(name == "our_unit_type_2_1_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			left_smallship_initial_text = ptr.get();
			return ptr;

		} else if(name == "our_unit_type_2_2_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			left_smallship_casualties_text = ptr.get();
			return ptr;

		} else if(name == "our_unit_type_2_3_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			left_smallship_survivors_text = ptr.get();
			return ptr;

		} else if(name == "our_unit_type_3") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = ptr->frame -1;
			return ptr;

		} else if(name == "our_unit_type_3_1_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			left_transship_initial_text = ptr.get();
			return ptr;

		} else if(name == "our_unit_type_3_2_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			left_transship_casualties_text = ptr.get();
			return ptr;

		} else if(name == "our_unit_type_3_3_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			left_transship_survivors_text = ptr.get();
			return ptr;

		} else if(name == "enemy_unit_type_1") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = ptr->frame -1;
			return ptr;

		} else if(name == "enemy_unit_type_1_1_text") {
			auto ptr =  make_element_by_type<simple_text_element_base>(state, id);
			right_bigship_initial_text = ptr.get();
			return ptr;

		} else if(name == "enemy_unit_type_1_2_text") {
			auto ptr =  make_element_by_type<simple_text_element_base>(state, id);
			right_bigship_casualties_text = ptr.get();
			return ptr;

		} else if(name == "enemy_unit_type_1_3_text") {
			auto ptr =  make_element_by_type<simple_text_element_base>(state, id);
			right_bigship_survivors_text = ptr.get();
			return ptr;

		} else if(name == "enemy_unit_type_2") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = ptr->frame -1;
			return ptr;

		} else if(name == "enemy_unit_type_2_1_text") {
			auto ptr =  make_element_by_type<simple_text_element_base>(state, id);
			right_smallship_initial_text = ptr.get();
			return ptr;

		} else if(name == "enemy_unit_type_2_2_text") {
			auto ptr =  make_element_by_type<simple_text_element_base>(state, id);
			right_smallship_casualties_text = ptr.get();
			return ptr;

		} else if(name == "enemy_unit_type_2_3_text") {
			auto ptr =  make_element_by_type<simple_text_element_base>(state, id);
			right_smallship_survivors_text = ptr.get();
			return ptr;

		} else if(name == "enemy_unit_type_3") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = ptr->frame -1;
			return ptr;

		} else if(name == "enemy_unit_type_3_1_text") {
			auto ptr =  make_element_by_type<simple_text_element_base>(state, id);
			right_transship_initial_text = ptr.get();
			return ptr;

		} else if(name == "enemy_unit_type_3_2_text") {
			auto ptr =  make_element_by_type<simple_text_element_base>(state, id);
			right_transship_casualties_text = ptr.get();
			return ptr;

		} else if(name == "enemy_unit_type_3_3_text") {
			auto ptr =  make_element_by_type<simple_text_element_base>(state, id);
			right_transship_survivors_text = ptr.get();
			return ptr;

		} else if(name == "our_total_armies") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			left_total_initial_text = ptr.get();
			return ptr;

		} else if(name == "our_total_loss") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			left_total_lost_text = ptr.get();
			return ptr;

		} else if(name == "our_total_left") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			left_total_left_text = ptr.get();
			return ptr;

		} else if(name == "enemy_total_armies") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			right_total_initial_text = ptr.get();
			return ptr;

		} else if(name == "enemy_total_loss") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			right_total_lost_text = ptr.get();
			return ptr;

		} else if(name == "enemy_total_left") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			right_total_left_text = ptr.get();
			return ptr;

		} else if(name == "agreebutton") {
			auto ptr = make_element_by_type<naval_combat_end_popup_close_button>(state, id);
			ptr->base_data.position.y += 146; // Nudge
			return ptr;
		} else if(name == "declinebutton") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->base_data.position.y += 146; // Nudge
			ptr->set_button_text(state, text::produce_simple_string(state, "navalbattleover_btn3"));
			return ptr;
		} else if(name == "centerok") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->set_visible(state, false); // Disacrd
			return ptr;
		} else if(name == "warscore_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "warscore") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			warscore_text = ptr.get();
			return ptr;
		} else if(name == "winorlose") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			winorlose_text = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		battlename_text->set_text(state, (text::produce_simple_string(state, "battle_of") + " " + text::produce_simple_string(state, dcon::fatten(state.world, report.location).get_name())));
		if(report.player_on_winning_side) {
			win_image->set_visible(state, true);
			loss_image->set_visible(state, false);
		} else {
			win_image->set_visible(state, false);
			loss_image->set_visible(state, true);
		}

		warscore_text->set_text(state, text::format_float(report.warscore_effect, 1));
		winorlose_text->set_text(state, report.player_on_winning_side ? text::produce_simple_string(state, "you_won") :
										text::produce_simple_string(state, "you_lost"));
		auto attack_leader = dcon::fatten(state.world, report.attacking_admiral);
		auto defend_leader = dcon::fatten(state.world, report.defending_admiral);

		attack_leader.is_valid() ? ourleader_text->set_text(state, text::produce_simple_string(state, state.to_string_view(attack_leader.get_name())))
					: ourleader_text->set_text(state, text::produce_simple_string(state, "no_leader"));
		defend_leader.is_valid() ? enemyleader_text->set_text(state, text::produce_simple_string(state, state.to_string_view(defend_leader.get_name())))
					: enemyleader_text->set_text(state, text::produce_simple_string(state, "no_leader"));

		//============================================================================================

		left_bigship_initial_text->set_text(state, text::format_float(report.attacker_big_ships, 0));
		left_smallship_initial_text->set_text(state, text::format_float(report.attacker_small_ships, 0));
		left_transship_initial_text->set_text(state, text::format_float(report.attacker_transport_ships, 0));

		right_bigship_initial_text->set_text(state, text::format_float(report.defender_big_ships, 0));
		right_smallship_initial_text->set_text(state, text::format_float(report.defender_small_ships, 0));
		right_transship_initial_text->set_text(state, text::format_float(report.defender_transport_ships, 0));

		//============================================================================================

		left_bigship_casualties_text->set_text(state, text::format_float(report.attacker_big_losses, 0));
		left_smallship_casualties_text->set_text(state, text::format_float(report.attacker_small_losses, 0));
		left_transship_casualties_text->set_text(state, text::format_float(report.attacker_transport_losses, 0));	// We stand for the Thin Pink Line

		right_bigship_casualties_text->set_text(state, text::format_float(report.defender_big_losses, 0));
		right_smallship_casualties_text->set_text(state, text::format_float(report.defender_small_losses, 0));
		right_transship_casualties_text->set_text(state, text::format_float(report.defender_transport_losses, 0));

		//============================================================================================

		left_bigship_survivors_text->set_text(state, text::format_float(float(report.attacker_big_ships - report.attacker_big_losses), 0));
		left_smallship_survivors_text->set_text(state, text::format_float(float(report.attacker_small_ships - report.attacker_small_losses), 0));
		left_transship_survivors_text->set_text(state, text::format_float(float(report.attacker_transport_ships - report.attacker_transport_losses), 0));

		right_bigship_survivors_text->set_text(state, text::format_float(float(report.defender_big_ships - report.defender_big_losses), 0));
		right_smallship_survivors_text->set_text(state, text::format_float(float(report.defender_small_ships - report.defender_small_losses), 0));
		right_transship_survivors_text->set_text(state, text::format_float(float(report.defender_transport_ships - report.defender_transport_losses), 0));

		//============================================================================================

		left_total_initial_text->set_text(state, text::format_float(
					float(report.attacker_big_ships + report.attacker_small_ships + report.attacker_transport_ships), 0));
		left_total_lost_text->set_text(state, text::format_float(
					float(report.attacker_big_losses + report.attacker_small_losses + report.attacker_transport_losses), 0));
		left_total_left_text->set_text(state, text::format_float(float(
					(report.attacker_big_ships - report.attacker_big_losses) +
					(report.attacker_small_ships - report.attacker_small_losses) +
					(report.attacker_transport_ships - report.attacker_transport_losses)
					), 0));
		right_total_initial_text->set_text(state, text::format_float(
					float(report.defender_big_ships + report.defender_small_ships + report.defender_transport_ships), 0));
		right_total_lost_text->set_text(state, text::format_float(
					float(report.defender_big_losses + report.defender_small_losses + report.defender_transport_losses), 0));
		right_total_left_text->set_text(state, text::format_float(float(
					(report.defender_big_ships - report.defender_big_losses) +
					(report.defender_small_ships - report.defender_small_losses) +
					(report.defender_transport_ships - report.defender_transport_losses)
					), 0));

		//============================================================================================
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<element_selection_wrapper<bool>>()) {
			auto content = any_cast<element_selection_wrapper<bool>>(payload).data;
			if(content) {
					auto root = static_cast<ui::container_base*>(state.ui_state.root.get());
					if(auto result = std::find_if(root->children.begin(), root->children.end(), [this](std::unique_ptr<element_base>& p){return p.get() == this;});
							result != root->children.end()) {
						if(result + 1 != children.end()) { std::rotate(result, result+1, root->children.end());}

						auto temp = std::move(root->children.back());
						root->children.pop_back();
						set_visible(state, false);
						state.ui_state.endof_navalcombat_windows.push_back(std::move(temp));
					} else {
						std::abort();	// Failed to move the naavl combat window back out
					}
			} else {
				// TODO - We dont handle this, yet
			}
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

} // namespace ui
