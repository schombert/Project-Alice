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

/*
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

class wargoal_item_listboxrow : public listbox_row_element_base<bool> {
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

//====================================================================================================================================

class wargoal_type_item_icon : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::cb_type_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::cb_type_id>(payload);
			frame = (dcon::fatten(state.world, content).get_sprite_index() - 1);
		}
	}
};

class wargoal_type_item_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::cb_type_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::cb_type_id>(payload);

			Cyto::Any newpayload = element_selection_wrapper<dcon::cb_type_id>{content};
			parent->impl_get(state, newpayload);
		}
	}
};

class wargoal_type_item : public listbox_row_element_base<dcon::cb_type_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_icon") {
			auto ptr = make_element_by_type<wargoal_type_item_icon>(state, id);
			ptr->base_data.position.x += 16;	// Nudge
			return ptr;
		} else if(name == "select_goal_invalid") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->base_data.position.x += 16;	// Nudge
			return ptr;
		} else if(name == "select_goal") {
			auto ptr = make_element_by_type<wargoal_type_item_button>(state, id);
			ptr->base_data.position.x += 16;	// Nudge
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
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			row_contents.clear();

			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			auto one_cbs = state.world.nation_get_available_cbs(state.local_player_nation);
			for(auto& cb : one_cbs) {
				if(cb.target == content && cb.expiration >= state.current_date) {
					row_contents.push_back(cb.cb_type);
				}
			}

			update(state);
		}
	}
};

class wargoal_setup_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_list") {
			return make_element_by_type<wargoal_type_listbox>(state, id);
		} else {
			return nullptr;
		}
	}
};

//====================================================================================================================================

class wargoal_state_item_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::state_definition_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::state_definition_id>(payload);

			Cyto::Any newpayload = element_selection_wrapper<dcon::state_definition_id>{content};
			parent->impl_get(state, newpayload);
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::state_definition_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::state_definition_id>(payload);
			auto fat = dcon::fatten(state.world, content);

			set_button_text(state, text::produce_simple_string(state, fat.get_name()));
		}
	}
};

class wargoal_state_item : public listbox_row_element_base<dcon::state_definition_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "select_state") {
			return make_element_by_type<wargoal_state_item_button>(state, id);
		} else {
			return nullptr;
		}
	}
};

class wargoal_state_listbox : public listbox_element_base<wargoal_state_item, dcon::state_definition_id> {
protected:
	std::string_view get_row_element_name() override {
		return "wargoal_state_item";
	}
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			row_contents.clear();
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);
			auto fat = dcon::fatten(state.world, content);

			for(auto staat : fat.get_state_ownership()) {
				row_contents.push_back(dcon::fatten(state.world, staat).get_state().get_definition().id);
			}

			update(state);
		}
	}
};

class wargoal_state_select_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "state_list") {
			return make_element_by_type<wargoal_state_listbox>(state, id);
		} else if(name == "cancel_select") {
			return make_element_by_type<button_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

//====================================================================================================================================

class wargoal_country_item_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::national_identity_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::national_identity_id>(payload);

			Cyto::Any newpayload = element_selection_wrapper<dcon::national_identity_id>{content};
			parent->impl_get(state, newpayload);
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::national_identity_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::national_identity_id>(payload);

			set_button_text(state, text::produce_simple_string(state, dcon::fatten(state.world, content).get_name()));
		}
	}
};

class wargoal_country_item : public listbox_row_element_base<dcon::national_identity_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "select_country") {
			return make_element_by_type<wargoal_country_item_button>(state, id);
		} else {
			return nullptr;
		}
	}
};

class wargoal_country_listbox : public listbox_element_base<wargoal_country_item, dcon::national_identity_id> {
protected:
	std::string_view get_row_element_name() override {
		return "wargoal_country_item";
	}
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			state.world.for_each_national_identity([&](dcon::national_identity_id ident) {
				if(nations::can_release_as_vassal(state, content, ident)) {
					row_contents.push_back(ident);
				}
			});

			row_contents.clear();
			update(state);
		}
	}
};

class wargoal_country_select_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "country_list") {
			return make_element_by_type<wargoal_country_listbox>(state, id);
		} else if(name == "cancel_select") {
			return make_element_by_type<button_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

//====================================================================================================================================

class diplomacy_declare_war_agree_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any cb_payload = dcon::cb_type_id{};
			parent->impl_get(state, cb_payload);
			auto fat_cb = dcon::fatten(state.world, any_cast<dcon::cb_type_id>(cb_payload));
			Cyto::Any state_payload = dcon::state_definition_id{};
			parent->impl_get(state, state_payload);
			auto fat_state = dcon::fatten(state.world, any_cast<dcon::state_definition_id>(state_payload));
			Cyto::Any nation_payload = dcon::national_identity_id{};
			parent->impl_get(state, nation_payload);
			auto fat_nation = dcon::fatten(state.world, any_cast<dcon::cb_type_id>(nation_payload));
		}
	}
};

class diplomacy_declare_war_dialog : public window_element_base {			// eu3dialogtype
private:
	dcon::cb_type_id cb_to_use{};
	dcon::state_definition_id target_state{};
	dcon::national_identity_id target_country{};
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
			return make_element_by_type<wargoal_setup_window>(state, id);

		} else if(name == "wargoal_state_select") {
			return make_element_by_type<wargoal_state_select_window>(state, id);

		} else if(name == "wargoal_country_select") {
			return make_element_by_type<wargoal_country_select_window>(state, id);

		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<element_selection_wrapper<dcon::cb_type_id>>()) {
			cb_to_use = any_cast<element_selection_wrapper<dcon::cb_type_id>>(payload).data;
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<dcon::state_definition_id>>()) {
			target_state = any_cast<element_selection_wrapper<dcon::state_definition_id>>(payload).data;
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<dcon::national_identity_id>()) {
			target_country = any_cast<element_selection_wrapper<dcon::national_identity_id>>(payload).data;
			return message_result::consumed;
		}
		//=======================================================================================================
		else if(payload.holds_type<dcon::cb_type_id>>()) {
			payload.emplace<dcon::cb_type_id>(cb_to_use);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::state_definition_id>>()) {
			payload.emplace<dcon::state_definition_id>(target_state);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::national_identity_id>>()) {
			payload.emplace<dcon::national_identity_id>(target_country);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

//====================================================================================================================================

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

class cb_wargoal_icon : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::cb_type_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::cb_type_id>(payload);
			frame = (dcon::fatten(state.world, content).get_sprite_index() - 1);
		}
	}
};

class cb_wargoal_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::cb_type_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::cb_type_id>(payload);
			set_button_text(state, text::produce_simple_string(state, dcon::fatten(state.world, content).get_name()));
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::cb_type_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::cb_type_id>(payload);

			Cyto::Any newpayload = element_selection_wrapper<dcon::cb_type_id>{content};
			parent->impl_get(state, newpayload);
		}
	}
};

class cb_type_row : public listbox_row_element_base<dcon::cb_type_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_icon") {
			return make_element_by_type<cb_wargoal_icon>(state, id);
		} else if(name == "select_cb") {
			return make_element_by_type<cb_wargoal_button>(state, id);
		} else {
			return nullptr;
		}
	}
};

class cb_listbox : public listbox_element_base<cb_type_row, dcon::cb_type_id> {
protected:
	std::string_view get_row_element_name() override {
		return "cb_type_item";
	}
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			row_contents.clear();
			state.world.for_each_cb_type([&](dcon::cb_type_id cb) {
				//if(military::cb_conditions_satisfied(state, state.local_player_nation, content, cb) && dcon::fatten(state.world, cb).is_valid()) {
				if(command::can_fabricate_cb(state, state.local_player_nation, content, cb) && dcon::fatten(state.world, cb).is_valid()) {
					row_contents.push_back(cb);
				}
			});
			update(state);
		}
	}
};

class make_cb_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::cb_type_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::cb_type_id>(payload);

			Cyto::Any newpayload = dcon::nation_id{};
			parent->impl_get(state, newpayload);
			auto target_nation = any_cast<dcon::nation_id>(newpayload);

			command::fabricate_cb(state, state.local_player_nation, target_nation, content);
			parent->set_visible(state, false);
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::cb_type_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::cb_type_id>(payload);

			Cyto::Any newpayload = dcon::nation_id{};
			parent->impl_get(state, newpayload);
			auto target_nation = any_cast<dcon::nation_id>(newpayload);

			disabled = !command::can_fabricate_cb(state, state.local_player_nation, target_nation, content);
		}
	}
};

class make_cb_desc : public generic_multiline_text<dcon::cb_type_id> {
public:
	void populate_layout(sys::state& state, text::endless_layout& contents, dcon::cb_type_id id) noexcept override {
		auto fat_cb = dcon::fatten(state.world, id);

		auto box = text::open_layout_box(contents);

		if(fat_cb.is_valid()) {
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::type, fat_cb.get_name());
			text::add_to_substitution_map(sub, text::variable_type::days, (fat_cb.get_months() * 30));
			text::add_to_substitution_map(sub, text::variable_type::badboy, text::fp_two_places{fat_cb.get_badboy_factor()});
			text::localised_format_box(state, contents, box, std::string_view("cb_creation_detail"), sub);
		}

		text::close_layout_box(contents, box);
	}
};

class make_cb_title : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::produce_simple_string(state, "make_cbtitle"));
	}
};

class diplomacy_make_cb_window : public window_element_base {			// eu3dialogtype
private:
	dcon::cb_type_id root_cb{};
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "background") {
			auto ptr = make_element_by_type<draggable_target>(state, id);
			ptr->base_data.size = base_data.size; // Nudge size for proper sizing
			return ptr;
		} else if(name == "diplo_makecb_bg") {
			return make_element_by_type<image_element_base>(state,id);
		} else if(name == "title") {
			return make_element_by_type<make_cb_title>(state, id);
		} else if(name == "description") {
			return make_element_by_type<make_cb_desc>(state, id);
		} else if(name == "leftshield") {
			return make_element_by_type<nation_player_flag>(state, id);
		} else if(name == "rightshield") {
			return make_element_by_type<flag_button>(state, id);
		} else if(name == "agreebutton") {
			return make_element_by_type<make_cb_button>(state, id);
		} else if(name == "declinebutton") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "cb_list") {
			return make_element_by_type<cb_listbox>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<element_selection_wrapper<dcon::cb_type_id>>()) {
			root_cb = any_cast<element_selection_wrapper<dcon::cb_type_id>>(payload).data;
			impl_on_update(state);
			return message_result::consumed;
		}
		//=====================================================================================
		else if(payload.holds_type<dcon::cb_type_id>()) {
			payload.emplace<dcon::cb_type_id>(root_cb);
			return message_result::consumed;
		}
		return message_result::unseen;
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
		}
		return nullptr;
	}
};

class diplomacy_crisis_backdown_window : public window_element_base {		// eu3dialogtype
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
