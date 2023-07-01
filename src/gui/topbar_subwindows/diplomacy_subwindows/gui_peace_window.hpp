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
