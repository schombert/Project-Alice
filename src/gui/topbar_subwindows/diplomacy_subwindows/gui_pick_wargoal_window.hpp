#pragma once

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
		dcon::cb_type_id content = retrieve<dcon::cb_type_id>(state, parent);
		dcon::cb_type_id selected = parent ? retrieve<dcon::cb_type_id>(state, parent->parent) : dcon::cb_type_id{};

		if(content == selected)
			send(state, parent, element_selection_wrapper<dcon::cb_type_id>{dcon::cb_type_id{}});
		else
			send(state, parent, element_selection_wrapper<dcon::cb_type_id>{content});
	}

	void on_update(sys::state& state) noexcept override {
		dcon::cb_type_id content = retrieve<dcon::cb_type_id>(state, parent);

		auto fat = dcon::fatten(state.world, content);
		set_button_text(state, text::produce_simple_string(state, fat.get_name()));
	}
};

class wargoal_type_item : public listbox_row_element_base<dcon::cb_type_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_icon") {
			auto ptr = make_element_by_type<wargoal_type_item_icon>(state, id);
			//ptr->base_data.position.x += 16; // Nudge
			return ptr;
		} else if(name == "select_goal_invalid") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			//ptr->base_data.position.x += 16; // Nudge
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "select_goal") {
			auto ptr = make_element_by_type<wargoal_type_item_button>(state, id);
			//ptr->base_data.position.x += 16; // Nudge
			return ptr;
		} else {
			return nullptr;
		}
	}
};

class wargoal_type_listbox : public listbox_element_base<wargoal_type_item, dcon::cb_type_id> {
protected:
	std::string_view get_row_element_name() override {
		return "wargoal_item";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();

		auto selected_cb = retrieve<dcon::cb_type_id>(state, parent);
		if(selected_cb) {
			row_contents.push_back(selected_cb);
			update(state);
			return;
		}

		dcon::nation_id content = retrieve<dcon::nation_id>(state, parent);
		auto w = military::find_war_between(state, state.local_player_nation, content);

		if(w) { // war is ongoing
			for(auto cb_type : state.world.in_cb_type) {
				// prevent duplicate war goals
				if(!military::cb_requires_selection_of_a_liberatable_tag(state, cb_type) && !military::cb_requires_selection_of_a_state(state, cb_type) && !military::cb_requires_selection_of_a_valid_nation(state, cb_type)) {
					if(military::war_goal_would_be_duplicate(state, state.local_player_nation, w, content, cb_type, dcon::state_definition_id{}, dcon::national_identity_id{}, dcon::nation_id{}))
						continue;
				}

				if((state.world.cb_type_get_type_bits(cb_type) & military::cb_flag::always) == 0 && military::cb_conditions_satisfied(state, state.local_player_nation, content, cb_type)) {

					bool cb_fabbed = false;
					for(auto& fab_cb : state.world.nation_get_available_cbs(state.local_player_nation)) {
						if(fab_cb.cb_type == cb_type && fab_cb.target == content) {
							cb_fabbed = true;
							break;
						}
					}
					if(!cb_fabbed) {
						if((state.world.cb_type_get_type_bits(cb_type) & military::cb_flag::is_not_constructing_cb) != 0)
							continue; // can only add a constructable cb this way

						auto totalpop = state.world.nation_get_demographics(state.local_player_nation, demographics::total);
						auto jingoism_perc = totalpop > 0 ? state.world.nation_get_demographics(state.local_player_nation, demographics::to_key(state, state.culture_definitions.jingoism)) / totalpop : 0.0f;

						if(state.world.war_get_is_great(w)) {
							if(jingoism_perc >= state.defines.wargoal_jingoism_requirement * state.defines.gw_wargoal_jingoism_requirement_mod) {

								row_contents.push_back(cb_type);
								continue;
							}
						} else {
							if(jingoism_perc >= state.defines.wargoal_jingoism_requirement) {

								row_contents.push_back(cb_type);
								continue;
							}
						}
					} else {
						row_contents.push_back(cb_type);
						continue;
					}
				} else { // this is an always CB
					if(military::cb_conditions_satisfied(state, state.local_player_nation, content, cb_type) && military::can_add_always_cb_to_war(state, state.local_player_nation, content, cb_type, w)) {
						row_contents.push_back(cb_type);
						continue;
					}
				}
			}

		} else { // this is a declare war action
			auto other_cbs = state.world.nation_get_available_cbs(state.local_player_nation);
			for(auto cb : state.world.in_cb_type) {
				bool can_use = military::cb_conditions_satisfied(state, state.local_player_nation, content, cb) && [&]() {
					if((cb.get_type_bits() & military::cb_flag::always) != 0) {
						return true;
					}
					for(auto& fabbed : other_cbs) {
						if(fabbed.cb_type == cb && fabbed.target == content)
							return true;
					}
					return false;
				}();

				if(can_use) {
					row_contents.push_back(cb);
				}
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

class wargoal_state_item_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::state_definition_id{};
			parent->impl_get(state, payload);
			dcon::state_definition_id content = any_cast<dcon::state_definition_id>(payload);
			Cyto::Any newpayload = element_selection_wrapper<dcon::state_definition_id>{ content };
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
	std::string_view get_row_element_name() override {
		return "wargoal_state_item";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		if(parent) {
			dcon::nation_id target = retrieve<dcon::nation_id>(state, parent);
			auto actor = state.local_player_nation;
			dcon::cb_type_id cb = retrieve<dcon::cb_type_id>(state, parent);
			auto war = military::find_war_between(state, actor, target);
			auto secondary_tag = retrieve<dcon::national_identity_id>(state, parent);

			auto allowed_substate_regions = state.world.cb_type_get_allowed_substate_regions(cb);
			if(allowed_substate_regions) {
				for(auto v : state.world.nation_get_overlord_as_ruler(target)) {
					if(v.get_subject().get_is_substate()) {
						for(auto si : state.world.nation_get_state_ownership(target)) {
							if(trigger::evaluate(state, allowed_substate_regions, trigger::to_generic(si.get_state().id), trigger::to_generic(actor), trigger::to_generic(actor))) {

								auto def = si.get_state().get_definition().id;
								if(std::find(row_contents.begin(), row_contents.end(), def) == row_contents.end()) {
									if(!military::war_goal_would_be_duplicate(state, state.local_player_nation, war, v.get_subject(), cb, def, dcon::national_identity_id{}, dcon::nation_id{})) {
										row_contents.push_back(def);
									}
								}
							}
						}
					}
				}
			} else {
				auto allowed_states = state.world.cb_type_get_allowed_states(cb);
				if(auto ac = state.world.cb_type_get_allowed_countries(cb); ac) {
					auto in_nation = state.world.national_identity_get_nation_from_identity_holder(secondary_tag);

					for(auto si : state.world.nation_get_state_ownership(target)) {
						if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(actor), trigger::to_generic(in_nation))) {

							auto def = si.get_state().get_definition().id;
							if(!military::war_goal_would_be_duplicate(state, state.local_player_nation, war, target, cb, def, secondary_tag, dcon::nation_id{})) {
								row_contents.push_back(def);
							}
						}
					}

				} else {
					for(auto si : state.world.nation_get_state_ownership(target)) {
						if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(actor), trigger::to_generic(actor))) {

							auto def = si.get_state().get_definition().id;
							if(!military::war_goal_would_be_duplicate(state, state.local_player_nation, war, target, cb, def, dcon::national_identity_id{}, dcon::nation_id{})) {
								row_contents.push_back(def);
							}
						}
					}
				}
			}
		}
		update(state);
	}
};

class wargoal_cancel_state_select : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		send(state, parent, element_selection_wrapper<dcon::state_definition_id>{dcon::state_definition_id{}});
	}
};


class wargoal_cancel_state_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<dcon::state_definition_id>{};
			parent->impl_get(state, payload);
		}
	}
};


class wargoal_state_select_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "state_list") {
			return make_element_by_type<wargoal_state_listbox>(state, id);
		} else if(name == "cancel_select") {
			return make_element_by_type<wargoal_cancel_state_button>(state, id);
		} else {
			return nullptr;
		}
	}
};

class wargoal_country_item_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::national_identity_id{};
			parent->impl_get(state, payload);
			dcon::national_identity_id content = any_cast<dcon::national_identity_id>(payload);
			Cyto::Any newpayload = element_selection_wrapper<dcon::national_identity_id>{ content };
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
	std::string_view get_row_element_name() override {
		return "wargoal_country_item";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();

		dcon::nation_id target = retrieve<dcon::nation_id>(state, parent);
		auto actor = state.local_player_nation;
		dcon::cb_type_id cb = retrieve<dcon::cb_type_id>(state, parent);
		auto war = military::find_war_between(state, actor, target);

		dcon::trigger_key allowed_countries = state.world.cb_type_get_allowed_countries(cb);
		if(!allowed_countries) {
			update(state);
			return;
		}

		for(auto n : state.world.in_nation) {
			if(n != target && n != actor) {
				if(trigger::evaluate(state, allowed_countries, trigger::to_generic(target), trigger::to_generic(actor),
					trigger::to_generic(n.id))) {

					auto id = state.world.nation_get_identity_from_identity_holder(n);
					if(!war) {
						row_contents.push_back(id);
					} else if(military::cb_requires_selection_of_a_state(state, cb)) {
						row_contents.push_back(id);
					} else {
						if(military::cb_requires_selection_of_a_liberatable_tag(state, cb)) {
							if(!military::war_goal_would_be_duplicate(state, state.local_player_nation, war, target, cb, dcon::state_definition_id{}, id, dcon::nation_id{})) {
								row_contents.push_back(id);
							}
						} else {
							if(!military::war_goal_would_be_duplicate(state, state.local_player_nation, war, target, cb, dcon::state_definition_id{}, dcon::national_identity_id{}, n)) {
								row_contents.push_back(id);
							}
						}
					}
				}
			}
		}

		update(state);
	}
};

class wargoal_cancel_country_select : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		send(state, parent, element_selection_wrapper<dcon::national_identity_id>{dcon::national_identity_id{}});
	}
};

class wargoal_country_select_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "country_list") {
			return make_element_by_type<wargoal_country_listbox>(state, id);
		} else if(name == "cancel_select") {
			return make_element_by_type<wargoal_cancel_country_select>(state, id);
		} else {
			return nullptr;
		}
	}
};


class wargoal_add_prestige : public color_text_element {
public:
	void on_update(sys::state& state) noexcept override {
		auto cb = retrieve<dcon::cb_type_id>(state, parent);
		auto target = retrieve<dcon::nation_id>(state, parent);
		auto source = state.local_player_nation;

		dcon::nation_id real_target = target;

		auto target_ol_rel = state.world.nation_get_overlord_as_subject(target);
		if(state.world.overlord_get_ruler(target_ol_rel))
			real_target = state.world.overlord_get_ruler(target_ol_rel);

		if(cb) {
			if(auto w = military::find_war_between(state, source, target); w) {
				color = text::text_color::white;
				set_text(state, "0.0");
			} else if(military::has_truce_with(state, source, real_target)) {
				auto cb_prestige_loss = military::truce_break_cb_prestige_cost(state, cb);

				color = text::text_color::red;
				set_text(state, text::format_float(cb_prestige_loss, 1));
			} else {
				color = text::text_color::white;
				set_text(state, "0.0");
			}
		} else {
			color = text::text_color::white;
			set_text(state, "0.0");
		}
	}
};
class wargoal_add_infamy : public color_text_element {
public:
	void on_update(sys::state& state) noexcept override {
		auto cb = retrieve<dcon::cb_type_id>(state, parent);
		auto target = retrieve<dcon::nation_id>(state, parent);
		auto source = state.local_player_nation;

		dcon::nation_id real_target = target;

		auto target_ol_rel = state.world.nation_get_overlord_as_subject(target);
		if(state.world.overlord_get_ruler(target_ol_rel))
			real_target = state.world.overlord_get_ruler(target_ol_rel);

		if(cb) {
			if(auto w = military::find_war_between(state, source, target); w) {
				auto cb_infamy = military::cb_addition_infamy_cost(state, w, cb, source, target);

				color = text::text_color::red;
				set_text(state, text::format_float(cb_infamy, 1));
			} else if(military::has_truce_with(state, source, real_target)) {
				auto cb_infamy = military::truce_break_cb_infamy(state, cb);

				color = text::text_color::red;
				set_text(state, text::format_float(cb_infamy, 1));
			} else {
				color = text::text_color::white;
				set_text(state, "0.0");
			}
		} else {
			color = text::text_color::white;
			set_text(state, "0.0");
		}
	}
};
class wargoal_add_militancy : public color_text_element {
public:
	void on_update(sys::state& state) noexcept override {
		auto cb = retrieve<dcon::cb_type_id>(state, parent);
		auto target = retrieve<dcon::nation_id>(state, parent);
		auto source = state.local_player_nation;

		dcon::nation_id real_target = target;

		auto target_ol_rel = state.world.nation_get_overlord_as_subject(target);
		if(state.world.overlord_get_ruler(target_ol_rel))
			real_target = state.world.overlord_get_ruler(target_ol_rel);

		if(cb) {
			if(auto w = military::find_war_between(state, source, target); w) {
				color = text::text_color::white;
				set_text(state, "0.0");
			} else if(military::has_truce_with(state, source, real_target)) {
				auto cb_militancy = military::truce_break_cb_militancy(state, cb);

				color = text::text_color::red;
				set_text(state, text::format_float(cb_militancy, 1));
			} else {
				color = text::text_color::white;
				set_text(state, "0.0");
			}
		} else {
			color = text::text_color::white;
			set_text(state, "0.0");
		}
	}
};

class wargoal_add_header : public simple_body_text {
public:
	void on_create(sys::state& state) noexcept override {
		simple_body_text::on_create(state);
		set_text(state, text::produce_simple_string(state, "wg_result_1"));
	}
};

class diplomacy_wargoal_add_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_gain_effect_text") {
			return make_element_by_type<wargoal_add_header>(state, id);
		} else if(name == "prestige_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "prestige") {
			return make_element_by_type<wargoal_add_prestige>(state, id);
		} else if(name == "infamy_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "infamy") {
			return make_element_by_type<wargoal_add_infamy>(state, id);
		} else if(name == "militancy_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "militancy") {
			return make_element_by_type<wargoal_add_militancy>(state, id);
		} else {
			return nullptr;
		}
	}
};

class fixed_zero : public simple_text_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		simple_text_element_base::on_create(state);
		set_text(state, "0.0");
	}
};


class wargoal_success_prestige : public color_text_element {
public:
	void on_update(sys::state& state) noexcept override {
		auto cb = retrieve<dcon::cb_type_id>(state, parent);
		auto source = state.local_player_nation;

		if(cb) {
			float prestige_gain = military::successful_cb_prestige(state, cb, source);
			if(prestige_gain > 0) {
				color = text::text_color::green;
				set_text(state, text::format_float(prestige_gain, 1));
			} else {
				color = text::text_color::white;
				set_text(state, "0.0");
			}
		} else {
			color = text::text_color::white;
			set_text(state, "0.0");
		}
	}
};

class wargoal_success_header : public simple_body_text {
public:
	void on_create(sys::state& state) noexcept override {
		simple_body_text::on_create(state);
		set_text(state, text::produce_simple_string(state, "wg_result_2"));
	}
};

class diplomacy_wargoal_success_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_gain_effect_text") {
			return make_element_by_type<wargoal_success_header>(state, id);
		} else if(name == "prestige_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "prestige") {
			return make_element_by_type<wargoal_success_prestige>(state, id);
		} else if(name == "infamy_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "infamy") {
			return make_element_by_type<fixed_zero>(state, id);
		} else if(name == "militancy_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "militancy") {
			return make_element_by_type<fixed_zero>(state, id);
		} else {
			return nullptr;
		}
	}
};

class wargoal_failure_prestige : public color_text_element {
public:
	void on_update(sys::state& state) noexcept override {
		auto cb = retrieve<dcon::cb_type_id>(state, parent);
		auto source = state.local_player_nation;
		if(cb) {
			float prestige_loss = std::min(state.defines.war_failed_goal_prestige_base, state.defines.war_failed_goal_prestige * nations::prestige_score(state, source)) * state.world.cb_type_get_penalty_factor(cb);

			if(prestige_loss < 0) {
				color = text::text_color::red;
				set_text(state, text::format_float(prestige_loss, 1));
			} else {
				color = text::text_color::white;
				set_text(state, "0.0");
			}
		} else {
			color = text::text_color::white;
			set_text(state, "0.0");
		}
	}
};

class wargoal_failure_militancy : public color_text_element {
public:
	void on_update(sys::state& state) noexcept override {
		auto cb = retrieve<dcon::cb_type_id>(state, parent);
		if(cb) {
			auto pop_militancy = state.defines.war_failed_goal_militancy * state.world.cb_type_get_penalty_factor(cb);

			if(pop_militancy > 0) {
				color = text::text_color::red;
				set_text(state, text::format_float(pop_militancy, 1));
			} else {
				color = text::text_color::white;
				set_text(state, "0.0");
			}
		} else {
			color = text::text_color::white;
			set_text(state, "0.0");
		}
	}
};

class wargoal_failure_header : public simple_body_text {
public:
	void on_create(sys::state& state) noexcept override {
		simple_body_text::on_create(state);
		set_text(state, text::produce_simple_string(state, "wg_result_3"));
	}
};

class diplomacy_wargoal_failure_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_gain_effect_text") {
			return make_element_by_type<wargoal_failure_header>(state, id);
		} else if(name == "prestige_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "prestige") {
			return make_element_by_type<wargoal_failure_prestige>(state, id);
		} else if(name == "infamy_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "infamy") {
			return make_element_by_type<fixed_zero>(state, id);
		} else if(name == "militancy_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "militancy") {
			return make_element_by_type<wargoal_failure_militancy>(state, id);
		} else {
			return nullptr;
		}
	}
};

class diplomacy_declare_war_title : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto war = retrieve<dcon::war_id>(state, parent);
		if(!war) {
			set_text(state, text::produce_simple_string(state, "wartitle"));
		} else {
			set_text(state, text::produce_simple_string(state, "wargoaltitle"));
		}
	}
};

struct check_wg_completion {
	bool done = false;
};

class diplomacy_declare_war_agree_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto wg_ready = retrieve<check_wg_completion>(state, parent).done;
		if(!wg_ready) {
			disabled = true;
			return;
		}

		dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);
		dcon::state_definition_id s = retrieve<dcon::state_definition_id>(state, parent);
		dcon::national_identity_id ni = retrieve<dcon::national_identity_id>(state, parent);
		dcon::cb_type_id c = retrieve<dcon::cb_type_id>(state, parent);

		if(military::are_at_war(state, state.local_player_nation, n)) {
			dcon::war_id w = military::find_war_between(state, state.local_player_nation, n);
			disabled = !command::can_add_war_goal(state, state.local_player_nation, w, n, c, s, ni,
					state.world.national_identity_get_nation_from_identity_holder(ni));
		} else {
			disabled = !command::can_declare_war(state, state.local_player_nation, n, c, s, ni,
					state.world.national_identity_get_nation_from_identity_holder(ni));
		}
	}

	void button_action(sys::state& state) noexcept override {
		dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);
		dcon::state_definition_id s = retrieve<dcon::state_definition_id>(state, parent);
		dcon::national_identity_id ni = retrieve<dcon::national_identity_id>(state, parent);
		dcon::cb_type_id c = retrieve<dcon::cb_type_id>(state, parent);
		if(military::are_at_war(state, state.local_player_nation, n)) {
			dcon::war_id w = military::find_war_between(state, state.local_player_nation, n);
			command::add_war_goal(state, state.local_player_nation, w, n, c, s, ni,
					state.world.national_identity_get_nation_from_identity_holder(ni));
		} else {
			command::declare_war(state, state.local_player_nation, n, c, s, ni,
					state.world.national_identity_get_nation_from_identity_holder(ni), retrieve<bool>(state, parent));
		}
		parent->set_visible(state, false);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto wg_ready = retrieve<check_wg_completion>(state, parent).done;
		if(!wg_ready) {
			text::add_line_with_condition(state, contents, "wg_not_ready", false);
			return;
		}

		dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);
		dcon::state_definition_id s = retrieve<dcon::state_definition_id>(state, parent);
		dcon::national_identity_id ni = retrieve<dcon::national_identity_id>(state, parent);
		dcon::cb_type_id c = retrieve<dcon::cb_type_id>(state, parent);

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
};


int32_t calculate_partial_score(sys::state& state, dcon::nation_id target, dcon::cb_type_id id, dcon::state_definition_id state_def, dcon::national_identity_id second_nation) {
	int32_t cost = -1;

	auto war = military::find_war_between(state, state.local_player_nation, target);
	if(!military::cb_requires_selection_of_a_state(state, id) && !military::cb_requires_selection_of_a_liberatable_tag(state, id) && !military::cb_requires_selection_of_a_valid_nation(state, id)) {

		cost = military::peace_cost(state, military::find_war_between(state, state.local_player_nation, target), id, state.local_player_nation, target, dcon::nation_id{}, dcon::state_definition_id{}, dcon::national_identity_id{});
	} else if(military::cb_requires_selection_of_a_state(state, id)) {

		if(state_def) {
			if(military::cb_requires_selection_of_a_liberatable_tag(state, id)) {
				if(!second_nation) {
					return -1;
				}
			} else if(military::cb_requires_selection_of_a_valid_nation(state, id)) {
				if(!second_nation) {
					return -1;
				}
			}

			cost = 0;

			// for each state ...
			if(war) {
				auto is_attacker = military::is_attacker(state, war, state.local_player_nation);
				for(auto si : state.world.in_state_instance) {
					if(si.get_definition() == state_def) {
						auto wr = military::get_role(state, war, si.get_nation_from_state_ownership());
						if((is_attacker && wr == military::war_role::defender) || (!is_attacker && wr == military::war_role::attacker)) {
							if(military::cb_requires_selection_of_a_liberatable_tag(state, id)) {
								cost += military::peace_cost(state, war, id, state.local_player_nation, si.get_nation_from_state_ownership(), dcon::nation_id{}, state_def, second_nation);
							} else if(military::cb_requires_selection_of_a_valid_nation(state, id)) {
								cost += military::peace_cost(state, war, id, state.local_player_nation, si.get_nation_from_state_ownership(), state.world.national_identity_get_nation_from_identity_holder(second_nation), state_def, dcon::national_identity_id{});
							} else {
								cost += military::peace_cost(state, war, id, state.local_player_nation, si.get_nation_from_state_ownership(), dcon::nation_id{}, state_def, dcon::national_identity_id{});
							}
						}
					}
				}
			} else {
				for(auto si : state.world.in_state_instance) {
					if(si.get_definition() == state_def) {
						auto n = si.get_nation_from_state_ownership();
						auto no = n.get_overlord_as_subject().get_ruler();
						if(n == target || no == target) {
							if(military::cb_requires_selection_of_a_liberatable_tag(state, id)) {
								cost += military::peace_cost(state, dcon::war_id{}, id, state.local_player_nation, si.get_nation_from_state_ownership(), dcon::nation_id{}, state_def, second_nation);
							} else if(military::cb_requires_selection_of_a_valid_nation(state, id)) {
								cost += military::peace_cost(state, dcon::war_id{}, id, state.local_player_nation, si.get_nation_from_state_ownership(), state.world.national_identity_get_nation_from_identity_holder(second_nation), state_def, dcon::national_identity_id{});
							} else {
								cost += military::peace_cost(state, dcon::war_id{}, id, state.local_player_nation, si.get_nation_from_state_ownership(), dcon::nation_id{}, state_def, dcon::national_identity_id{});
							}
						}
					}
				}
			}
		}
	} else if(military::cb_requires_selection_of_a_liberatable_tag(state, id)) {
		if(second_nation) {
			cost = military::peace_cost(state, military::find_war_between(state, state.local_player_nation, target), id, state.local_player_nation, target, dcon::nation_id{}, dcon::state_definition_id{}, second_nation);
		}
	} else if(military::cb_requires_selection_of_a_valid_nation(state, id)) {
		if(second_nation) {
			cost = military::peace_cost(state, military::find_war_between(state, state.local_player_nation, target), id, state.local_player_nation, target, state.world.national_identity_get_nation_from_identity_holder(second_nation), dcon::state_definition_id{}, dcon::national_identity_id{});
		}
	}
	return cost;
}

class diplomacy_declare_war_description1 : public simple_multiline_body_text {
public:
	void populate_layout(sys::state& state, text::endless_layout& contents) noexcept override {
		auto id = retrieve<dcon::cb_type_id>(state, parent);
		auto fat_cb = dcon::fatten(state.world, id);
		auto box = text::open_layout_box(contents);
		auto target = retrieve<dcon::nation_id>(state, parent);

		text::substitution_map sub;

		if(id) {
			text::add_to_substitution_map(sub, text::variable_type::goal, fat_cb.get_name());
			text::localised_format_box(state, contents, box, std::string_view("add_wargoal_wargoal"), sub);
		} else {
			auto str = text::produce_simple_string(state, "nocasusbelli");
			text::add_to_substitution_map(sub, text::variable_type::goal, std::string_view(str));
			text::localised_format_box(state, contents, box, std::string_view("add_wargoal_wargoal"), sub);
		}
		text::close_layout_box(contents, box);

		if(id) {
			auto state_def = retrieve<dcon::state_definition_id>(state, parent);
			auto second_nation = retrieve<dcon::national_identity_id>(state, parent);
			auto cost = calculate_partial_score(state, target, id, state_def, second_nation);
			if(cost != -1) {
				text::add_line(state, contents, "add_wargoal_peace_cost", text::variable_type::cost, int64_t(cost));
			}
		}

	}
};

class diplomacy_declare_war_description2 : public simple_multiline_body_text {
public:
	void populate_layout(sys::state& state, text::endless_layout& contents) noexcept override {
		auto id = retrieve<dcon::cb_type_id>(state, parent);

		auto content = retrieve<dcon::nation_id>(state, parent);
		auto staat = retrieve<dcon::state_definition_id>(state, parent);
		auto nacion = retrieve<dcon::national_identity_id>(state, parent);

		auto fat_cb = dcon::fatten(state.world, id);

		auto box = text::open_layout_box(contents);

		if(id) {
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::recipient,
					dcon::fatten(state.world, content).get_name()); // Target Nation
			text::add_to_substitution_map(sub, text::variable_type::third,
					dcon::fatten(state.world, nacion).get_name()); // Third Party Country
			text::add_to_substitution_map(sub, text::variable_type::actor, dcon::fatten(state.world, nacion).get_name());
			text::add_to_substitution_map(sub, text::variable_type::state, dcon::fatten(state.world, staat).get_name());
			text::add_to_substitution_map(sub, text::variable_type::region, dcon::fatten(state.world, staat).get_name());

			text::add_to_layout_box(state, contents, box, fat_cb.get_long_desc(), sub);
		}

		text::close_layout_box(contents, box);
	}
};

class diplomacy_declare_war_call_allies_checkbox : public button_element_base {
public:
	bool show = true;

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = bool{};
			parent->impl_get(state, payload);
			bool content = any_cast<bool>(payload);
			Cyto::Any b_payload = element_selection_wrapper<bool>{ !content };
			parent->impl_get(state, b_payload);
		}
	}

	void on_update(sys::state& state) noexcept override {
		bool content = retrieve<bool>(state, parent);
		frame = content ? 1 : 0;
		auto war = retrieve<dcon::war_id>(state, parent);
		show = !bool(war);
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(show)
			button_element_base::render(state, x, y);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return show ? tooltip_behavior::variable_tooltip : tooltip_behavior::no_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(!show)
			return;
	}
};

class diplomacy_declare_war_call_allies_text : public simple_text_element_base {
public:
	bool show = true;

	void on_update(sys::state& state) noexcept override {
		auto war = retrieve<dcon::war_id>(state, parent);
		show = !bool(war);
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(show)
			simple_text_element_base::render(state, x, y);
	}
};

class diplomacy_declare_war_dialog : public window_element_base { // eu3dialogtype
private:
	wargoal_setup_window* wargoal_setup_win = nullptr;
	wargoal_state_select_window* wargoal_state_win = nullptr;
	wargoal_country_select_window* wargoal_country_win = nullptr;

	dcon::cb_type_id cb_to_use;
	dcon::state_definition_id target_state;
	dcon::national_identity_id target_country;
	bool will_call_allies = false;
	bool wargoal_decided_upon = false;
public:
	void reset_window() {
		cb_to_use = dcon::cb_type_id{};
		target_state = dcon::state_definition_id{};
		target_country = dcon::national_identity_id{};
		will_call_allies = false;
		wargoal_decided_upon = false;
	}

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
			return make_element_by_type<diplomacy_declare_war_call_allies_text>(state, id);
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
		if(payload.holds_type<dcon::war_id>()) {
			const dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);
			const dcon::war_id w = military::find_war_between(state, state.local_player_nation, n);
			payload.emplace<dcon::war_id>(w);
			return message_result::consumed;
		} else if(payload.holds_type< check_wg_completion>()) {
			payload.emplace<check_wg_completion>(check_wg_completion{ wargoal_decided_upon });
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<dcon::cb_type_id>>()) {
			cb_to_use = any_cast<element_selection_wrapper<dcon::cb_type_id>>(payload).data;
			if(!cb_to_use) {
				wargoal_decided_upon = false;
			} else if(military::cb_requires_selection_of_a_valid_nation(state, cb_to_use) || military::cb_requires_selection_of_a_liberatable_tag(state, cb_to_use)) {
				wargoal_setup_win->set_visible(state, false);
				wargoal_state_win->set_visible(state, false);
				wargoal_country_win->set_visible(state, true);
			} else if(military::cb_requires_selection_of_a_state(state, cb_to_use)) {
				wargoal_setup_win->set_visible(state, false);
				wargoal_state_win->set_visible(state, true);
				wargoal_country_win->set_visible(state, false);
			} else {
				wargoal_decided_upon = true;
				wargoal_setup_win->set_visible(state, true);
				wargoal_state_win->set_visible(state, false);
				wargoal_country_win->set_visible(state, false);
			}
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<dcon::state_definition_id>>()) {
			target_state = any_cast<element_selection_wrapper<dcon::state_definition_id>>(payload).data;
			if(target_state) {
				wargoal_decided_upon = true;
				wargoal_setup_win->set_visible(state, true);
				wargoal_state_win->set_visible(state, false);
				wargoal_country_win->set_visible(state, false);
			} else {
				if(military::cb_requires_selection_of_a_valid_nation(state, cb_to_use) || military::cb_requires_selection_of_a_liberatable_tag(state, cb_to_use)) {
					wargoal_setup_win->set_visible(state, false);
					wargoal_state_win->set_visible(state, false);
					wargoal_country_win->set_visible(state, true);
				} else {
					cb_to_use = dcon::cb_type_id{};
					wargoal_decided_upon = false;
					wargoal_setup_win->set_visible(state, true);
					wargoal_state_win->set_visible(state, false);
					wargoal_country_win->set_visible(state, false);
				}
			}
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<dcon::national_identity_id>>()) {
			target_country = any_cast<element_selection_wrapper<dcon::national_identity_id>>(payload).data;

			if(target_country) { // goto next step
				if(military::cb_requires_selection_of_a_state(state, cb_to_use)) {
					wargoal_setup_win->set_visible(state, false);
					wargoal_state_win->set_visible(state, true);
					wargoal_country_win->set_visible(state, false);
				} else {
					wargoal_decided_upon = true;
					wargoal_setup_win->set_visible(state, true);
					wargoal_state_win->set_visible(state, false);
					wargoal_country_win->set_visible(state, false);
				}
			} else {
				wargoal_decided_upon = false;
				cb_to_use = dcon::cb_type_id{};
				wargoal_setup_win->set_visible(state, true);
				wargoal_state_win->set_visible(state, false);
				wargoal_country_win->set_visible(state, false);
			}

			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<bool>>()) {
			will_call_allies = any_cast<element_selection_wrapper<bool>>(payload).data;
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::cb_type_id>()) {
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

struct get_target {
	dcon::nation_id n;
};
struct get_offer_to {
	dcon::nation_id n;
};
struct set_target {
	dcon::nation_id n;
};

class wargoal_offer_description1 : public simple_multiline_body_text {
public:
	void populate_layout(sys::state& state, text::endless_layout& contents) noexcept override {
		auto id = retrieve<dcon::cb_type_id>(state, parent);
		auto fat_cb = dcon::fatten(state.world, id);
		auto box = text::open_layout_box(contents);
		auto target = retrieve<get_target>(state, parent).n;

		text::substitution_map sub;

		if(id) {
			text::add_to_substitution_map(sub, text::variable_type::goal, fat_cb.get_name());
			text::localised_format_box(state, contents, box, std::string_view("add_wargoal_wargoal"), sub);
		} else {
			auto str = text::produce_simple_string(state, "nocasusbelli");
			text::add_to_substitution_map(sub, text::variable_type::goal, std::string_view(str));
			text::localised_format_box(state, contents, box, std::string_view("add_wargoal_wargoal"), sub);
		}
		text::close_layout_box(contents, box);

		if(id) {
			auto state_def = retrieve<dcon::state_definition_id>(state, parent);
			auto second_nation = retrieve<dcon::national_identity_id>(state, parent);
			auto cost = calculate_partial_score(state, target, id, state_def, second_nation);
			if(cost != -1) {
				text::add_line(state, contents, "add_wargoal_peace_cost", text::variable_type::cost, int64_t(cost));
			}
		}

	}
};

class wargoal_offer_description2 : public simple_multiline_body_text {
public:
	void populate_layout(sys::state& state, text::endless_layout& contents) noexcept override {
		auto id = retrieve<dcon::cb_type_id>(state, parent);

		auto content = retrieve<get_target>(state, parent).n;
		auto staat = retrieve<dcon::state_definition_id>(state, parent);
		auto nacion = retrieve<dcon::national_identity_id>(state, parent);

		auto fat_cb = dcon::fatten(state.world, id);

		auto box = text::open_layout_box(contents);

		if(id) {
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::recipient,
					dcon::fatten(state.world, content).get_name()); // Target Nation
			text::add_to_substitution_map(sub, text::variable_type::third,
					dcon::fatten(state.world, nacion).get_name()); // Third Party Country
			text::add_to_substitution_map(sub, text::variable_type::actor, dcon::fatten(state.world, nacion).get_name());
			text::add_to_substitution_map(sub, text::variable_type::state, dcon::fatten(state.world, staat).get_name());
			text::add_to_substitution_map(sub, text::variable_type::region, dcon::fatten(state.world, staat).get_name());

			text::add_to_layout_box(state, contents, box, fat_cb.get_long_desc(), sub);
		}

		text::close_layout_box(contents, box);
	}
};


class wargoal_offer_agree_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto wg_ready = retrieve<check_wg_completion>(state, parent).done;
		if(!wg_ready) {
			disabled = true;
			return;
		}
		auto ni = retrieve<dcon::national_identity_id>(state, parent);
		disabled = !command::can_invite_to_crisis(state, state.local_player_nation, retrieve<get_offer_to>(state, parent).n, retrieve<get_target>(state, parent).n, retrieve<dcon::cb_type_id>(state, parent), retrieve<dcon::state_definition_id>(state, parent), ni, state.world.national_identity_get_nation_from_identity_holder(ni));
	}

	void button_action(sys::state& state) noexcept override {
		auto ni = retrieve<dcon::national_identity_id>(state, parent);
		command::invite_to_crisis(state, state.local_player_nation, retrieve<get_offer_to>(state, parent).n, retrieve<get_target>(state, parent).n, retrieve<dcon::cb_type_id>(state, parent), retrieve<dcon::state_definition_id>(state, parent), ni, state.world.national_identity_get_nation_from_identity_holder(ni));
		parent->set_visible(state, false);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto wg_ready = retrieve<check_wg_completion>(state, parent).done;
		if(!wg_ready) {
			text::add_line_with_condition(state, contents, "wg_not_ready", false);
			return;
		}
	}
};

class wargoal_offer_title : public simple_text_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		simple_text_element_base::on_create(state);
		set_text(state, text::produce_simple_string(state, "crisis_offertitle"));
	}
};


class wargoal_offer_add_infamy : public color_text_element {
public:
	void on_update(sys::state& state) noexcept override {
		auto cb = retrieve<dcon::cb_type_id>(state, parent);
		auto target = retrieve<get_target>(state, parent).n;
		auto offered = retrieve<get_offer_to>(state, parent).n;

		auto infamy = military::crisis_cb_addition_infamy_cost(state, cb, offered, target) *
			state.defines.crisis_wargoal_infamy_mult;

		if(infamy > 0) {
			color = text::text_color::red;
			set_text(state, text::format_float(infamy, 1));
		} else {
			color = text::text_color::white;
			set_text(state, "0.0");
		}
	}
};


class wargoal_offer_add_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_gain_effect_text") {
			return make_element_by_type<wargoal_add_header>(state, id);
		} else if(name == "prestige_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "prestige") {
			return make_element_by_type<fixed_zero>(state, id);
		} else if(name == "infamy_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "infamy") {
			return make_element_by_type<wargoal_offer_add_infamy>(state, id);
		} else if(name == "militancy_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "militancy") {
			return make_element_by_type<fixed_zero>(state, id);
		} else {
			return nullptr;
		}
	}
};

class wargoal_offer_type_listbox : public listbox_element_base<wargoal_type_item, dcon::cb_type_id> {
protected:
	std::string_view get_row_element_name() override {
		return "wargoal_item";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();

		auto selected_cb = retrieve<dcon::cb_type_id>(state, parent);
		if(selected_cb) {
			row_contents.push_back(selected_cb);
			update(state);
			return;
		}

		dcon::nation_id actor = retrieve<get_offer_to>(state, parent).n;
		dcon::nation_id content = retrieve<get_target>(state, parent).n;

		for(auto cb_type : state.world.in_cb_type) {
			
			if(military::cb_conditions_satisfied(state, actor, content, cb_type)) {
				if((cb_type.get_type_bits() & military::cb_flag::always) != 0)
					row_contents.push_back(cb_type);
				else if((cb_type.get_type_bits() & military::cb_flag::is_not_constructing_cb) == 0)
					row_contents.push_back(cb_type);
			}
		}

		update(state);
	}
};

class wargoal_offer_setup_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "country_list") {
			return make_element_by_type<wargoal_offer_type_listbox>(state, id);
		} else if(name == "cancel_select") {
			return make_element_by_type<wargoal_cancel_button>(state, id);
		} else {
			return nullptr;
		}
	}
};

class wargoal_offer_state_listbox : public listbox_element_base<wargoal_state_item, dcon::state_definition_id> {
protected:
	std::string_view get_row_element_name() override {
		return "wargoal_state_item";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		if(parent) {
			dcon::nation_id target = retrieve<get_target>(state, parent).n;
			auto actor = state.local_player_nation;
			dcon::cb_type_id cb = retrieve<dcon::cb_type_id>(state, parent);
			auto secondary_tag = retrieve<dcon::national_identity_id>(state, parent);

			auto allowed_substate_regions = state.world.cb_type_get_allowed_substate_regions(cb);
			if(allowed_substate_regions) {
				for(auto v : state.world.nation_get_overlord_as_ruler(target)) {
					if(v.get_subject().get_is_substate()) {
						for(auto si : state.world.nation_get_state_ownership(target)) {
							if(trigger::evaluate(state, allowed_substate_regions, trigger::to_generic(si.get_state().id), trigger::to_generic(actor), trigger::to_generic(actor))) {

								auto def = si.get_state().get_definition().id;
								if(std::find(row_contents.begin(), row_contents.end(), def) == row_contents.end()) {

									
									row_contents.push_back(def);

								}

							}
						}
					}
				}
			} else {
				auto allowed_states = state.world.cb_type_get_allowed_states(cb);
				if(auto ac = state.world.cb_type_get_allowed_countries(cb); ac) {
					auto in_nation = state.world.national_identity_get_nation_from_identity_holder(secondary_tag);

					for(auto si : state.world.nation_get_state_ownership(target)) {
						if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(actor), trigger::to_generic(in_nation))) {

							row_contents.push_back(si.get_state().get_definition().id);
						}
					}

				} else {
					for(auto si : state.world.nation_get_state_ownership(target)) {
						if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(actor), trigger::to_generic(actor))) {

							
							row_contents.push_back(si.get_state().get_definition().id);
						}
					}
				}
			}
		}
		update(state);
	}
};

class wargoal_offer_state_select_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "state_list") {
			return make_element_by_type<wargoal_offer_state_listbox>(state, id);
		} else if(name == "cancel_select") {
			return make_element_by_type<wargoal_cancel_button>(state, id);
		} else {
			return nullptr;
		}
	}
};

class wargoal_target_country_item_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		send(state, parent, set_target{ n });
	}

	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		set_button_text(state, text::produce_simple_string(state, dcon::fatten(state.world, n).get_name()));
	}
};

class wargoal_target_country_item : public listbox_row_element_base<dcon::nation_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "select_country") {
			return make_element_by_type<wargoal_target_country_item_button>(state, id);
		} else {
			return nullptr;
		}
	}
};

class wargoal_target_country_listbox : public listbox_element_base<wargoal_target_country_item, dcon::nation_id> {
protected:
	std::string_view get_row_element_name() override {
		return "wargoal_country_item";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		for(auto& par : state.crisis_participants) {
			if(!par.id) {
				break; // not in crisis
			}
			if(!par.merely_interested) {
				if(state.local_player_nation == state.primary_crisis_attacker && !par.supports_attacker)
					row_contents.push_back(par.id);
				else if(state.local_player_nation == state.primary_crisis_defender && par.supports_attacker)
					row_contents.push_back(par.id);
			}
		}

		update(state);
	}
};

class wargoal_target_country_select_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_list") {
			return make_element_by_type<wargoal_target_country_listbox>(state, id);
		} else {
			return nullptr;
		}
	}
};

class wargoal_offer_country_listbox : public listbox_element_base<wargoal_country_item, dcon::national_identity_id> {
protected:
	std::string_view get_row_element_name() override {
		return "wargoal_country_item";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();

		dcon::nation_id target = retrieve<get_target>(state, parent).n;
		auto actor = retrieve<get_offer_to>(state, parent).n;
		dcon::cb_type_id cb = retrieve<dcon::cb_type_id>(state, parent);

		dcon::trigger_key allowed_countries = state.world.cb_type_get_allowed_countries(cb);
		if(!allowed_countries) {
			update(state);
			return;
		}

		for(auto n : state.world.in_nation) {
			if(n != target && n != actor && n != state.local_player_nation) {
				if(trigger::evaluate(state, allowed_countries, trigger::to_generic(target), trigger::to_generic(actor),
					trigger::to_generic(n.id))) {

					auto id = state.world.nation_get_identity_from_identity_holder(n);
					row_contents.push_back(id);
				}
			}
		}

		update(state);
	}
};

class wargoal_offer_country_select_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "country_list") {
			return make_element_by_type<wargoal_offer_country_listbox>(state, id);
		} else if(name == "cancel_select") {
			return make_element_by_type<wargoal_cancel_country_select>(state, id);
		} else {
			return nullptr;
		}
	}
};

class offer_war_goal_dialog : public window_element_base {
private:
	wargoal_offer_setup_window* wargoal_setup_win = nullptr;
	wargoal_offer_state_select_window* wargoal_state_win = nullptr;
	wargoal_offer_country_select_window* wargoal_country_win = nullptr;
	wargoal_target_country_select_window* wargoal_target_win = nullptr;

	dcon::nation_id offer_made_to;
	dcon::nation_id wargoal_against;
	dcon::cb_type_id cb_to_use;
	dcon::state_definition_id target_state;
	dcon::national_identity_id target_country;
	bool will_call_allies = false;
	bool wargoal_decided_upon = false;
public:
	void reset_window(sys::state& state, dcon::nation_id offer_to) {
		wargoal_against = dcon::nation_id{};
		offer_made_to = offer_to;
		cb_to_use = dcon::cb_type_id{};
		target_state = dcon::state_definition_id{};
		target_country = dcon::national_identity_id{};
		will_call_allies = false;
		wargoal_decided_upon = false;

		wargoal_setup_win->set_visible(state, false);
		wargoal_state_win->set_visible(state, false);
		wargoal_country_win->set_visible(state, false);
		wargoal_target_win->set_visible(state, true);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "background") {
			auto ptr = make_element_by_type<draggable_target>(state, id);
			ptr->base_data.size = base_data.size; // Nudge size for proper sizing
			return ptr;
		} else if(name == "leftshield") {
			return make_element_by_type<nation_player_flag>(state, id);
		} else if(name == "rightshield") {
			return make_element_by_type<flag_button>(state, id);
		} else if(name == "title") {
			return make_element_by_type<wargoal_offer_title>(state, id);
		} else if(name == "description") {
			return make_element_by_type<wargoal_offer_description1>(state, id);
		} else if(name == "wargoal_add_effect") {
			return make_element_by_type<wargoal_offer_add_window>(state, id);
		} else if(name == "wargoal_success_effect") {
			auto ptr = make_element_by_type<element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "wargoal_failure_effect") {
			auto ptr = make_element_by_type<element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "description2") {
			return make_element_by_type<wargoal_offer_description2>(state, id);
		} else if(name == "acceptance") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "call_allies_checkbox") {
			auto ptr = make_element_by_type<element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "call_allies_text") {
			auto ptr = make_element_by_type<element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "agreebutton") {
			return make_element_by_type<wargoal_offer_agree_button>(state, id);
		} else if(name == "declinebutton") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "wargoal_setup") {
			auto ptr = make_element_by_type<wargoal_target_country_select_window>(state, id);
			wargoal_target_win = ptr.get();
			ptr->set_visible(state, true);
			return ptr;
		} else if(name == "wargoal_state_select") {
			auto ptr = make_element_by_type<wargoal_offer_state_select_window>(state, id);
			wargoal_state_win = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "wargoal_country_select") {
			{
				auto ptr = make_element_by_type<wargoal_offer_setup_window>(state, id);
				wargoal_setup_win = ptr.get();
				ptr->set_visible(state, false);
				add_child_to_front(std::move(ptr));
			}
			{
				auto ptr = make_element_by_type<wargoal_offer_country_select_window>(state, id);
				wargoal_country_win = ptr.get();
				ptr->set_visible(state, false);
				return ptr;
			}
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(offer_made_to);
		} else if(payload.holds_type<dcon::national_identity_id>()) {
			payload.emplace<dcon::national_identity_id>(state.world.nation_get_identity_from_identity_holder(offer_made_to));
		} else if(payload.holds_type<get_target>()) {
			payload.emplace<get_target>(get_target{ wargoal_against });
		} else if(payload.holds_type<get_offer_to>()) {
			payload.emplace<get_offer_to>(get_offer_to{ offer_made_to });
		} else if(payload.holds_type< check_wg_completion>()) {
			payload.emplace<check_wg_completion>(check_wg_completion{ wargoal_decided_upon });
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<dcon::cb_type_id>>()) {
			cb_to_use = any_cast<element_selection_wrapper<dcon::cb_type_id>>(payload).data;
			if(!cb_to_use) {
				wargoal_against = dcon::nation_id{};
				cb_to_use = dcon::cb_type_id{};
				target_state = dcon::state_definition_id{};
				target_country = dcon::national_identity_id{};
				wargoal_decided_upon = false;

				wargoal_setup_win->set_visible(state, false);
				wargoal_state_win->set_visible(state, false);
				wargoal_country_win->set_visible(state, false);
				wargoal_target_win->set_visible(state, true);
			} else if(military::cb_requires_selection_of_a_valid_nation(state, cb_to_use) || military::cb_requires_selection_of_a_liberatable_tag(state, cb_to_use)) {
				wargoal_setup_win->set_visible(state, false);
				wargoal_state_win->set_visible(state, false);
				wargoal_country_win->set_visible(state, true);
				wargoal_target_win->set_visible(state, false);
			} else if(military::cb_requires_selection_of_a_state(state, cb_to_use)) {
				wargoal_setup_win->set_visible(state, false);
				wargoal_state_win->set_visible(state, true);
				wargoal_country_win->set_visible(state, false);
				wargoal_target_win->set_visible(state, false);
			} else {
				wargoal_decided_upon = true;
				wargoal_setup_win->set_visible(state, true);
				wargoal_state_win->set_visible(state, false);
				wargoal_country_win->set_visible(state, false);
				wargoal_target_win->set_visible(state, false);
			}
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<dcon::state_definition_id>>()) {
			target_state = any_cast<element_selection_wrapper<dcon::state_definition_id>>(payload).data;
			if(target_state) {
				wargoal_decided_upon = true;
				wargoal_setup_win->set_visible(state, true);
				wargoal_state_win->set_visible(state, false);
				wargoal_country_win->set_visible(state, false);
				wargoal_target_win->set_visible(state, false);
			} else {
				if(military::cb_requires_selection_of_a_valid_nation(state, cb_to_use) || military::cb_requires_selection_of_a_liberatable_tag(state, cb_to_use)) {
					wargoal_setup_win->set_visible(state, false);
					wargoal_state_win->set_visible(state, false);
					wargoal_country_win->set_visible(state, true);
					wargoal_target_win->set_visible(state, false);
				} else {
					wargoal_decided_upon = false;
					cb_to_use = dcon::cb_type_id{};
					wargoal_setup_win->set_visible(state, true);
					wargoal_state_win->set_visible(state, false);
					wargoal_country_win->set_visible(state, false);
					wargoal_target_win->set_visible(state, false);
				}
			}
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<dcon::national_identity_id>>()) {
			target_country = any_cast<element_selection_wrapper<dcon::national_identity_id>>(payload).data;

			if(target_country) { // goto next step
				if(military::cb_requires_selection_of_a_state(state, cb_to_use)) {
					wargoal_setup_win->set_visible(state, false);
					wargoal_state_win->set_visible(state, true);
					wargoal_country_win->set_visible(state, false);
					wargoal_target_win->set_visible(state, false);
				} else {
					wargoal_setup_win->set_visible(state, true);
					wargoal_state_win->set_visible(state, false);
					wargoal_country_win->set_visible(state, false);
					wargoal_target_win->set_visible(state, false);
					wargoal_decided_upon = true;
				}
			} else {
				wargoal_decided_upon = false;
				cb_to_use = dcon::cb_type_id{};
				wargoal_setup_win->set_visible(state, true);
				wargoal_state_win->set_visible(state, false);
				wargoal_country_win->set_visible(state, false);
				wargoal_target_win->set_visible(state, false);
			}

			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<set_target>()) {
			wargoal_against = any_cast<set_target>(payload).n;
			if(wargoal_against) {
				wargoal_decided_upon = false;
				cb_to_use = dcon::cb_type_id{};
				wargoal_setup_win->set_visible(state, true);
				wargoal_state_win->set_visible(state, false);
				wargoal_country_win->set_visible(state, false);
				wargoal_target_win->set_visible(state, false);
			}
			return message_result::consumed;
		} else if(payload.holds_type<dcon::cb_type_id>()) {
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

}
