#pragma once

#include "gui_element_types.hpp"
#include "gui_generic_elements.hpp"

namespace ui {

class diplomacy_crisis_attacker_flag : public flag_button {
public:
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		return state.world.nation_get_identity_from_identity_holder(state.primary_crisis_attacker);
	}
};

class diplomacy_crisis_sponsored_attacker_flag : public flag_button {
public:
	bool show = false;

	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {

		auto first_wg = state.crisis_attacker_wargoals.at(0);
		if(first_wg.cb == state.military_definitions.crisis_colony) {
			auto colonizers = state.world.state_definition_get_colonization(first_wg.state);
			if(colonizers.begin() != colonizers.end()) {
				auto attacking_colonizer = (*colonizers.begin()).get_colonizer();
				return state.world.nation_get_identity_from_identity_holder(attacking_colonizer);
			}
		} else if(first_wg.cb == state.military_definitions.liberate) {
			return first_wg.wg_tag;
		}
		return dcon::national_identity_id{};
	}
	void on_update(sys::state& state) noexcept override {
		flag_button::on_update(state);
		auto first_wg = state.crisis_attacker_wargoals.at(0);
		if(first_wg.cb == state.military_definitions.crisis_colony) {
			auto colonizers = state.world.state_definition_get_colonization(first_wg.state);
			if(colonizers.begin() != colonizers.end()) {
				auto attacking_colonizer = (*colonizers.begin()).get_colonizer();
				show = attacking_colonizer != state.primary_crisis_attacker;
			} else {
				show = false;
			}
		} else if(first_wg.cb == state.military_definitions.liberate) {
			show = first_wg.wg_tag != state.world.nation_get_identity_from_identity_holder(state.primary_crisis_attacker);
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(show)
			flag_button::render(state, x, y);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return show ? tooltip_behavior::variable_tooltip : tooltip_behavior::no_tooltip;
	}
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(!show)
			return mouse_probe{ nullptr, ui::xy_pair{} };
		else
			return flag_button::impl_probe_mouse(state, x, y, type);
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(!show)
			return;
		flag_button::update_tooltip(state, x, y, contents);
	}
};

class support_defender_button : public button_element_base {
	bool show = true;

	void on_update(sys::state& state) noexcept override {
		show = nations::is_involved_in_crisis(state, state.local_player_nation) && !nations::is_committed_in_crisis(state, state.local_player_nation);
		disabled = !command::can_take_sides_in_crisis(state, state.local_player_nation, false);
	}
	void button_action(sys::state& state) noexcept override {
		if(show)
			command::take_sides_in_crisis(state, state.local_player_nation, false);
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(show)
			button_element_base::render(state, x, y);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return show ? tooltip_behavior::variable_tooltip : tooltip_behavior::no_tooltip;
	}
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(!show)
			return mouse_probe{ nullptr, ui::xy_pair{} };
		else
			return button_element_base::impl_probe_mouse(state, x, y, type);
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(!show)
			return;

		text::add_line_with_condition(state, contents, "crisis_back_explain_2", state.current_crisis_state == sys::crisis_state::heating_up);

		for(auto& i : state.crisis_participants) {
			if(i.id == state.local_player_nation) {
				text::add_line_with_condition(state, contents, "crisis_back_explain_1", i.merely_interested == true);
				return;
			}
			if(!i.id) {
				text::add_line_with_condition(state, contents, "crisis_back_explain_1", false);
				return;
			}
		}
	}
};

class support_attacker_button : public button_element_base {
	bool show = true;

	void on_update(sys::state& state) noexcept override {
		show = nations::is_involved_in_crisis(state, state.local_player_nation) && !nations::is_committed_in_crisis(state, state.local_player_nation);
		disabled = !command::can_take_sides_in_crisis(state, state.local_player_nation, true);
	}
	void button_action(sys::state& state) noexcept override {
		if(show)
			command::take_sides_in_crisis(state, state.local_player_nation, true);
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(show)
			button_element_base::render(state, x, y);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return show ? tooltip_behavior::variable_tooltip : tooltip_behavior::no_tooltip;
	}
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(!show)
			return mouse_probe{ nullptr, ui::xy_pair{} };
		else
			return button_element_base::impl_probe_mouse(state, x, y, type);
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(!show)
			return;

		text::add_line_with_condition(state, contents, "crisis_back_explain_2", state.current_crisis_state == sys::crisis_state::heating_up);

		for(auto& i : state.crisis_participants) {
			if(i.id == state.local_player_nation) {
				text::add_line_with_condition(state, contents, "crisis_back_explain_1", i.merely_interested == true);
				return;
			}
			if(!i.id) {
				text::add_line_with_condition(state, contents, "crisis_back_explain_1", false);
				return;
			}
		}
	}
};


class propose_attacker_solution_button : public button_element_base {
	bool show = true;

	void on_update(sys::state& state) noexcept override {
		show = state.local_player_nation == state.primary_crisis_attacker;
	}
	void button_action(sys::state& state) noexcept override {
		if(show)
			send(state, parent, diplomacy_action::crisis_backdown);
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(show)
			button_element_base::render(state, x, y);
	}
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(!show)
			return mouse_probe{ nullptr, ui::xy_pair{} };
		else
			return button_element_base::impl_probe_mouse(state, x, y, type);
	}
};

class propose_defender_solution_button : public button_element_base {
	bool show = true;

	void on_update(sys::state& state) noexcept override {
		show = state.local_player_nation == state.primary_crisis_defender;
	}
	void button_action(sys::state& state) noexcept override {
		if(show)
			send(state, parent, diplomacy_action::crisis_backdown);
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(show)
			button_element_base::render(state, x, y);
	}
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(!show)
			return mouse_probe{ nullptr, ui::xy_pair{} };
		else
			return button_element_base::impl_probe_mouse(state, x, y, type);
	}
};

class attacker_add_wg_button : public button_element_base {
	bool show = true;

	void on_update(sys::state& state) noexcept override {
		show = state.local_player_nation == state.primary_crisis_attacker;
		disabled = state.world.nation_get_diplomatic_points(state.local_player_nation) < 1.0f;
	}
	void button_action(sys::state& state) noexcept override {
		if(show)
			send(state, parent, diplomacy_action::crisis_add_wargoal);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "crisis_offer_button");
		text::add_line_break_to_layout(state, contents);
		text::add_line_with_condition(state, contents, "crisis_offer_button_ex_3", state.world.nation_get_diplomatic_points(state.local_player_nation) >= 1.0f);
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(show)
			button_element_base::render(state, x, y);
	}
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(!show)
			return mouse_probe{ nullptr, ui::xy_pair{} };
		else
			return button_element_base::impl_probe_mouse(state, x, y, type);
	}
};

class defender_add_wg_button : public button_element_base {
	bool show = true;

	void on_update(sys::state& state) noexcept override {
		show = state.local_player_nation == state.primary_crisis_defender;
		disabled = state.world.nation_get_diplomatic_points(state.local_player_nation) < 1.0f;
	}
	void button_action(sys::state& state) noexcept override {
		if(show)
			send(state, parent, diplomacy_action::crisis_add_wargoal);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "crisis_offer_button");
		text::add_line_break_to_layout(state, contents);
		text::add_line_with_condition(state, contents, "crisis_offer_button_ex_3", state.world.nation_get_diplomatic_points(state.local_player_nation) >= 1.0f);
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(show)
			button_element_base::render(state, x, y);
	}
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(!show)
			return mouse_probe{ nullptr, ui::xy_pair{} };
		else
			return button_element_base::impl_probe_mouse(state, x, y, type);
	}
};

class crisis_add_wargoal_confirm_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto wg_ready = retrieve<check_wg_completion>(state, parent).done;
		if(!wg_ready) {
			disabled = true;
			return;
		}
		auto ni = retrieve<dcon::national_identity_id>(state, parent);
		disabled = !command::crisis_can_add_wargoal(state, state.local_player_nation,
		sys::full_wg{
			state.local_player_nation, // added_by;
			retrieve<get_target>(state, parent).n, // target_nation;
			state.world.national_identity_get_nation_from_identity_holder(ni), //  secondary_nation;
			ni, // wg_tag;
			retrieve<dcon::state_definition_id>(state, parent), // state;
			retrieve<dcon::cb_type_id>(state, parent) // cb
		});
	}

	void button_action(sys::state& state) noexcept override {
		auto ni = retrieve<dcon::national_identity_id>(state, parent);
		command::queue_crisis_add_wargoal(state, state.local_player_nation, sys::full_wg{
			state.local_player_nation, // added_by;
			retrieve<get_target>(state, parent).n, // target_nation;
			state.world.national_identity_get_nation_from_identity_holder(ni), //  secondary_nation;
			ni, // wg_tag;
			retrieve<dcon::state_definition_id>(state, parent), // state;
			retrieve<dcon::cb_type_id>(state, parent) // cb
		});
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

class crisis_add_wargoal_window : public window_element_base {
private:
	wargoal_offer_setup_window* wargoal_setup_win = nullptr;
	wargoal_offer_country_select_window* wargoal_country_win = nullptr;
	wargoal_target_country_select_window* wargoal_target_win = nullptr;

	dcon::nation_id offer_made_to;
	dcon::nation_id wargoal_against;
	dcon::cb_type_id cb_to_use;
	dcon::state_definition_id target_state;
	dcon::national_identity_id secondary_tag_identity;
	dcon::national_identity_id wg_tag;

	bool will_call_allies = false;
	bool wargoal_decided_upon = false;

	void select_mode(sys::state& state) {
		sys::state_selection_data seldata;
		seldata.single_state_select = true;
		// Populate selectable states...
		dcon::nation_id target = wargoal_against;
		auto actor = state.local_player_nation;
		dcon::cb_type_id cb = cb_to_use;
		auto allowed_substate_regions = state.world.cb_type_get_allowed_substate_regions(cb);
		if(allowed_substate_regions) {
			for(auto v : state.world.nation_get_overlord_as_ruler(target)) {
				if(v.get_subject().get_is_substate()) {
					auto secondary_tag = state.world.national_identity_get_nation_from_identity_holder(secondary_tag_identity);
					for(auto si : state.world.nation_get_state_ownership(target)) {
						if(trigger::evaluate(state, allowed_substate_regions, trigger::to_generic(si.get_state().id), trigger::to_generic(actor), trigger::to_generic(actor)) &&
							command::crisis_can_add_wargoal(state, actor, sys::full_wg{
								actor, // added_by;
								target, // target_nation;
								secondary_tag, //  secondary_nation;
								wg_tag, // wg_tag;
								si.get_state().get_definition(), // state;
								cb // cb
								})) {
							seldata.selectable_states.push_back(si.get_state().get_definition().id);
						}
					}
				}
			}
		} else {
			auto allowed_states = state.world.cb_type_get_allowed_states(cb);
			if(auto allowed_countries = state.world.cb_type_get_allowed_countries(cb); allowed_countries) {
				auto secondary_tag = state.world.national_identity_get_nation_from_identity_holder(secondary_tag_identity);
				for(auto si : state.world.nation_get_state_ownership(target)) {
					if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(actor), trigger::to_generic(secondary_tag)) &&
						command::crisis_can_add_wargoal(state, actor, sys::full_wg{
								actor, // added_by;
								target, // target_nation;
								secondary_tag, //  secondary_nation;
								wg_tag, // wg_tag;
								si.get_state().get_definition(), // state;
								cb // cb
						})) {
						seldata.selectable_states.push_back(si.get_state().get_definition().id);
					}
				}
			} else {
				for(auto si : state.world.nation_get_state_ownership(target)) {
					auto secondary_tag = state.world.national_identity_get_nation_from_identity_holder(secondary_tag_identity);

					if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(actor), trigger::to_generic(actor)) &&
						command::crisis_can_add_wargoal(state, actor, sys::full_wg{
								actor, // added_by;
								target, // target_nation;
								secondary_tag, //  secondary_nation;
								wg_tag, // wg_tag;
								si.get_state().get_definition(), // state;
								cb // cb
						})) {
						seldata.selectable_states.push_back(si.get_state().get_definition().id);
					}
				}
			}
		}
		seldata.on_select = [&](sys::state& state, dcon::state_definition_id sdef) {
			target_state = sdef;
			wargoal_decided_upon = true;
			wargoal_setup_win->set_visible(state, true);
			wargoal_country_win->set_visible(state, false);
			impl_on_update(state);
			};
		seldata.on_cancel = [&](sys::state& state) {
			target_state = dcon::state_definition_id{};
			if(military::cb_requires_selection_of_a_valid_nation(state, cb_to_use) || military::cb_requires_selection_of_a_liberatable_tag(state, cb_to_use)) {
				wargoal_setup_win->set_visible(state, false);
				//wargoal_state_win->set_visible(state, false);
				wargoal_country_win->set_visible(state, true);
			} else {
				cb_to_use = dcon::cb_type_id{};
				wargoal_decided_upon = false;
				wargoal_setup_win->set_visible(state, true);
				//wargoal_state_win->set_visible(state, false);
				wargoal_country_win->set_visible(state, false);
			}
			impl_on_update(state);
			};
		state.start_state_selection(seldata);
	}
public:
	void reset_window(sys::state& state) {
		wargoal_against = dcon::nation_id{};
		offer_made_to = state.local_player_nation;
		cb_to_use = dcon::cb_type_id{};
		target_state = dcon::state_definition_id{};
		secondary_tag_identity = dcon::national_identity_id{};
		will_call_allies = false;
		wargoal_decided_upon = false;

		wargoal_setup_win->set_visible(state, false);
		//wargoal_state_win->set_visible(state, false);
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
		} else if(name == "wargoal_success_effect" || name == "wargoal_failure_effect") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "description2") {
			return make_element_by_type<wargoal_offer_description2>(state, id);
		} else if(name == "acceptance") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "call_allies_checkbox") {
			return make_element_by_type<invisible_element>(state, id);
		} if(name == "call_allies_checkbox") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "run_conference_checkbox") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "agreebutton") {
			return make_element_by_type<crisis_add_wargoal_confirm_button>(state, id);
		} else if(name == "declinebutton") {
			return make_element_by_type<cancel_pick_wg_button>(state, id);
		} else if(name == "wargoal_setup") {
			auto ptr = make_element_by_type<wargoal_target_country_select_window>(state, id);
			wargoal_target_win = ptr.get();
			ptr->set_visible(state, true);
			return ptr;
		} else if(name == "wargoal_state_select") {
			return make_element_by_type<invisible_element>(state, id);
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
				secondary_tag_identity = dcon::national_identity_id{};
				wargoal_decided_upon = false;

				wargoal_setup_win->set_visible(state, false);
				//wargoal_state_win->set_visible(state, false);
				wargoal_country_win->set_visible(state, false);
				wargoal_target_win->set_visible(state, true);
			} else if(military::cb_requires_selection_of_a_valid_nation(state, cb_to_use) || military::cb_requires_selection_of_a_liberatable_tag(state, cb_to_use)) {
				wargoal_setup_win->set_visible(state, false);
				//wargoal_state_win->set_visible(state, false);
				wargoal_country_win->set_visible(state, true);
				wargoal_target_win->set_visible(state, false);
			} else if(military::cb_requires_selection_of_a_state(state, cb_to_use)) {
				wargoal_setup_win->set_visible(state, false);
				//wargoal_state_win->set_visible(state, true);
				wargoal_country_win->set_visible(state, false);
				wargoal_target_win->set_visible(state, false);
				select_mode(state);
			} else {
				wargoal_decided_upon = true;
				wargoal_setup_win->set_visible(state, true);
				//wargoal_state_win->set_visible(state, false);
				wargoal_country_win->set_visible(state, false);
				wargoal_target_win->set_visible(state, false);
			}
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<dcon::national_identity_id>>()) {
			secondary_tag_identity = any_cast<element_selection_wrapper<dcon::national_identity_id>>(payload).data;

			if(secondary_tag_identity) { // goto next step
				if(military::cb_requires_selection_of_a_state(state, cb_to_use)) {
					wargoal_setup_win->set_visible(state, true);
					//wargoal_state_win->set_visible(state, true);
					wargoal_country_win->set_visible(state, false);
					wargoal_target_win->set_visible(state, true);
					select_mode(state);
				} else {
					wargoal_setup_win->set_visible(state, true);
					//wargoal_state_win->set_visible(state, false);
					wargoal_country_win->set_visible(state, false);
					wargoal_target_win->set_visible(state, false);
					wargoal_decided_upon = true;
				}
			} else {
				wargoal_decided_upon = false;
				cb_to_use = dcon::cb_type_id{};
				wargoal_setup_win->set_visible(state, true);
				//wargoal_state_win->set_visible(state, false);
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
				//wargoal_state_win->set_visible(state, false);
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
			payload.emplace<dcon::national_identity_id>(secondary_tag_identity);
			return message_result::consumed;
		} else if(payload.holds_type<bool>()) {
			payload.emplace<bool>(will_call_allies);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class diplomacy_crisis_attacker_name : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::produce_simple_string(state, text::get_name(state, state.primary_crisis_attacker)));
	}
};

class diplomacy_crisis_attacker_backers : public overlapping_flags_box {
protected:
	void populate_flags(sys::state& state) noexcept override {
		row_contents.clear();
		for(uint32_t i = 0; i < state.crisis_participants.size(); i++) {
			if(!state.crisis_participants[i].id)
				break;

			if(state.crisis_participants[i].supports_attacker && !state.crisis_participants[i].merely_interested && state.crisis_participants[i].id != state.primary_crisis_attacker) {
				auto content = dcon::fatten(state.world, state.crisis_participants[i].id);
				row_contents.push_back(content.get_identity_from_identity_holder().id);
			}
		}
		update(state);
	}
};

class crisis_attacker_wargoals : public overlapping_listbox_element_base<overlapping_full_wg_icon, sys::full_wg> {
protected:
	std::string_view get_row_element_name() override {
		return "wargoal";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();

		for(auto wg : state.crisis_attacker_wargoals) {
			if(wg.cb) {
				row_contents.push_back(wg);
			}
			else {
				break;
			}
		}

		update(state);

	}
};

class diplomacy_crisis_attacker_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "country_flag") {
			return make_element_by_type<diplomacy_crisis_attacker_flag>(state, id);
		} else if(name == "sponsored_flag") { 
			return make_element_by_type<diplomacy_crisis_sponsored_attacker_flag>(state, id);
		} else if(name == "country_name") {
			return make_element_by_type<diplomacy_crisis_attacker_name>(state, id);
		} else if(name == "wargoals") {
			return make_element_by_type<crisis_attacker_wargoals>(state, id);
		} else if(name == "backers") {
			auto ptr = make_element_by_type<diplomacy_crisis_attacker_backers>(state, id);
			ptr->base_data.position.y -= 6;
			return ptr;
		} else if(name == "support_side") {
			return make_element_by_type<support_attacker_button>(state, id);
		} else if(name == "back_down") {
			return make_element_by_type<propose_defender_solution_button>(state, id);
		} else if(name == "crisis_add_wg") {
			return make_element_by_type<attacker_add_wg_button>(state, id);
		} else {
			return nullptr;
		}
	}
};

class diplomacy_crisis_defender_flag : public flag_button {
public:
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		return state.world.nation_get_identity_from_identity_holder(state.primary_crisis_defender);
	}
};

class diplomacy_crisis_sponsored_defender_flag : public flag_button {
public:
	bool show = false;

	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		auto first_wg = state.crisis_attacker_wargoals.at(0);

		if(first_wg.cb == state.military_definitions.crisis_colony) {
			auto colonizers = state.world.state_definition_get_colonization(first_wg.state);
			if(colonizers.end() - colonizers.begin() >= 2) {
				auto def_colonizer = (*(colonizers.begin() + 1)).get_colonizer();
				return state.world.nation_get_identity_from_identity_holder(def_colonizer);
			}
		} else if(first_wg.cb == state.military_definitions.liberate) {
			return first_wg.wg_tag;
		}
		return dcon::national_identity_id{};
	}
	void on_update(sys::state& state) noexcept override {
		flag_button::on_update(state);

		auto first_wg = state.crisis_attacker_wargoals.at(0);

		if(first_wg.cb == state.military_definitions.crisis_colony) {
			auto colonizers = state.world.state_definition_get_colonization(first_wg.state);
			if(colonizers.end() - colonizers.begin() >= 2) {
				auto def_colonizer = (*(colonizers.begin() + 1)).get_colonizer();
				show = def_colonizer != state.primary_crisis_defender;
			} else {
				show = false;
			}
		} else if(first_wg.cb == state.military_definitions.liberate) {
			show = first_wg.target_nation != state.primary_crisis_defender;
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(show)
			flag_button::render(state, x, y);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return show ? tooltip_behavior::variable_tooltip : tooltip_behavior::no_tooltip;
	}
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(!show)
			return mouse_probe{ nullptr, ui::xy_pair{} };
		else
			return flag_button::impl_probe_mouse(state, x, y, type);
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(!show)
			return;
		flag_button::update_tooltip(state, x, y, contents);
	}
};

class diplomacy_crisis_defender_name : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::produce_simple_string(state, text::get_name(state, state.primary_crisis_defender)));
	}
};

class diplomacy_crisis_defender_backers : public overlapping_flags_box {
protected:
	void populate_flags(sys::state& state) noexcept override {
		row_contents.clear();
		for(uint32_t i = 0; i < state.crisis_participants.size(); i++) {
			if(!state.crisis_participants[i].id)
				break;

			if(!state.crisis_participants[i].supports_attacker && !state.crisis_participants[i].merely_interested && state.crisis_participants[i].id != state.primary_crisis_defender) {
				auto content = dcon::fatten(state.world, state.crisis_participants[i].id);
				row_contents.push_back(content.get_identity_from_identity_holder().id);
			}
		}
		update(state);
	}
};


class crisis_defender_wargoals : public overlapping_listbox_element_base<overlapping_full_wg_icon, sys::full_wg> {
protected:
	std::string_view get_row_element_name() override {
		return "wargoal";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();

		for(auto wg : state.crisis_defender_wargoals) {
			if(wg.cb) {
				row_contents.push_back(wg);
			}
			else {
				break;
			}
		}

		update(state);

	}
};

class diplomacy_crisis_defender_window : public window_element_base {
public:
	std::function<void(generic_callback_button&, sys::state&)> on_add_wg_action = [&](generic_callback_button& btn, sys::state& state) {
		send(state, parent, diplomacy_action::add_wargoal);
		};
	std::function<void(generic_callback_button&, sys::state&)> on_add_wg_update = [&](generic_callback_button& btn, sys::state& state) {
		btn.disabled = (state.local_player_nation == state.primary_crisis_attacker);
		};

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "country_flag") {
			return make_element_by_type<diplomacy_crisis_defender_flag>(state, id);
		} else if(name == "sponsored_flag") {
			return make_element_by_type<diplomacy_crisis_sponsored_defender_flag>(state, id);
		} else if(name == "country_name") {
			return make_element_by_type<diplomacy_crisis_defender_name>(state, id);
		} else if(name == "wargoals") {
			return make_element_by_type<crisis_defender_wargoals>(state, id);
		} else if(name == "backers") {
			auto ptr = make_element_by_type<diplomacy_crisis_defender_backers>(state, id);
			ptr->base_data.position.y -= 6;
			return ptr;
		} else if(name == "support_side") {
			return make_element_by_type<support_defender_button>(state, id);
		} else if(name == "back_down") {
			return make_element_by_type<propose_attacker_solution_button>(state, id);
		} else if(name == "crisis_add_wg") {
			return make_element_by_type<defender_add_wg_button>(state, id);
		} else {
			return nullptr;
		}
	}
};

class diplomacy_crisis_title_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto first_wg = state.crisis_attacker_wargoals.at(0);
		set_text(state, text::produce_simple_string(state, state.world.state_definition_get_name(first_wg.state)));
	}
};

class diplomacy_crisis_subtitle_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto first_wg = state.crisis_attacker_wargoals.at(0);
		if (first_wg.cb == state.military_definitions.liberate) {
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::country, first_wg.wg_tag);
			if(state.world.nation_get_owned_province_count(state.world.national_identity_get_nation_from_identity_holder(first_wg.wg_tag)) > 0) {
				set_text(state, text::resolve_string_substitution(state, "crisis_description_reclaim", m));
			} else {
				set_text(state, text::resolve_string_substitution(state, "crisis_description_liberation", m));
			}
		}
		else if(first_wg.cb == state.military_definitions.crisis_colony) {
			set_text(state, text::produce_simple_string(state, "crisis_description_colonize"));
		}
	}
};

class diplomacy_crisis_temperature_bar : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		progress = state.current_crisis_state == sys::crisis_state::heating_up ? state.crisis_temperature / 100.0f : 0.0f;
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return state.current_crisis_state == sys::crisis_state::heating_up ? tooltip_behavior::variable_tooltip : tooltip_behavior::no_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(state.current_crisis_state != sys::crisis_state::heating_up)
			return;
		text::add_line(state, contents, "crisis_temperature_ex", text::variable_type::value, int64_t(state.crisis_temperature));
	}
};

class diplomacy_crisis_status_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		switch(state.current_crisis_state) {
		case sys::crisis_state::inactive:
			set_text(state, text::produce_simple_string(state, "crisis_mode_no_crisis"));
			break;
		case sys::crisis_state::finding_attacker:
			set_text(state, text::produce_simple_string(state, "crisis_mode_finding_attacker"));
			break;
		case sys::crisis_state::finding_defender:
			set_text(state, text::produce_simple_string(state, "crisis_mode_finding_defender"));
			break;
		case sys::crisis_state::heating_up:
			set_text(state, text::produce_simple_string(state, "crisis_mode_heating_up"));
			break;
		default:
			break;
		}
	}
};

class interested_flag : public flag_button {
public:
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		return state.world.nation_get_identity_from_identity_holder(n);
	}
};

struct open_offer_window {
	dcon::nation_id to;
};

class make_offer_button : public button_element_base {
public:

	void on_update(sys::state& state) noexcept override {
		disabled = state.world.nation_get_diplomatic_points(state.local_player_nation) < 1.0f || (state.local_player_nation != state.primary_crisis_attacker && state.local_player_nation != state.primary_crisis_defender) || (state.current_crisis_state != sys::crisis_state::heating_up);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void button_action(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		if(n)
			send(state, parent, open_offer_window{ n });
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "crisis_offer_button");
		text::add_line_break_to_layout(state, contents);
		text::add_line_with_condition(state, contents, "crisis_offer_button_ex_1", state.local_player_nation == state.primary_crisis_attacker || state.local_player_nation == state.primary_crisis_defender);
		text::add_line_with_condition(state, contents, "crisis_offer_button_ex_2", state.current_crisis_state == sys::crisis_state::heating_up);
		text::add_line_with_condition(state, contents, "crisis_offer_button_ex_3", state.world.nation_get_diplomatic_points(state.local_player_nation) >= 1.0f);
	}
};

class join_crisis_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		command::become_interested_in_crisis(state, state.local_player_nation);
	}
	void on_update(sys::state& state) noexcept override {
		disabled = !(state.world.nation_get_is_great_power(state.local_player_nation) && state.current_crisis_state == sys::crisis_state::heating_up);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "crisis_add_interest_button");
		text::add_line_break_to_layout(state, contents);
		text::add_line_with_condition(state, contents, "crisis_add_interest_button_ex_1", state.world.nation_get_is_great_power(state.local_player_nation));
		text::add_line_with_condition(state, contents, "crisis_add_interest_button_ex_2", state.current_crisis_state == sys::crisis_state::heating_up);
		text::add_line_with_condition(state, contents, "crisis_add_interest_button_ex_3", state.world.nation_get_diplomatic_points(state.local_player_nation) >= 1.0f);
	}
};

class diplomacy_crisis_interested_container : public window_element_base {
public:
	bool show = true;
	int32_t index = 0;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "country_flag") {
			return make_element_by_type<interested_flag>(state, id);
		} else if(name == "make_offer") {
			return make_element_by_type<make_offer_button>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			dcon::nation_id n;
			int32_t count = 0;
			for(auto& par: state.crisis_participants) {
				if(!par.id)
					break;
				if(par.merely_interested) {
					if(count == index) {
						n = par.id;
						break;
					}
					++count;
				}
			}
			payload.emplace<dcon::nation_id>(n);
			return message_result::consumed;
		}
		return message_result::unseen;
	}

	void on_update(sys::state& state) noexcept override {
		show = false;
		int32_t count = 0;
		for(auto& par : state.crisis_participants) {
			if(!par.id)
				break;
			if(par.merely_interested) {
				if(count == index) {
					show = true;
					break;
				}
				++count;
			}
		}
	}
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(show)
			window_element_base::impl_render(state, x, y);
	}
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(!show)
			return mouse_probe{ nullptr, ui::xy_pair{} };
		else
			return window_element_base::impl_probe_mouse(state, x, y, type);
	}
};

class diplomacy_crisis_join_container : public window_element_base {
public:
	bool show = true;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "join_button") {
			return make_element_by_type<join_crisis_button>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		show = false;
		if(!state.world.nation_get_is_great_power(state.local_player_nation))
			return;
		if(nations::is_involved_in_crisis(state, state.local_player_nation)) {
			return;
		}
		show = true;
	}
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(show)
			window_element_base::impl_render(state, x, y);
	}
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(!show)
			return mouse_probe{ nullptr, ui::xy_pair{} };
		else
			return window_element_base::impl_probe_mouse(state, x, y, type);
	}
};

class diplomacy_crisis_interested_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		{
			auto ptr = make_element_by_type<diplomacy_crisis_interested_container>(state, state.ui_state.defs_by_name.find(state.lookup_key("fence_sitter_entry"))->second.definition);
			ptr->base_data.position.x = int16_t(0);
			ptr->base_data.position.y = int16_t(0);
			ptr->index = 0;
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<diplomacy_crisis_interested_container>(state, state.ui_state.defs_by_name.find(state.lookup_key("fence_sitter_entry"))->second.definition);
			ptr->base_data.position.x = int16_t(70 * 1);
			ptr->base_data.position.y = int16_t(0);
			ptr->index = 1;
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<diplomacy_crisis_interested_container>(state, state.ui_state.defs_by_name.find(state.lookup_key("fence_sitter_entry"))->second.definition);
			ptr->base_data.position.x = int16_t(70 * 2);
			ptr->base_data.position.y = int16_t(0);
			ptr->index = 2;
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<diplomacy_crisis_interested_container>(state, state.ui_state.defs_by_name.find(state.lookup_key("fence_sitter_entry"))->second.definition);
			ptr->base_data.position.x = int16_t(0);
			ptr->base_data.position.y = int16_t(75);
			ptr->index = 3;
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<diplomacy_crisis_interested_container>(state, state.ui_state.defs_by_name.find(state.lookup_key("fence_sitter_entry"))->second.definition);
			ptr->base_data.position.x = int16_t(70 * 1);
			ptr->base_data.position.y = int16_t(75);
			ptr->index = 4;
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<diplomacy_crisis_interested_container>(state, state.ui_state.defs_by_name.find(state.lookup_key("fence_sitter_entry"))->second.definition);
			ptr->base_data.position.x = int16_t(70 * 2);
			ptr->base_data.position.y = int16_t(75);
			ptr->index = 5;
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<diplomacy_crisis_join_container>(state, state.ui_state.defs_by_name.find(state.lookup_key("join_fence_sitter_entry"))->second.definition);
			ptr->base_data.position.x = int16_t(70 * 2 - 3);
			ptr->base_data.position.y = int16_t(58);
			add_child_to_front(std::move(ptr));
		}
	}
};

class diplomacy_crisis_info_window : public window_element_base {
public:

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "crisis_title") {
			return make_element_by_type<diplomacy_crisis_title_text>(state, id);
		} else if(name == "crisis_sub_title") {
			return make_element_by_type<diplomacy_crisis_subtitle_text>(state, id);
		} else if(name == "crisis_temperature") {
			return make_element_by_type<diplomacy_crisis_temperature_bar>(state, id);
		} else if(name == "crisis_status_label") {
			return make_element_by_type<diplomacy_crisis_status_text>(state, id);
		} else if(name == "attacker_win") {
			return make_element_by_type<diplomacy_crisis_attacker_window>(state, id);
		} else if(name == "defender_win") {
			return make_element_by_type<diplomacy_crisis_defender_window>(state, id);
		} else if(name == "fence_sitters_win") {
			return make_element_by_type< diplomacy_crisis_interested_window>(state, id);
		} else {
			return nullptr;
		}
	}
};

} // namespace ui
