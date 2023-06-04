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

}
