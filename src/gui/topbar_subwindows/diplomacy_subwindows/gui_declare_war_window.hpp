#pragma once

#include "gui_element_types.hpp"

namespace ui {

class wargoal_add_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_gain_effect_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "prestige_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "prestige") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "infamy_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "infamy") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "militancy_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "militancy") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

class wargoal_success_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_gain_effect_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "prestige_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "prestige") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "infamy_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "infamy") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "militancy_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "militancy") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

class wargoal_failure_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_gain_effect_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "prestige_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "prestige") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "infamy_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "infamy") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "militancy_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "militancy") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

class wargoal_setup_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_list") {
			// TODO - Listbox here
			return nullptr;
		} else {
			return nullptr;
		}
	}
};

class wargoal_state_select : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "state_list") {
			// TODO - Listbox here
			return nullptr;
		} else if(name == "cancel_select") {
			return make_element_by_type<button_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

class wargoal_country_select : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "country_list") {
			// TODO - Listbox here
			return nullptr;
		} else if(name == "cancel_select") {
			return make_element_by_type<button_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

/*class wargoal_item_listboxrow : public listbox_row_element_base<bool> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "select_goal_invalid") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "select_goal") {
			return make_element_by_type<button_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};*/

class wargoal_type_item : public listbox_row_element_base<dcon::cb_type_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->base_data.position.x += 16;
			return ptr;
		} else if(name == "select_goal_invalid") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->base_data.position.x += 16;
			return ptr;
		} else if(name == "select_goal") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->base_data.position.x += 16;
			return ptr;
		} else {
			return nullptr;
		}
	}
};	// Done by Leaf

class wargoal_type_listbox : public listbox_element_base<wargoal_type_item, dcon::cb_type_id> {
protected:
	std::string_view get_row_element_name() override {
		return "wargoal_item";
	}
public:
	void on_create(sys::state& state) noexcept override {
		listbox_element_base::on_create(state);
		row_contents.clear();
		state.world.for_each_cb_type([&](dcon::cb_type_id id) {
			row_contents.push_back(id);
		});
		update(state);
	}
};	// Done by Leaf

class wargoal_state_listboxrow : public listbox_row_element_base<bool> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "select_state") {
			return make_element_by_type<button_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

class wargoal_country_listboxrow : public listbox_row_element_base<bool> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "select_country") {
			return make_element_by_type<button_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

class diplomacy_declare_war_dialog : public window_element_base {			// eu3dialogtype
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "background") {
			auto ptr = make_element_by_type<draggable_target>(state, id);
			ptr->base_data.size = base_data.size; // Nudge size for proper sizing
			return ptr;
		} else if(name == "diplo_declarewar_bg") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "leftshield") {
			return make_element_by_type<nation_player_flag>(state, id);
		} else if(name == "rightshield") {
			return make_element_by_type<flag_button>(state, id);
		} else if(name == "title") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "description") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "wargoal_add_effect") {
			return make_element_by_type<wargoal_add_window>(state, id);
		} else if(name == "wargoal_success_effect") {
			return make_element_by_type<wargoal_success_window>(state, id);
		} else if(name == "wargoal_failure_effect") {
			return make_element_by_type<wargoal_failure_window>(state, id);
		} else if(name == "description2") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "acceptance") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "call_allies_checkbox") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "call_allies_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "agreebutton") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "declinebutton") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "wargoal_setup") {
			return make_element_by_type<wargoal_type_listbox>(state, id);
		} else if(name == "wargoal_state_select") {
			// TODO - Listbox here
			return nullptr;
		} else if(name == "wargoal_country_select") {
			// TODO - Listbox here
			return nullptr;
		} else {
			return nullptr;
		}
	}
};

class pick_side_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "war_desc") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "offer_button") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "offer_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "demand_button") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "demand_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

class setup_goals_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_list") {
			// TODO - Listbox here
			return nullptr;
		} else if(name == "score") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "acceptance") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

class diplomacy_setup_peace_dialog : public window_element_base {			// eu3dialogtype
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "background") {
			auto ptr = make_element_by_type<draggable_target>(state, id);
			ptr->base_data.size = base_data.size; // Nudge size for proper sizing
			return ptr;
		} else if(name == "diplo_peace_bg") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "leftshield") {
			return make_element_by_type<nation_player_flag>(state, id);
		} else if(name == "rightshield") {
			return make_element_by_type<flag_button>(state, id);
		} else if(name == "title") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "warscore_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "warscore") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "agreebutton") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "declinebutton") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "pick_side") {
			return make_element_by_type<pick_side_window>(state, id);
		} else if(name == "setup_goals") {
			return make_element_by_type<setup_goals_window>(state, id);
		} else {
			return nullptr;
		}
	}
};

class peace_goal_row : public listbox_row_element_base<bool> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "select") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "country_flag") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "country_flag2") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "cb_direction_arrow") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "select_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "score") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

class diplomacy_make_cb_window : public window_element_base {			// eu3dialogtype
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "background") {
			auto ptr = make_element_by_type<draggable_target>(state, id);
			ptr->base_data.size = base_data.size; // Nudge size for proper sizing
			return ptr;
		} else if(name == "diplo_makecb_bg") {
			return make_element_by_type<image_element_base>(state,id);
		} else if(name == "title") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "description") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "leftshield") {
			return make_element_by_type<nation_player_flag>(state, id);
		} else if(name == "rightshield") {
			return make_element_by_type<flag_button>(state, id);
		} else if(name == "agreebutton") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "declinebutton") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "cb_list") {
			// TODO - Listbox here
			return nullptr;
		} else {
			return nullptr;
		}
	}
};

class cb_type_row : public listbox_row_element_base<bool> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "select_cb") {
			return make_element_by_type<button_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

class setup_crisis_pick_side_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "crisis_desc") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "offer_button") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "offer_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "demand_button") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "demand_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

class setup_crisis_setup_goals_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_list") {
			// TODO - Listbox here
		} else if(name == "acceptance") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

class setup_crisis_backdown_window : public window_element_base {		// eu3dialogtype
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "background") {
			auto ptr = make_element_by_type<draggable_target>(state, id);
			ptr->base_data.size = base_data.size; // Nudge size for proper sizing
			return ptr;
		} else if(name == "diplo_peace_bg") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "leftshield") {
			return make_element_by_type<nation_player_flag>(state, id);
		} else if(name == "rightshield") {
			return make_element_by_type<flag_button>(state, id);
		} else if(name == "title") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "agreebutton") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "declinebutton") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "pick_side") {
			return make_element_by_type<setup_crisis_pick_side_window>(state, id);
		} else if(name == "setup_goals") {
			return make_element_by_type<setup_crisis_setup_goals_window>(state, id);
		} else {
			return nullptr;
		}
	}
};

}
