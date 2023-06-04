#pragma once

#include <gui_element_types.hpp>

namespace ui {

class naval_combat_ship_lslot : public window_element_base {
	image_element_base* nav_icon_norm = nullptr;
	image_element_base* nav_icon_attack = nullptr;
	image_element_base* nav_icon_sunk = nullptr;
	image_element_base* nav_icon_retreat = nullptr;
	image_element_base* nav_icon_disengaged = nullptr;
	image_element_base* nav_icon_seeking = nullptr;
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "attacker_icon_normal") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			nav_icon_normal = ptr.get();
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
			return make_element_by_type<vertical_progress_bar>(state, id);

		} else if(name == "org") {
			return make_element_by_type<vertical_progress_bar>(state, id);

		} else if(name == "mini_frame_str") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "mini_frame_org") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "torpedo") {
			return make_element_by_type<image_element_base>(state, id);

		} else {
			return nullptr;
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
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "attacker_icon_normal") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			nav_icon_normal = ptr.get();
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
			return make_element_by_type<vertical_progress_bar>(state, id);

		} else if(name == "org") {
			return make_element_by_type<vertical_progress_bar>(state, id);

		} else if(name == "mini_frame_str") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "mini_frame_org") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "torpedo") {
			return make_element_by_type<image_element_base>(state, id);

		} else {
			return nullptr;
		}
	}
};

class naval_combat_ship_unit : public listbox_row_element_base<bool> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "bg") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "highlight") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "left_slot") {
			return make_element_by_type<naval_combat_ship_lslot>(state, id);

		} else if(name == "right_slot") {
			return make_element_by_type<naval_combat_ship_rslot>(state, id);

		} else if(name == "attacker_name") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "defender_name") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "attacker_flag") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "defender_flag") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "status") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "range_r") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "range_l") {
			return make_element_by_type<image_element_base>(state, id);

		} else {
			return nullptr;
		}
	}
};

class naval_slots_listbox : public listbox_element_base<naval_combat_ship_unit, bool> {
protected:
	std::string_view get_row_element_name() override {
		return "ship_unit";
	}
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			row_contents.clear();
			// TODO - here we want to populate the listbox with all of the attackers/defenders in the battle
			update(state);
		}
	}
};

class naval_combat_attacker_window : public window_element_base {
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

		} else if(name == "morale_frame") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "strength_frame") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "big_ship_icon") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "small_ship_icon") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "trade_ship_icon") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "big_ship_value") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "small_ship_value") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "trade_ship_value") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "slots_list") {
			return make_element_by_type<naval_slots_listbox>(state, id);

		} else {
			return nullptr;
		}
	}
};

class naval_combat_defender_window : public window_element_base {
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

		} else if(name == "morale_frame") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "strength_frame") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "big_ship_icon") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "small_ship_icon") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "trade_ship_icon") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "big_ship_value") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "small_ship_value") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "trade_ship_value") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "slots_list") {
			return make_element_by_type<naval_slots_listbox>(state, id);

		} else {
			return nullptr;
		}
	}
};

class naval_combat_window : public window_element_base {
public:
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
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "combat_may_retreat") {
			return make_element_by_type<image_element_base>(state, id);

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
};

}
