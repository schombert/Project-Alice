#pragma once

#include "gui_element_types.hpp"

namespace ui {

class wargoal_cancel_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<dcon::cb_type_id>{};
			parent->impl_get(state, payload);
		}
	}
};

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
			dcon::cb_type_id content = any_cast<dcon::cb_type_id>(payload);
			Cyto::Any c_payload = element_selection_wrapper<dcon::cb_type_id>{content};
			parent->impl_get(state, c_payload);
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::cb_type_id{};
			parent->impl_get(state, payload);
			dcon::cb_type_id content = any_cast<dcon::cb_type_id>(payload);
			auto fat = dcon::fatten(state.world, content);
			set_button_text(state, text::produce_simple_string(state, fat.get_name()));
		}
	}
};

class wargoal_type_item : public listbox_row_element_base<dcon::cb_type_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_icon") {
			auto ptr = make_element_by_type<wargoal_type_item_icon>(state, id);
			ptr->base_data.position.x += 16; // Nudge
			return ptr;
		} else if(name == "select_goal_invalid") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->base_data.position.x += 16; // Nudge
			return ptr;
		} else if(name == "select_goal") {
			auto ptr = make_element_by_type<wargoal_type_item_button>(state, id);
			ptr->base_data.position.x += 16; // Nudge
			return ptr;
		} else {
			return nullptr;
		}
	}
}; // Done by Leaf

class wargoal_type_listbox : public listbox_element_base<wargoal_type_item, dcon::cb_type_id> {
protected:
	std::string_view get_row_element_name() override { return "wargoal_item"; }

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			dcon::nation_id content = any_cast<dcon::nation_id>(payload);
			if(military::are_at_war(state, state.local_player_nation, content)) {
				state.world.for_each_cb_type([&](dcon::cb_type_id cbt) {
					bool is_valid = false;
					dcon::war_id w = military::find_war_between(state, state.local_player_nation, content);
					state.world.nation_for_each_state_ownership_as_nation(content, [&](dcon::state_ownership_id soi) {
						dcon::state_definition_id sdef =
							state.world.state_instance_get_definition(state.world.state_ownership_get_state(soi));
						state.world.for_each_national_identity([&](dcon::national_identity_id nid) {
							if(command::can_add_war_goal(state, state.local_player_nation, w, content, cbt, sdef, nid,
								   state.world.national_identity_get_nation_from_identity_holder(nid)))
								is_valid = true;
						});
					});
					if(is_valid)
						row_contents.push_back(cbt);
				});
			} else {
				for(auto& cb : state.world.nation_get_available_cbs(state.local_player_nation))
					if(cb.target == content && cb.expiration >= state.current_date)
						row_contents.push_back(cb.cb_type);
			}
		}
		update(state);
	}
};

class wargoal_setup_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_list") {
			return make_element_by_type<wargoal_type_listbox>(state, id);
		} else if(name == "cancel_select") {
			return make_element_by_type<wargoal_cancel_button>(state, id);
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
			dcon::state_definition_id content = any_cast<dcon::state_definition_id>(payload);
			Cyto::Any newpayload = element_selection_wrapper<dcon::state_definition_id>{content};
			parent->impl_get(state, newpayload);
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::state_definition_id{};
			parent->impl_get(state, payload);
			dcon::state_definition_id content = any_cast<dcon::state_definition_id>(payload);
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
	std::string_view get_row_element_name() override { return "wargoal_state_item"; }

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			dcon::nation_id n = any_cast<dcon::nation_id>(payload);
			Cyto::Any c_payload = dcon::cb_type_id{};
			parent->impl_get(state, c_payload);
			dcon::cb_type_id c = any_cast<dcon::cb_type_id>(c_payload);
			dcon::trigger_key allowed_states = state.world.cb_type_get_allowed_states(c);
			for(auto si : dcon::fatten(state.world, n).get_state_ownership()) {
				auto fat_id = dcon::fatten(state.world, si);
				if(trigger::evaluate(state, allowed_states, trigger::to_generic(fat_id.get_state().id),
					   trigger::to_generic(state.local_player_nation), trigger::to_generic(state.local_player_nation)))
					row_contents.push_back(fat_id.get_state().get_definition().id);
			}
		}
		update(state);
	}
};

class wargoal_state_select_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "state_list") {
			return make_element_by_type<wargoal_state_listbox>(state, id);
		} else if(name == "cancel_select") {
			return make_element_by_type<wargoal_cancel_button>(state, id);
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
			dcon::national_identity_id content = any_cast<dcon::national_identity_id>(payload);
			Cyto::Any newpayload = element_selection_wrapper<dcon::national_identity_id>{content};
			parent->impl_get(state, newpayload);
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::national_identity_id{};
			parent->impl_get(state, payload);
			dcon::national_identity_id content = any_cast<dcon::national_identity_id>(payload);
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
	std::string_view get_row_element_name() override { return "wargoal_country_item"; }

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			dcon::nation_id content = any_cast<dcon::nation_id>(payload);
			Cyto::Any c_payload = dcon::cb_type_id{};
			parent->impl_get(state, c_payload);
			dcon::cb_type_id c = any_cast<dcon::cb_type_id>(c_payload);
			dcon::trigger_key allowed_countries = state.world.cb_type_get_allowed_countries(c);
			state.world.for_each_nation([&](dcon::nation_id n) {
				if(trigger::evaluate(state, allowed_countries, trigger::to_generic(content),
					   trigger::to_generic(state.local_player_nation), trigger::to_generic(n)))
					row_contents.push_back(
						state.world.identity_holder_get_identity(state.world.nation_get_identity_holder_as_nation(n)));
			});
		}
		update(state);
	}
};

class wargoal_country_select_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "country_list") {
			return make_element_by_type<wargoal_country_listbox>(state, id);
		} else if(name == "cancel_select") {
			return make_element_by_type<wargoal_cancel_button>(state, id);
		} else {
			return nullptr;
		}
	}
};

//====================================================================================================================================

class diplomacy_wargoal_add_window : public window_element_base {
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

class diplomacy_wargoal_success_window : public window_element_base {
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

class diplomacy_wargoal_failure_window : public window_element_base {
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

class diplomacy_declare_war_title : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override { set_text(state, text::produce_simple_string(state, "wartitle")); }
};

class diplomacy_declare_war_agree_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			dcon::nation_id n = any_cast<dcon::nation_id>(payload);
			Cyto::Any s_payload = dcon::state_definition_id{};
			parent->impl_get(state, s_payload);
			dcon::state_definition_id s = any_cast<dcon::state_definition_id>(s_payload);
			Cyto::Any n_payload = dcon::national_identity_id{};
			parent->impl_get(state, n_payload);
			dcon::national_identity_id ni = any_cast<dcon::national_identity_id>(n_payload);
			Cyto::Any c_payload = dcon::cb_type_id{};
			parent->impl_get(state, c_payload);
			dcon::cb_type_id c = any_cast<dcon::cb_type_id>(c_payload);

			if(military::are_at_war(state, state.local_player_nation, n)) {
				dcon::war_id w = military::find_war_between(state, state.local_player_nation, n);
				disabled = !command::can_add_war_goal(state, state.local_player_nation, w, n, c, s, ni,
					state.world.national_identity_get_nation_from_identity_holder(ni));
			} else {
				disabled = !command::can_declare_war(state, state.local_player_nation, n, c, s, ni,
					state.world.national_identity_get_nation_from_identity_holder(ni));
			}
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			dcon::nation_id n = any_cast<dcon::nation_id>(payload);
			Cyto::Any s_payload = dcon::state_definition_id{};
			parent->impl_get(state, s_payload);
			dcon::state_definition_id s = any_cast<dcon::state_definition_id>(s_payload);
			Cyto::Any n_payload = dcon::national_identity_id{};
			parent->impl_get(state, n_payload);
			dcon::national_identity_id ni = any_cast<dcon::national_identity_id>(n_payload);
			Cyto::Any c_payload = dcon::cb_type_id{};
			parent->impl_get(state, c_payload);
			dcon::cb_type_id c = any_cast<dcon::cb_type_id>(c_payload);

			if(military::are_at_war(state, state.local_player_nation, n)) {
				dcon::war_id w = military::find_war_between(state, state.local_player_nation, n);
				command::add_war_goal(state, state.local_player_nation, w, n, c, s, ni,
					state.world.national_identity_get_nation_from_identity_holder(ni));
			} else {
				command::declare_war(state, state.local_player_nation, n, c, s, ni,
					state.world.national_identity_get_nation_from_identity_holder(ni));
			}
			parent->set_visible(state, false);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override { return tooltip_behavior::variable_tooltip; }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			dcon::nation_id n = any_cast<dcon::nation_id>(payload);
			Cyto::Any s_payload = dcon::state_definition_id{};
			parent->impl_get(state, s_payload);
			dcon::state_definition_id s = any_cast<dcon::state_definition_id>(s_payload);
			Cyto::Any n_payload = dcon::national_identity_id{};
			parent->impl_get(state, n_payload);
			dcon::national_identity_id ni = any_cast<dcon::national_identity_id>(n_payload);
			Cyto::Any c_payload = dcon::cb_type_id{};
			parent->impl_get(state, c_payload);
			dcon::cb_type_id c = any_cast<dcon::cb_type_id>(c_payload);

			auto box = text::open_layout_box(contents, 0);
			if(command::can_declare_war(state, state.local_player_nation, n, c, s, ni,
				   state.world.national_identity_get_nation_from_identity_holder(ni))) {
				text::localised_format_box(state, contents, box, std::string_view("valid_wartarget"));
			} else {
				if(military::are_allied_in_war(state, state.local_player_nation, n)) {
					text::localised_format_box(state, contents, box, std::string_view("invalid_wartarget_shared_war"));
				}
				auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(state.local_player_nation, n);
				if(state.world.diplomatic_relation_get_are_allied(rel)) {
					text::localised_format_box(state, contents, box, std::string_view("no_war_allied"));
				}
			}
			text::close_layout_box(contents, box);
		}
	}
};

class diplomacy_declare_war_description1 : public generic_multiline_text<dcon::cb_type_id> {
public:
	void populate_layout(sys::state& state, text::endless_layout& contents, dcon::cb_type_id id) noexcept override {
		auto fat_cb = dcon::fatten(state.world, id);
		auto box = text::open_layout_box(contents);

		text::substitution_map sub1;
		text::add_to_substitution_map(sub1, text::variable_type::goal, fat_cb.get_name());

		text::substitution_map sub2;
		auto str = text::produce_simple_string(state, "nocasusbelli");
		text::add_to_substitution_map(sub2, text::variable_type::goal, std::string_view(str));

		if(fat_cb.is_valid()) {
			text::localised_format_box(state, contents, box, std::string_view("add_wargoal_wargoal"), sub1);
		} else {
			text::localised_format_box(state, contents, box, std::string_view("add_wargoal_wargoal"), sub2);
		}

		text::close_layout_box(contents, box);
	}
};

class diplomacy_declare_war_description2 : public generic_multiline_text<dcon::cb_type_id> {
public:
	void populate_layout(sys::state& state, text::endless_layout& contents, dcon::cb_type_id id) noexcept override {
		Cyto::Any payload = dcon::nation_id{};
		parent->impl_get(state, payload);
		auto content = any_cast<dcon::nation_id>(payload);

		Cyto::Any newpayload = dcon::state_definition_id{};
		parent->impl_get(state, newpayload);
		auto staat = any_cast<dcon::state_definition_id>(newpayload);

		Cyto::Any newnewpayload = dcon::national_identity_id{};
		parent->impl_get(state, newnewpayload);
		auto nacion = any_cast<dcon::national_identity_id>(newnewpayload);

		auto fat_cb = dcon::fatten(state.world, id);

		auto box = text::open_layout_box(contents);

		if(fat_cb.is_valid()) {
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::recipient,
				dcon::fatten(state.world, content).get_name()); // Target Nation
			text::add_to_substitution_map(sub, text::variable_type::third,
				dcon::fatten(state.world, nacion).get_name()); // Third Party Country
			text::add_to_substitution_map(sub, text::variable_type::actor, dcon::fatten(state.world, nacion).get_name());
			text::add_to_substitution_map(sub, text::variable_type::state, dcon::fatten(state.world, staat).get_name());
			text::add_to_substitution_map(sub, text::variable_type::region, dcon::fatten(state.world, staat).get_name());

			text::add_to_layout_box(contents, state, box, fat_cb.get_long_desc(), sub);
		}

		text::close_layout_box(contents, box);
	}
};

class diplomacy_declare_war_call_allies_checkbox : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = bool{};
			parent->impl_get(state, payload);
			bool content = any_cast<bool>(payload);
			Cyto::Any b_payload = element_selection_wrapper<bool>{!content};
			parent->impl_get(state, b_payload);
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = bool{};
			parent->impl_get(state, payload);
			bool content = any_cast<bool>(payload);
			frame = content ? 1 : 0;
		}
	}
};

class diplomacy_declare_war_dialog : public window_element_base { // eu3dialogtype
private:
	wargoal_setup_window* wargoal_setup_win = nullptr;
	wargoal_state_select_window* wargoal_state_win = nullptr;
	wargoal_country_select_window* wargoal_country_win = nullptr;

	dcon::cb_type_id cb_to_use{};
	dcon::state_definition_id target_state{};
	dcon::national_identity_id target_country{};
	bool will_call_allies = false;

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
			return make_element_by_type<diplomacy_declare_war_title>(state, id);
		} else if(name == "description") {
			return make_element_by_type<diplomacy_declare_war_description1>(state, id);
		} else if(name == "wargoal_add_effect") {
			return make_element_by_type<diplomacy_wargoal_add_window>(state, id);
		} else if(name == "wargoal_success_effect") {
			return make_element_by_type<diplomacy_wargoal_success_window>(state, id);
		} else if(name == "wargoal_failure_effect") {
			return make_element_by_type<diplomacy_wargoal_failure_window>(state, id);
		} else if(name == "description2") {
			return make_element_by_type<diplomacy_declare_war_description2>(state, id);
		} else if(name == "acceptance") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "call_allies_checkbox") {
			return make_element_by_type<diplomacy_declare_war_call_allies_checkbox>(state, id);
		} else if(name == "call_allies_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "agreebutton") {
			return make_element_by_type<diplomacy_declare_war_agree_button>(state, id);
		} else if(name == "declinebutton") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "wargoal_setup") {
			auto ptr = make_element_by_type<wargoal_setup_window>(state, id);
			wargoal_setup_win = ptr.get();
			ptr->set_visible(state, true);
			return ptr;
		} else if(name == "wargoal_state_select") {
			auto ptr = make_element_by_type<wargoal_state_select_window>(state, id);
			wargoal_state_win = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "wargoal_country_select") {
			auto ptr = make_element_by_type<wargoal_country_select_window>(state, id);
			wargoal_country_win = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<element_selection_wrapper<dcon::cb_type_id>>()) {
			cb_to_use = any_cast<element_selection_wrapper<dcon::cb_type_id>>(payload).data;
			if(military::cb_requires_selection_of_a_valid_nation(state, cb_to_use)) {
				wargoal_setup_win->set_visible(state, false);
				wargoal_state_win->set_visible(state, false);
				wargoal_country_win->set_visible(state, true);
			} else if(military::cb_requires_selection_of_a_state(state, cb_to_use)) {
				wargoal_setup_win->set_visible(state, false);
				wargoal_state_win->set_visible(state, true);
				wargoal_country_win->set_visible(state, false);
			} else {
				wargoal_setup_win->set_visible(state, true);
				wargoal_state_win->set_visible(state, false);
				wargoal_country_win->set_visible(state, false);
			}
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<dcon::state_definition_id>>()) {
			target_state = any_cast<element_selection_wrapper<dcon::state_definition_id>>(payload).data;
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<dcon::national_identity_id>>()) {
			target_country = any_cast<element_selection_wrapper<dcon::national_identity_id>>(payload).data;
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<bool>>()) {
			will_call_allies = any_cast<element_selection_wrapper<bool>>(payload).data;
			impl_on_update(state);
			return message_result::consumed;
		}
		//=======================================================================================================
		else if(payload.holds_type<dcon::cb_type_id>()) {
			payload.emplace<dcon::cb_type_id>(cb_to_use);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::state_definition_id>()) {
			payload.emplace<dcon::state_definition_id>(target_state);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::national_identity_id>()) {
			payload.emplace<dcon::national_identity_id>(target_country);
			return message_result::consumed;
		} else if(payload.holds_type<bool>()) {
			payload.emplace<bool>(will_call_allies);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

//====================================================================================================================================

class diplomacy_peace_pick_side_description : public generic_multiline_text<bool> {
public:
	void populate_layout(sys::state& state, text::endless_layout& contents, bool id) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("po_pd_neg_5"));
		text::add_line_break_to_layout_box(contents, state, box);
		text::add_line_break_to_layout_box(contents, state, box);
		text::localised_format_box(state, contents, box, std::string_view("po_welead"));
		text::close_layout_box(contents, box);
	}
};

template<bool B> class diplomacy_peace_tab_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<bool>{B};
			parent->impl_get(state, payload);
		}
	}
};

class diplomacy_peace_pick_side_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "war_desc") {
			return make_element_by_type<diplomacy_peace_pick_side_description>(state, id);
		} else if(name == "offer_button") {
			return make_element_by_type<diplomacy_peace_tab_button<false>>(state, id);
		} else if(name == "offer_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "demand_button") {
			return make_element_by_type<diplomacy_peace_tab_button<true>>(state, id);
		} else if(name == "demand_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

template<bool B> class diplomacy_peace_nation_flag : public flag_button {
public:
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::wargoals_attached_id{};
			parent->impl_get(state, payload);
			dcon::wargoals_attached_id wa = Cyto::any_cast<dcon::wargoals_attached_id>(payload);
			dcon::wargoal_id wg = state.world.wargoals_attached_get_wargoal(wa);
			dcon::nation_id n = B ? state.world.wargoal_get_added_by(wg) : state.world.wargoal_get_target_nation(wg);
			return state.world.nation_get_identity_from_identity_holder(n);
		}
		return dcon::national_identity_id{};
	}
};

class diplomacy_peace_wargoal_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::wargoals_attached_id{};
			parent->impl_get(state, payload);
			dcon::wargoals_attached_id wa = Cyto::any_cast<dcon::wargoals_attached_id>(payload);
			dcon::wargoal_id wg = state.world.wargoals_attached_get_wargoal(wa);
			dcon::cb_type_id cbt = state.world.wargoal_get_type(wg);
			set_text(state, text::produce_simple_string(state, state.world.cb_type_get_name(cbt)));
		}
	}
};

class diplomacy_peace_wargoal_score_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			int32_t score = 0;
			set_text(state, std::to_string(score));
		}
	}
};

struct diplomacy_peace_wargoal {
	dcon::wargoals_attached_id id;
};
class diplomacy_peace_select_button : public button_element_base {
	uint32_t color = 0;

public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::wargoals_attached_id{};
			parent->impl_get(state, payload);
			dcon::wargoals_attached_id wa = Cyto::any_cast<dcon::wargoals_attached_id>(payload);

			Cyto::Any q_payload = diplomacy_peace_wargoal{wa};
			parent->impl_get(state, q_payload);
			frame = Cyto::any_cast<bool>(q_payload) ? 1 : 0;
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::wargoals_attached_id{};
			parent->impl_get(state, payload);
			dcon::wargoals_attached_id wa = Cyto::any_cast<dcon::wargoals_attached_id>(payload);

			Cyto::Any s_payload = element_selection_wrapper<diplomacy_peace_wargoal>{diplomacy_peace_wargoal{wa}};
			parent->impl_get(state, s_payload);
		}
	}
};

class diplomacy_peace_goal_row : public listbox_row_element_base<dcon::wargoals_attached_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "select") {
			return make_element_by_type<diplomacy_peace_select_button>(state, id);
		} else if(name == "country_flag") {
			return make_element_by_type<diplomacy_peace_nation_flag<true>>(state, id);
		} else if(name == "country_flag2") {
			return make_element_by_type<diplomacy_peace_nation_flag<false>>(state, id);
		} else if(name == "cb_direction_arrow") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "select_label") {
			return make_element_by_type<diplomacy_peace_wargoal_text>(state, id);
		} else if(name == "score") {
			return make_element_by_type<diplomacy_peace_wargoal_score_text>(state, id);
		} else {
			return nullptr;
		}
	}
};

class diplomacy_peace_goal_listbox : public listbox_element_base<diplomacy_peace_goal_row, dcon::wargoals_attached_id> {
protected:
	std::string_view get_row_element_name() override { return "peace_goal_item"; }

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		if(parent) {
			Cyto::Any payload = dcon::war_id{};
			parent->impl_get(state, payload);
			const dcon::war_id w = any_cast<dcon::war_id>(payload);
			Cyto::Any b_payload = bool{};
			parent->impl_get(state, b_payload);
			bool const side = any_cast<bool>(b_payload);
			const military::war_role prole = military::get_role(state, w, state.local_player_nation);
			for(auto const wa : state.world.war_get_wargoals_attached_as_war(w)) {
				bool const same_side = military::get_role(state, w, wa.get_wargoal().get_added_by()) == prole;
				if(same_side == side)
					row_contents.push_back(wa);
			}
		}
		update(state);
	}
};

class diplomacy_peace_setup_goals_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_list") {
			return make_element_by_type<diplomacy_peace_goal_listbox>(state, id);
		} else if(name == "score") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "acceptance") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

class diplomacy_peace_total_warscore : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::war_id{};
			parent->impl_get(state, payload);
			const dcon::war_id w = any_cast<dcon::war_id>(payload);
			Cyto::Any n_payload = dcon::nation_id{};
			parent->impl_get(state, n_payload);
			const dcon::nation_id n = any_cast<dcon::nation_id>(n_payload);
			const int32_t score = int32_t(military::directed_warscore(state, w, state.local_player_nation, n));
			set_text(state, std::to_string(score));
		}
	}
};

class diplomacy_peace_send : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::war_id{};
			parent->impl_get(state, payload);
			const dcon::war_id w = any_cast<dcon::war_id>(payload);
			Cyto::Any n_payload = dcon::nation_id{};
			parent->impl_get(state, n_payload);
			const dcon::nation_id n = any_cast<dcon::nation_id>(n_payload);
			Cyto::Any b_payload = bool{};
			parent->impl_get(state, b_payload);
			bool const b = any_cast<bool>(b_payload);

			command::start_peace_offer(state, state.local_player_nation, n, w, b);
			for(auto const wa : state.world.war_get_wargoals_attached_as_war(w)) {
				Cyto::Any q_payload = diplomacy_peace_wargoal{wa};
				parent->impl_get(state, q_payload);
				if(Cyto::any_cast<bool>(q_payload))
					command::add_to_peace_offer(state, state.local_player_nation, wa.get_wargoal());
			}
			command::send_peace_offer(state, state.local_player_nation);
			parent->set_visible(state, false);
		}
	}
};

class diplomacy_setup_peace_dialog : public window_element_base { // eu3dialogtype
	bool side = true;
	std::map<int32_t, bool> wargoals;

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
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			side ? ptr->set_text(state, text::produce_simple_string(state, "demand_peace"))
				 : ptr->set_text(state, text::produce_simple_string(state, "offer_peace"));
			return ptr;
		} else if(name == "warscore_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "warscore") {
			return make_element_by_type<diplomacy_peace_total_warscore>(state, id);
		} else if(name == "agreebutton") {
			return make_element_by_type<diplomacy_peace_send>(state, id);
		} else if(name == "declinebutton") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "pick_side") {
			return make_element_by_type<diplomacy_peace_pick_side_window>(state, id);
		} else if(name == "setup_goals") {
			return make_element_by_type<diplomacy_peace_setup_goals_window>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::war_id>()) {
			Cyto::Any n_payload = dcon::nation_id{};
			parent->impl_get(state, n_payload);
			const dcon::nation_id n = any_cast<dcon::nation_id>(n_payload);
			const dcon::war_id w = military::find_war_between(state, state.local_player_nation, n);
			payload.emplace<dcon::war_id>(w);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<bool>>()) {
			wargoals.clear();
			side = any_cast<element_selection_wrapper<bool>>(payload).data;
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<bool>()) {
			payload.emplace<bool>(side);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<diplomacy_peace_wargoal>>()) {
			const dcon::wargoals_attached_id wa =
				Cyto::any_cast<element_selection_wrapper<diplomacy_peace_wargoal>>(payload).data.id;
			wargoals[wa.index()] = !wargoals[wa.index()];
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<diplomacy_peace_wargoal>()) {
			const dcon::wargoals_attached_id wa = Cyto::any_cast<diplomacy_peace_wargoal>(payload).id;
			payload.emplace<bool>(wargoals[wa.index()]);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

//==========================================================================================================================================================

class cb_wargoal_icon : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::cb_type_id{};
			parent->impl_get(state, payload);
			const dcon::cb_type_id cbt = any_cast<dcon::cb_type_id>(payload);
			frame = state.world.cb_type_get_sprite_index(cbt) - 1;
		}
	}
};

class cb_wargoal_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::cb_type_id{};
			parent->impl_get(state, payload);
			const dcon::cb_type_id content = any_cast<dcon::cb_type_id>(payload);
			set_button_text(state, text::produce_simple_string(state, dcon::fatten(state.world, content).get_name()));
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::cb_type_id{};
			parent->impl_get(state, payload);
			const dcon::cb_type_id content = any_cast<dcon::cb_type_id>(payload);
			Cyto::Any newpayload = element_selection_wrapper<dcon::cb_type_id>{content};
			parent->impl_get(state, newpayload);
		}
	}
};

class diplomacy_make_cb_type : public listbox_row_element_base<dcon::cb_type_id> {
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

class diplomacy_make_cb_listbox : public listbox_element_base<diplomacy_make_cb_type, dcon::cb_type_id> {
protected:
	std::string_view get_row_element_name() override { return "cb_type_item"; }

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			dcon::nation_id content = any_cast<dcon::nation_id>(payload);
			state.world.for_each_cb_type([&](dcon::cb_type_id cb) {
				if(command::can_fabricate_cb(state, state.local_player_nation, content, cb))
					row_contents.push_back(cb);
			});
		}
		update(state);
	}
};

class diplomacy_make_cb_button : public button_element_base {
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

class diplomacy_make_cb_desc : public generic_multiline_text<dcon::cb_type_id> {
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
	void on_update(sys::state& state) noexcept override { set_text(state, text::produce_simple_string(state, "make_cbtitle")); }
};

class diplomacy_make_cb_window : public window_element_base { // eu3dialogtype
private:
	dcon::cb_type_id root_cb{};

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "background") {
			auto ptr = make_element_by_type<draggable_target>(state, id);
			ptr->base_data.size = base_data.size; // Nudge size for proper sizing
			return ptr;
		} else if(name == "diplo_makecb_bg") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "title") {
			return make_element_by_type<make_cb_title>(state, id);
		} else if(name == "description") {
			return make_element_by_type<diplomacy_make_cb_desc>(state, id);
		} else if(name == "leftshield") {
			return make_element_by_type<nation_player_flag>(state, id);
		} else if(name == "rightshield") {
			return make_element_by_type<flag_button>(state, id);
		} else if(name == "agreebutton") {
			return make_element_by_type<diplomacy_make_cb_button>(state, id);
		} else if(name == "declinebutton") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "cb_list") {
			return make_element_by_type<diplomacy_make_cb_listbox>(state, id);
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

class diplomacy_crisis_setup_crisis_pick_side_window : public window_element_base {
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

class diplomacy_crisis_setup_goals_window : public window_element_base {
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

class diplomacy_crisis_backdown_window : public window_element_base { // eu3dialogtype
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
			return make_element_by_type<diplomacy_crisis_setup_crisis_pick_side_window>(state, id);
		} else if(name == "setup_goals") {
			return make_element_by_type<diplomacy_crisis_setup_goals_window>(state, id);
		} else {
			return nullptr;
		}
	}
};

} // namespace ui
