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
					// prevent duplicate war goals
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

									bool dup = false;
									for(auto wg : state.world.war_get_wargoals_attached(war)) {
										if(wg.get_wargoal().get_added_by() == actor && wg.get_wargoal().get_type() == cb && wg.get_wargoal().get_associated_state() == def && wg.get_wargoal().get_target_nation() == v.get_subject()) {
											dup = true;
										}
									}
									if(!dup) {
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
							bool dup = false;

							if(military::cb_requires_selection_of_a_liberatable_tag(state, cb)) {
								for(auto wg : state.world.war_get_wargoals_attached(war)) {
									if(wg.get_wargoal().get_added_by() == actor && wg.get_wargoal().get_type() == cb && wg.get_wargoal().get_associated_tag() == secondary_tag && wg.get_wargoal().get_associated_state() == def && wg.get_wargoal().get_target_nation() == target) {
										dup = true;
									}
								}
							} else {
								for(auto wg : state.world.war_get_wargoals_attached(war)) {
									if(wg.get_wargoal().get_added_by() == actor && wg.get_wargoal().get_type() == cb && wg.get_wargoal().get_secondary_nation() == in_nation && wg.get_wargoal().get_associated_state() == def && wg.get_wargoal().get_target_nation() == target) {
										dup = true;
									}
								}
							}

							if(!dup) {
								row_contents.push_back(def);
							}
						}
					}

				} else {
					for(auto si : state.world.nation_get_state_ownership(target)) {
						if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(actor), trigger::to_generic(actor))) {

							auto def = si.get_state().get_definition().id;
							bool dup = false;
							for(auto wg : state.world.war_get_wargoals_attached(war)) {
								if(wg.get_wargoal().get_added_by() == actor && wg.get_wargoal().get_type() == cb && wg.get_wargoal().get_associated_state() == def && wg.get_wargoal().get_target_nation() == target) {
									dup = true;
								}
							}
							if(!dup) {
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
					if(!war || military::cb_requires_selection_of_a_state(state, cb)) {
						row_contents.push_back(id);
					} else {
						bool dup = false;
						if(military::cb_requires_selection_of_a_liberatable_tag(state, cb)) {
							for(auto wg : state.world.war_get_wargoals_attached(war)) {
								if(wg.get_wargoal().get_added_by() == actor && wg.get_wargoal().get_type() == cb && wg.get_wargoal().get_associated_tag() == id && wg.get_wargoal().get_target_nation() == target) {
									dup = true;
								}
							}
						} else {
							for(auto wg : state.world.war_get_wargoals_attached(war)) {
								if(wg.get_wargoal().get_added_by() == actor && wg.get_wargoal().get_type() == cb && wg.get_wargoal().get_secondary_nation() == n && wg.get_wargoal().get_target_nation() == target) {
									dup = true;
								}
							}
						}
						if(!dup) {
							row_contents.push_back(id);
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
		auto target = retrieve<dcon::nation_id>(state, parent);
		auto source = state.local_player_nation;


		if(cb) {
			float prestige_gain = military::successful_cb_prestige(state, cb, state.local_player_nation);

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
		auto target = retrieve<dcon::nation_id>(state, parent);
		auto source = state.local_player_nation;

		dcon::nation_id real_target = target;

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
		auto target = retrieve<dcon::nation_id>(state, parent);
		auto source = state.local_player_nation;

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
						state.world.national_identity_get_nation_from_identity_holder(ni), retrieve<bool>(state, parent));
			}
			parent->set_visible(state, false);
		}
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
				auto is_attacker = military::is_attacker(state, war, state.local_player_nation);
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
		auto war = retrieve<dcon::war_id>(state, parent);
		show = bool(war);
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
				wargoal_setup_win->set_visible(state, true);
				wargoal_state_win->set_visible(state, false);
				wargoal_country_win->set_visible(state, false);
				wargoal_decided_upon = true;
			}
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<dcon::state_definition_id>>()) {
			target_state = any_cast<element_selection_wrapper<dcon::state_definition_id>>(payload).data;
			if(target_state) {
				wargoal_setup_win->set_visible(state, true);
				wargoal_state_win->set_visible(state, false);
				wargoal_country_win->set_visible(state, false);
				wargoal_decided_upon = true;
			} else {
				if(military::cb_requires_selection_of_a_valid_nation(state, cb_to_use) || military::cb_requires_selection_of_a_liberatable_tag(state, cb_to_use)) {
					wargoal_setup_win->set_visible(state, false);
					wargoal_state_win->set_visible(state, false);
					wargoal_country_win->set_visible(state, true);
				} else {
					wargoal_setup_win->set_visible(state, true);
					wargoal_state_win->set_visible(state, false);
					wargoal_country_win->set_visible(state, false);
					wargoal_decided_upon = false;
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
					wargoal_setup_win->set_visible(state, true);
					wargoal_state_win->set_visible(state, false);
					wargoal_country_win->set_visible(state, false);
					wargoal_decided_upon = true;
				}
			} else {
				wargoal_setup_win->set_visible(state, true);
				wargoal_state_win->set_visible(state, false);
				wargoal_country_win->set_visible(state, false);
				wargoal_decided_upon = false;
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


class diplomacy_peace_pick_side_description : public simple_multiline_body_text {
public:
	void populate_layout(sys::state& state, text::endless_layout& contents) noexcept override {
		auto target = retrieve<dcon::nation_id>(state, parent);
		auto war = military::find_war_between(state, target, state.local_player_nation);

		auto we_lead = (state.local_player_nation == state.world.war_get_primary_attacker(war) || state.local_player_nation == state.world.war_get_primary_defender(war));
		auto they_lead = (target == state.world.war_get_primary_attacker(war) || target == state.world.war_get_primary_defender(war));

		text::add_line(state, contents, we_lead ? "po_welead" : "po_wenotlead");
		text::add_line(state, contents, they_lead ? "po_theylead" : "po_theynotlead");
	}
};

template<bool is_concession>
class diplomacy_peace_tab_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<bool>{ is_concession };
			parent->impl_get(state, payload);
		}
	}
	void on_update(sys::state& state) noexcept override {
		frame = (retrieve<bool>(state, parent) == is_concession) ? 1 : 0;
	}
};

class diplomacy_peace_pick_side_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "war_desc") {
			return make_element_by_type<diplomacy_peace_pick_side_description>(state, id);
		} else if(name == "offer_button") {
			return make_element_by_type<diplomacy_peace_tab_button<true>>(state, id);
		} else if(name == "offer_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "demand_button") {
			return make_element_by_type<diplomacy_peace_tab_button<false>>(state, id);
		} else if(name == "demand_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

struct toggled_wargoal {
	dcon::wargoal_id wg;
	bool added = false;
};

struct toggle_wg_state {
	dcon::wargoal_id wg;
};

class peace_wg_from : public flag_button {
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		auto wg = retrieve<dcon::wargoal_id>(state, parent);
		return state.world.nation_get_identity_from_identity_holder(state.world.wargoal_get_target_nation(wg));
	}
};

class peace_wg_to : public flag_button {
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		auto wg = retrieve<dcon::wargoal_id>(state, parent);
		return state.world.nation_get_identity_from_identity_holder(state.world.wargoal_get_added_by(wg));
	}
};

class diplomacy_peace_wargoal_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		dcon::wargoal_id wg = retrieve<dcon::wargoal_id>(state, parent);
		dcon::cb_type_id cbt = state.world.wargoal_get_type(wg);

		text::substitution_map sub;
		text::add_to_substitution_map(sub, text::variable_type::recipient, state.world.wargoal_get_target_nation(wg));
		text::add_to_substitution_map(sub, text::variable_type::from, state.world.wargoal_get_added_by(wg));
		if(state.world.wargoal_get_secondary_nation(wg))
			text::add_to_substitution_map(sub, text::variable_type::third, state.world.wargoal_get_secondary_nation(wg));
		else if(state.world.wargoal_get_associated_tag(wg))
			text::add_to_substitution_map(sub, text::variable_type::third, state.world.wargoal_get_associated_tag(wg));
		text::add_to_substitution_map(sub, text::variable_type::state, state.world.wargoal_get_associated_state(wg));

		set_text(state, text::resolve_string_substitution(state, state.world.cb_type_get_shortest_desc(cbt), sub));
	}
};

class diplomacy_peace_wargoal_score_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto wg = fatten(state.world, retrieve<dcon::wargoal_id>(state, parent));
		int32_t score = military::peace_cost(state, retrieve<dcon::war_id>(state, parent), wg.get_type(), wg.get_added_by(), wg.get_target_nation(), wg.get_secondary_nation(), wg.get_associated_state(), wg.get_associated_tag());
		set_text(state, std::to_string(score));
	}
};

class diplomacy_peace_select_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto wg = retrieve<dcon::wargoal_id>(state, parent);
		Cyto::Any setting = toggled_wargoal{ wg, false };
		if(parent)
			parent->impl_get(state, setting);

		frame = any_cast<toggled_wargoal>(setting).added ? 1 : 0;
	}

	void button_action(sys::state& state) noexcept override {
		auto wg = retrieve<dcon::wargoal_id>(state, parent);
		send(state, parent, toggle_wg_state{wg});
	}
};

class diplomacy_peace_goal_row : public listbox_row_element_base<dcon::wargoal_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "select") {
			return make_element_by_type<diplomacy_peace_select_button>(state, id);
		} else if(name == "country_flag") {
			return make_element_by_type<peace_wg_from>(state, id);
		} else if(name == "country_flag2") {
			return make_element_by_type<peace_wg_to>(state, id);
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

class diplomacy_peace_goal_listbox : public listbox_element_base<diplomacy_peace_goal_row, dcon::wargoal_id> {
protected:
	std::string_view get_row_element_name() override {
		return "peace_goal_item";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		auto is_concession = retrieve<bool>(state, parent);

		auto target = retrieve<dcon::nation_id>(state, parent);
		auto war = military::find_war_between(state, target, state.local_player_nation);

		auto we_lead = (state.local_player_nation == state.world.war_get_primary_attacker(war) || state.local_player_nation == state.world.war_get_primary_defender(war));
		auto they_lead = (target == state.world.war_get_primary_attacker(war) || target == state.world.war_get_primary_defender(war));

		bool player_is_attacker = military::is_attacker(state, war, state.local_player_nation);
		if(they_lead && we_lead && is_concession) {
			for(auto wg : state.world.war_get_wargoals_attached(war)) {
				if(military::get_role(state, war, wg.get_wargoal().get_target_nation()) == (player_is_attacker ? military::war_role::attacker : military::war_role::defender)) {
					row_contents.push_back(wg.get_wargoal().id);
				}
			}
		} else if(they_lead && we_lead && !is_concession) {
			for(auto wg : state.world.war_get_wargoals_attached(war)) {
				if(military::get_role(state, war, wg.get_wargoal().get_target_nation()) == (player_is_attacker ? military::war_role::defender : military::war_role::attacker)) {
					row_contents.push_back(wg.get_wargoal().id);
				}
			}
		} else if(!they_lead && !is_concession) {
			for(auto wg : state.world.war_get_wargoals_attached(war)) {
				if(wg.get_wargoal().get_target_nation() == target || wg.get_wargoal().get_target_nation().get_overlord_as_subject().get_ruler() == target) {
					row_contents.push_back(wg.get_wargoal().id);
				}
			}
		} else if(!we_lead && is_concession) {
			for(auto wg : state.world.war_get_wargoals_attached(war)) {
				if(wg.get_wargoal().get_target_nation() == state.local_player_nation || wg.get_wargoal().get_target_nation().get_overlord_as_subject().get_ruler() == state.local_player_nation) {
					row_contents.push_back(wg.get_wargoal().id);
				}
			}
		} else if(we_lead && is_concession) {
			for(auto wg : state.world.war_get_wargoals_attached(war)) {
				if(wg.get_wargoal().get_added_by() == target || wg.get_wargoal().get_added_by().get_overlord_as_subject().get_ruler() == target) {
					row_contents.push_back(wg.get_wargoal().id);
				}
			}
		} else if(they_lead && !is_concession) {
			for(auto wg : state.world.war_get_wargoals_attached(war)) {
				if(wg.get_wargoal().get_added_by() == state.local_player_nation || wg.get_wargoal().get_added_by().get_overlord_as_subject().get_ruler() == state.local_player_nation) {
					row_contents.push_back(wg.get_wargoal().id);
				}
			}
		} else {
			std::abort(); // should be impossible: neither is war leader
		}
		
		update(state);
	}
};

struct offer_cost {
	int32_t val = 0;
};

class total_peace_cost : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::val, int64_t(retrieve<offer_cost>(state, parent).val));
		set_text(state, text::resolve_string_substitution(state, "calculated_warscore", m));
	}
};

class diplomacy_peace_setup_goals_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_list") {
			return make_element_by_type<diplomacy_peace_goal_listbox>(state, id);
		} else if(name == "score") {
			return make_element_by_type<total_peace_cost>(state, id);
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
		const dcon::war_id w = retrieve<dcon::war_id>(state, parent);
		const dcon::nation_id target = retrieve<dcon::nation_id>(state, parent);
		if(    (state.world.war_get_primary_attacker(w) == target && state.world.war_get_primary_defender(w) == state.local_player_nation)
			|| (state.world.war_get_primary_defender(w) == target && state.world.war_get_primary_attacker(w) == state.local_player_nation)) {

			const int32_t score = int32_t(military::primary_warscore(state, w));
			if(state.world.war_get_primary_attacker(w) == state.local_player_nation)
				set_text(state, std::to_string(score));
			else
				set_text(state, std::to_string(-score));
		} else {
			const int32_t score = int32_t(military::directed_warscore(state, w, state.local_player_nation, target));
			set_text(state, std::to_string(score));
		}
	}
};

struct send_offer { };

class diplomacy_peace_send : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		send(state, parent, send_offer{});
	}
};

class diplomacy_setup_peace_dialog : public window_element_base { // eu3dialogtype
	bool is_concession = false;
	std::vector<toggled_wargoal> wargoals;

public:
	void open_window(sys::state& state) {
		wargoals.clear();

		auto target = retrieve<dcon::nation_id>(state, parent);
		auto war = military::find_war_between(state, state.local_player_nation, target);
		is_concession = false;

		if(state.world.war_get_primary_attacker(war) == state.local_player_nation && state.world.war_get_primary_defender(war) == target) {
			for(auto wg : state.world.war_get_wargoals_attached(war)) {
				wargoals.push_back(toggled_wargoal{ wg.get_wargoal().id, false });
			}
		} else if(state.world.war_get_primary_attacker(war) == target && state.world.war_get_primary_defender(war) == state.local_player_nation) {
			for(auto wg : state.world.war_get_wargoals_attached(war)) {
				wargoals.push_back(toggled_wargoal{ wg.get_wargoal().id, false });
			}
		} else {
			for(auto wg : state.world.war_get_wargoals_attached(war)) {

				if(state.world.war_get_primary_attacker(war) == state.local_player_nation || state.world.war_get_primary_defender(war) == state.local_player_nation) {

					if(wg.get_wargoal().get_added_by() == target || wg.get_wargoal().get_added_by().get_overlord_as_subject().get_ruler() == target || wg.get_wargoal().get_target_nation() == target || wg.get_wargoal().get_target_nation().get_overlord_as_subject().get_ruler() == target) {

						wargoals.push_back(toggled_wargoal{ wg.get_wargoal().id, false });
					}
					
				} else {
					if(wg.get_wargoal().get_added_by() == state.local_player_nation || wg.get_wargoal().get_added_by().get_overlord_as_subject().get_ruler() == state.local_player_nation || wg.get_wargoal().get_target_nation() == state.local_player_nation || wg.get_wargoal().get_target_nation().get_overlord_as_subject().get_ruler() == state.local_player_nation) {

						wargoals.push_back(toggled_wargoal{ wg.get_wargoal().id, false });
					}
				}
			}
		}
	}

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
			!is_concession ? ptr->set_text(state, text::produce_simple_string(state, "demand_peace"))
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
			is_concession = any_cast<element_selection_wrapper<bool>>(payload).data;
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<bool>()) {
			payload.emplace<bool>(is_concession);
			return message_result::consumed;
		} else if(payload.holds_type<toggled_wargoal>()) {
			toggled_wargoal twg = any_cast<toggled_wargoal>(payload);
			for(auto& w : wargoals) {
				if(w.wg == twg.wg) {
					payload.emplace<toggled_wargoal>(w);
					break;
				}
			}
			return message_result::consumed;
		} else if(payload.holds_type<toggle_wg_state>()) {
			toggle_wg_state twg = any_cast<toggle_wg_state>(payload);
			for(auto& w : wargoals) {
				if(w.wg == twg.wg) {
					w.added = !w.added;
					break;
				}
			}
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type< offer_cost>()) {
			int32_t total = 0;
			auto target = retrieve<dcon::nation_id>(state, parent);
			auto war = military::find_war_between(state, state.local_player_nation, target);

			bool attacker_filter = (military::is_attacker(state, war, state.local_player_nation) != is_concession);
			for(auto& twg : wargoals) {
				if(twg.added) {
					auto wg = fatten(state.world, twg.wg);
					if(military::get_role(state, war, wg.get_added_by()) == (attacker_filter ? military::war_role::attacker : military::war_role::defender)) {

						total += military::peace_cost(state, retrieve<dcon::war_id>(state, parent), wg.get_type(), wg.get_added_by(), wg.get_target_nation(), wg.get_secondary_nation(), wg.get_associated_state(), wg.get_associated_tag());
					}
				}
			}

			payload.emplace<offer_cost>(offer_cost{total});
			return message_result::consumed;
		} else if(payload.holds_type<send_offer>()) {
			auto target = retrieve<dcon::nation_id>(state, parent);
			auto war = military::find_war_between(state, state.local_player_nation, target);

			command::start_peace_offer(state, state.local_player_nation, target, war, is_concession);

			bool attacker_filter = (military::is_attacker(state, war, state.local_player_nation) != is_concession);
			for(auto& twg : wargoals) {
				if(twg.added) {
					auto wg = fatten(state.world, twg.wg);
					if(military::get_role(state, war, wg.get_added_by()) == (attacker_filter ? military::war_role::attacker : military::war_role::defender)) {

						command::add_to_peace_offer(state, state.local_player_nation, twg.wg);
					}
				}
			}

			command::send_peace_offer(state, state.local_player_nation);
			set_visible(state, false);

			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

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

			auto selected = retrieve<dcon::cb_type_id>(state, parent->parent);
			disabled = selected == content;
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
	std::string_view get_row_element_name() override {
		return "cb_type_item";
	}

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

class diplomacy_make_cb_desc : public simple_multiline_body_text {
public:
	void populate_layout(sys::state& state, text::endless_layout& contents) noexcept override {

		auto fat_cb = dcon::fatten(state.world, retrieve<dcon::cb_type_id>(state, parent));

		auto box = text::open_layout_box(contents);

		auto fab_time = std::ceil(100.0f / (state.defines.cb_generation_base_speed * fat_cb.get_construction_speed() * (state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::cb_generation_speed_modifier) + 1.0f)));

		if(fat_cb.is_valid()) {
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::type, fat_cb.get_name());
			text::add_to_substitution_map(sub, text::variable_type::days, int64_t(fab_time));
			text::add_to_substitution_map(sub, text::variable_type::badboy, text::fp_one_place{military::cb_infamy(state, fat_cb)});
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
		} else if(payload.holds_type<dcon::cb_type_id>()) {
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
