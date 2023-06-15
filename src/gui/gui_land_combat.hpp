#pragma once

#include "gui_element_types.hpp"

namespace ui {

class land_combat_defender_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "shield") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "leader_icon") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "leader_name") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "morale") {
			return make_element_by_type<progress_bar>(state, id);

		} else if(name == "strength") {
			return make_element_by_type<progress_bar>(state, id);

		} else if(name == "morale_text") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "strength_text") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "unit_type_1") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "unit_type_2") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "unit_type_3") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "unit_type_1_value") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "unit_type_2_value") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "unit_type_3_value") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else {
			return nullptr;
		}
	}
};

class land_combat_attacker_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "shield") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "leader_icon") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "leader_name") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "morale") {
			return make_element_by_type<progress_bar>(state, id);

		} else if(name == "strength") {
			return make_element_by_type<progress_bar>(state, id);

		} else if(name == "morale_text") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "strength_text") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "unit_type_1") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "unit_type_2") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "unit_type_3") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "unit_type_1_value") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "unit_type_2_value") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "unit_type_3_value") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else {
			return nullptr;
		}
	}
};

class land_combat_window : public window_element_base {
	image_element_base* land_combat_may_retreat = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "combat_bg") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "combat_terrain1") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "combat_counter_bg") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "label_battlename") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "combat_may_retreat") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			land_combat_may_retreat = ptr.get();
			return ptr;

		} else if(name == "closebutton") {
			return make_element_by_type<generic_close_button>(state, id);

		} else if(name == "attacker") {
			return make_element_by_type<land_combat_attacker_window>(state, id);

		} else if(name == "defender") {
			return make_element_by_type<land_combat_defender_window>(state, id);

		} else {
			return nullptr;
		}
	}
};

//===============================================================================================================================

class land_combat_end_popup : public window_element_base {
private:
	simple_text_element_base* winorlose_text = nullptr;
	simple_text_element_base* warscore_text = nullptr;
public:
	void on_update(sys::state& state) noexcept override {
		winorlose_text->set_text(state, "UwU");
		warscore_text->set_text(state, "69");
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "background") {
			auto ptr = make_element_by_type<draggable_target>(state, id);
			//ptr->base_data.size = base_data.size; // Nudge
			return ptr;
		} else if(name == "combat_end_naval_lost") { 
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "combat_end_naval_won") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "line1") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "ourleader_photo") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "ourleader") {	// Our great leader, the leader of the universe, Puffy, Puffy the cat, tremble in her might
			return make_element_by_type<simple_text_element_base>(state, id);
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
			return make_element_by_type<simple_text_element_base>(state, id);
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
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "our_unit_type_1_1_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "our_unit_type_1_2_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "our_unit_type_1_3_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "our_unit_type_2") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "our_unit_type_2_1_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "our_unit_type_2_2_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "our_unit_type_2_3_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "our_unit_type_3") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "our_unit_type_3_1_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "our_unit_type_3_2_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "our_unit_type_3_3_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "enemy_unit_type_1") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "enemy_unit_type_1_1_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "enemy_unit_type_1_2_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "enemy_unit_type_1_3_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "enemy_unit_type_2") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "enemy_unit_type_2_1_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "enemy_unit_type_2_2_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "enemy_unit_type_2_3_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "enemy_unit_type_3") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "enemy_unit_type_3_1_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "enemy_unit_type_3_2_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "enemy_unit_type_3_3_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "our_total_armies") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "our_total_loss") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "our_total_left") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "enemy_total_armies") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "enemy_total_loss") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "enemy_total_left") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "agreebutton") {
			auto ptr = make_element_by_type<generic_close_button>(state, id);
			ptr->base_data.position.y += 146;	// Nudge
			return ptr;
		} else if(name == "declinebutton") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->base_data.position.y += 146;	// Nudge
			ptr->set_button_text(state, text::produce_simple_string(state, "landbattleover_btn3"));
			return ptr;
		} else if(name == "centerok") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->set_visible(state, false);	// Disacrd
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
};

} // namespace ui
