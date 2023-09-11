#pragma once

#include "gui_element_types.hpp"

namespace ui {

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

struct test_acceptance {
	bool result = false;
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
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto sent_to = retrieve<dcon::nation_id>(state, parent);
		if(state.world.nation_get_is_player_controlled(sent_to)) {
			// no tooltip -- no expected acceptance from players
			return;
		}
		auto res = retrieve<test_acceptance>(state, parent);
		if(res.result) {
			text::add_line(state, contents, "ai_will_accept_po");
		} else {
			text::add_line(state, contents, "ai_will_not_accept_po");
		}
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
		} else if(payload.holds_type<test_acceptance>()) { // test_acceptance
			auto target = retrieve<dcon::nation_id>(state, parent);
			const dcon::war_id w = military::find_war_between(state, state.local_player_nation, target);
			auto war_duration = state.current_date.value - state.world.war_get_start_date(w).value;

			int32_t target_personal_po_value = 0;
			int32_t potential_peace_score_against = 0;
			int32_t my_side_against_target = 0;
			int32_t my_side_peace_cost = !military::is_attacker(state, w, state.local_player_nation) ? military::attacker_peace_cost(state, w) : military::defender_peace_cost(state, w);
			int32_t overall_po_value = 0;
			int32_t my_po_target = 0;
			auto prime_attacker = state.world.war_get_primary_attacker(w);
			auto prime_defender = state.world.war_get_primary_defender(w);

			for(auto& twg : wargoals) {
				auto wg = fatten(state.world, twg.wg);
				auto wg_value = military::peace_cost(state, w, wg.get_type(), wg.get_added_by(), wg.get_target_nation(), wg.get_secondary_nation(), wg.get_associated_state(), wg.get_associated_tag());

				if(twg.added) {
					overall_po_value += wg_value;
					if(wg.get_target_nation() == target) {
						target_personal_po_value += wg_value;
					}
					if(wg.get_target_nation() == state.local_player_nation) {
						my_side_against_target += wg_value;
					}
				}
				if(wg.get_target_nation() == target || wg.get_added_by() == target) {
					if(wg.get_target_nation() == target && (wg.get_added_by() == state.local_player_nation || state.local_player_nation == prime_attacker || state.local_player_nation == prime_defender)) {
						potential_peace_score_against += wg_value;
					}
					if(wg.get_added_by() == target && (wg.get_target_nation() == state.local_player_nation || state.local_player_nation == prime_attacker || state.local_player_nation == prime_defender)) {
						my_po_target += wg_value;
					}
				}
			}

			auto acceptance = ai::will_accept_peace_offer_value(state,
				target, state.local_player_nation,
				prime_attacker, prime_defender,
				military::primary_warscore(state, w), military::directed_warscore(state, w, state.local_player_nation, target),
				military::is_attacker(state, w, state.local_player_nation), is_concession,
				overall_po_value, my_po_target,
				target_personal_po_value, potential_peace_score_against,
				my_side_against_target, my_side_peace_cost,
				war_duration);

			payload.emplace<test_acceptance>(test_acceptance{ acceptance });
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};


struct toggled_crisis_wargoal {
	int32_t index = 0;
	bool added = false;
};

struct toggle_crisis_wargoal {
	int32_t index = 0;
};

class diplomacy_crisis_peace_total_warscore : public simple_text_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		set_visible(state, false);
	}
};

class crisis_peace_wg_from : public flag_button {
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		auto wg = nations::get_nth_crisis_war_goal(state, retrieve<int32_t>(state, parent));
		return state.world.nation_get_identity_from_identity_holder(wg.target_nation);
	}
};

class crisis_peace_wg_to : public flag_button {
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		auto wg = nations::get_nth_crisis_war_goal(state, retrieve<int32_t>(state, parent));
		return state.world.nation_get_identity_from_identity_holder(wg.added_by);
	}
};

class diplomacy_crisis_peace_wargoal_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto wg = nations::get_nth_crisis_war_goal(state, retrieve<int32_t>(state, parent));
		dcon::cb_type_id cbt = wg.cb;

		text::substitution_map sub;
		text::add_to_substitution_map(sub, text::variable_type::recipient, wg.target_nation);
		text::add_to_substitution_map(sub, text::variable_type::from, wg.added_by);
		if(wg.secondary_nation)
			text::add_to_substitution_map(sub, text::variable_type::third, wg.secondary_nation);
		else if(wg.wg_tag)
			text::add_to_substitution_map(sub, text::variable_type::third, wg.wg_tag);
		text::add_to_substitution_map(sub, text::variable_type::state, wg.state);

		set_text(state, text::resolve_string_substitution(state, state.world.cb_type_get_shortest_desc(cbt), sub));
	}
};

class diplomacy_crisis_peace_wargoal_score_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto wg = nations::get_nth_crisis_war_goal(state, retrieve<int32_t>(state, parent));
		int32_t score = military::peace_cost(state, dcon::war_id{}, wg.cb, wg.added_by, wg.target_nation, wg.secondary_nation, wg.state, wg.wg_tag);
		set_text(state, std::to_string(score));
	}
};

class diplomacy_crisis_peace_select_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto wg = retrieve<int32_t>(state, parent);
		Cyto::Any setting = toggled_crisis_wargoal{ wg, false };
		if(parent)
			parent->impl_get(state, setting);

		frame = any_cast<toggled_crisis_wargoal>(setting).added ? 1 : 0;
	}

	void button_action(sys::state& state) noexcept override {
		auto wg = retrieve<int32_t>(state, parent);
		send(state, parent, toggle_crisis_wargoal{ wg });
	}
};

class diplomacy_crisis_peace_goal_row : public listbox_row_element_base<int32_t> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "select") {
			return make_element_by_type<diplomacy_crisis_peace_select_button>(state, id);
		} else if(name == "country_flag") {
			return make_element_by_type<crisis_peace_wg_from>(state, id);
		} else if(name == "country_flag2") {
			return make_element_by_type<crisis_peace_wg_to>(state, id);
		} else if(name == "select_label") {
			return make_element_by_type<diplomacy_crisis_peace_wargoal_text>(state, id);
		} else if(name == "score") {
			return make_element_by_type<diplomacy_crisis_peace_wargoal_score_text>(state, id);
		} else {
			return nullptr;
		}
	}
};

class diplomacy_crisis_peace_goal_listbox : public listbox_element_base<diplomacy_crisis_peace_goal_row, int32_t> {
protected:
	std::string_view get_row_element_name() override {
		return "peace_goal_item";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		auto is_concession = retrieve<bool>(state, parent);

		bool attacker_filter = ((state.local_player_nation == state.primary_crisis_attacker) != is_concession);
		auto count = nations::num_crisis_wargoals(state);
		for(int32_t i = 0; i < count; ++i) {
			if(nations::nth_crisis_war_goal_is_for_attacker(state, i) == attacker_filter) {
				row_contents.push_back(i);
			}
		}

		update(state);
	}
};

class diplomacy_crisis_peace_setup_goals_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "wargoal_list") {
			return make_element_by_type<diplomacy_crisis_peace_goal_listbox>(state, id);
		} else if(name == "score") {
			return make_element_by_type<total_peace_cost>(state, id);
		} else {
			return nullptr;
		}
	}
};

class crisis_resolution_dialog : public window_element_base { // eu3dialogtype
	bool is_concession = false;
	std::vector<bool> wargoals;

public:
	void open_window(sys::state& state) {
		wargoals.clear();
		wargoals.resize(nations::num_crisis_wargoals(state), false);
		is_concession = false;
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
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			!is_concession ? ptr->set_text(state, text::produce_simple_string(state, "demand_peace"))
				: ptr->set_text(state, text::produce_simple_string(state, "offer_peace"));
			return ptr;
		} else if(name == "warscore_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "warscore") {
			return make_element_by_type<diplomacy_crisis_peace_total_warscore>(state, id);
		} else if(name == "agreebutton") {
			return make_element_by_type<diplomacy_peace_send>(state, id);
		} else if(name == "declinebutton") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "pick_side") {
			return make_element_by_type<diplomacy_peace_pick_side_window>(state, id);
		} else if(name == "setup_goals") {
			return make_element_by_type<diplomacy_crisis_peace_setup_goals_window>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(state.local_player_nation == state.primary_crisis_attacker ? state.primary_crisis_defender : state.primary_crisis_attacker);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::national_identity_id>()) {
			payload.emplace<dcon::national_identity_id>(state.world.nation_get_identity_from_identity_holder(state.local_player_nation == state.primary_crisis_attacker ? state.primary_crisis_defender : state.primary_crisis_attacker));
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<bool>>()) {
			is_concession = any_cast<element_selection_wrapper<bool>>(payload).data;
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<bool>()) {
			payload.emplace<bool>(is_concession);
			return message_result::consumed;
		} else if(payload.holds_type<toggled_crisis_wargoal>()) {
			toggled_crisis_wargoal twg = any_cast<toggled_crisis_wargoal>(payload);
			if(uint32_t(twg.index) < wargoals.size())
				payload.emplace<toggled_crisis_wargoal>(toggled_crisis_wargoal{twg.index, wargoals[twg.index] });
			return message_result::consumed;
		} else if(payload.holds_type<toggle_crisis_wargoal>()) {
			toggle_crisis_wargoal twg = any_cast<toggle_crisis_wargoal>(payload);
			if(uint32_t(twg.index) < wargoals.size())
				wargoals[twg.index] = !wargoals[twg.index];
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<offer_cost>()) {
			int32_t total = 0;

			bool attacker_filter = ((state.local_player_nation == state.primary_crisis_attacker) != is_concession);
			for(uint32_t i = 0; i < wargoals.size(); ++i) {
				if(wargoals[i] && nations::nth_crisis_war_goal_is_for_attacker(state, i) == attacker_filter) {
					auto wg = nations::get_nth_crisis_war_goal(state, i);
					total += military::peace_cost(state, retrieve<dcon::war_id>(state, parent), wg.cb, wg.added_by, wg.target_nation, wg.secondary_nation, wg.state, wg.wg_tag);
				}
			}

			payload.emplace<offer_cost>(offer_cost{ total });
			return message_result::consumed;
		} else if(payload.holds_type<send_offer>()) {
			command::start_crisis_peace_offer(state, state.local_player_nation, is_concession);

			bool attacker_filter = ((state.local_player_nation == state.primary_crisis_attacker) != is_concession);
			auto count = nations::num_crisis_wargoals(state);
			for(uint32_t i = 0; i < wargoals.size(); ++i) {
				if(wargoals[i] && nations::nth_crisis_war_goal_is_for_attacker(state, i) == attacker_filter) {
					auto wg = nations::get_nth_crisis_war_goal(state, i);
					command::add_to_crisis_peace_offer(state, state.local_player_nation,wg.added_by, wg.target_nation, wg.cb, wg.state, wg.wg_tag, wg.secondary_nation);
				}
			}
			command::send_crisis_peace_offer(state, state.local_player_nation);
			set_visible(state, false);

			return message_result::consumed;
		} else if(payload.holds_type<test_acceptance>()) { // test_acceptance
			auto target = state.local_player_nation == state.primary_crisis_attacker ? state.primary_crisis_defender : state.primary_crisis_attacker;
			bool missing_wargoal = false;

			bool attacker_filter = ((state.local_player_nation == state.primary_crisis_attacker) != is_concession);
			auto count = nations::num_crisis_wargoals(state);
			for(uint32_t i = 0; i < wargoals.size(); ++i) {
				if(nations::nth_crisis_war_goal_is_for_attacker(state, i) == attacker_filter) {
					missing_wargoal = missing_wargoal || !wargoals[i];
				}
			}

			bool acceptance = ai::will_accept_crisis_peace_offer(state, target, is_concession, missing_wargoal);

			payload.emplace<test_acceptance>(test_acceptance{ acceptance });
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

} // namespace ui
