#pragma once

#include "dcon_generated.hpp"
#include "gui_element_types.hpp"
#include "gui_graphics.hpp"
#include "text.hpp"
#include "parsers.hpp"
#include <algorithm>
#include <functional>

namespace ui {

enum class diplomacy_window_tab : uint8_t {
	great_powers = 0x0,
	wars = 0x1,
	casus_belli = 0x2,
	crisis = 0x3
};

enum class country_list_filter : uint8_t {
	all,
	neighbors,
	sphere,
	enemies,
	allies,
	continent
};

class button_press_notification{};

class diplomacy_country_select : public button_element_base {
public:
	message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override {
		return parent->impl_on_scroll(state, x, y, amount, mods);
	}

	void button_action(sys::state& state) noexcept override {
		Cyto::Any payload = button_press_notification{};
		parent->impl_get(state, payload);
	}
};

class nation_overlord_flag : public flag_button {
	dcon::nation_id sphereling_id{};
public:
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		auto ovr_id = state.world.nation_get_in_sphere_of(sphereling_id);
		if(bool(ovr_id)) {
			auto fat_id = dcon::fatten(state.world, ovr_id);
			return fat_id.get_identity_from_identity_holder();
		}
		return dcon::national_identity_id{};
	}

	void on_update(sys::state& state) noexcept override {
		// Only show if there is any overlord
		set_visible(state, bool(get_current_nation(state)));
		set_current_nation(state, get_current_nation(state));
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			sphereling_id = any_cast<dcon::nation_id>(payload);
			on_update(state);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class diplomacy_country_info : public listbox_row_element_base<dcon::nation_id> {
private:
	flag_button* country_flag = nullptr;
public:
	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base<dcon::nation_id>::on_create(state);
		base_data.position.x -= 14;
		base_data.position.y -= 524;
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "country_select") {
			return make_element_by_type<diplomacy_country_select>(state, id);
		} else if(name == "country_flag") {
			auto ptr = make_element_by_type<flag_button>(state, id);
			country_flag = ptr.get();
			return ptr;
		} else if(name == "country_name") {
			return make_element_by_type<generic_name_text<dcon::nation_id>>(state, id);
		} else if(name == "country_gp0") {
			return make_element_by_type<nation_gp_opinion_text<0>>(state, id);
		} else if(name == "country_gp1") {
			return make_element_by_type<nation_gp_opinion_text<1>>(state, id);
		} else if(name == "country_gp2") {
			return make_element_by_type<nation_gp_opinion_text<2>>(state, id);
		} else if(name == "country_gp3") {
			return make_element_by_type<nation_gp_opinion_text<3>>(state, id);
		} else if(name == "country_gp4") {
			return make_element_by_type<nation_gp_opinion_text<4>>(state, id);
		} else if(name == "country_gp5") {
			return make_element_by_type<nation_gp_opinion_text<5>>(state, id);
		} else if(name == "country_gp6") {
			return make_element_by_type<nation_gp_opinion_text<6>>(state, id);
		} else if(name == "country_gp7") {
			return make_element_by_type<nation_gp_opinion_text<7>>(state, id);
		} else if(name == "country_boss_flag") {
			return make_element_by_type<nation_overlord_flag>(state, id);
		} else if(name == "country_prestige") {
			return make_element_by_type<nation_prestige_rank_text>(state, id);
		} else if(name == "country_economic") {
			return make_element_by_type<nation_industry_rank_text>(state, id);
		} else if(name == "country_military") {
			return make_element_by_type<nation_military_rank_text>(state, id);
		} else if(name == "country_total") {
			return make_element_by_type<nation_rank_text>(state, id);
		} else if(name == "country_opinion") {
			return make_element_by_type<nation_player_opinion_text>(state, id);
		} else if(name == "country_relation") {
			return make_element_by_type<nation_player_relations_text>(state, id);
		} else {
			return nullptr;
		}
	}

	void update(sys::state& state) noexcept override {
		country_flag->on_update(state);
		Cyto::Any payload = content;
		impl_set(state, payload);
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(content);
			return message_result::consumed;
		} else if(payload.holds_type<button_press_notification>()) {
			Cyto::Any new_payload = content;
			return parent->impl_get(state, new_payload);
		} else {
			return listbox_row_element_base<dcon::nation_id>::get(state, payload);
		}
	}
};

class diplomacy_country_listbox : public listbox_element_base<diplomacy_country_info, dcon::nation_id> {
protected:
	std::string_view get_row_element_name() override {
        return "diplomacy_country_info";
    }
};

class diplomacy_country_facts : public window_element_base {
private:
	dcon::nation_id active_nation{};
	flag_button* country_flag = nullptr;
	nation_player_relations_text* country_relation = nullptr;
	image_element_base* country_relation_icon = nullptr;
	simple_text_element_base* country_primary_cultures = nullptr;
	simple_text_element_base* country_accepted_cultures = nullptr;
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		if(bool(state.local_player_nation)) {
			Cyto::Any payload = state.local_player_nation;
			set(state, payload);
		}
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "country_flag") {
			auto ptr = make_element_by_type<flag_button>(state, id);
			country_flag = ptr.get();
			return ptr;
		} else if(name == "country_flag_overlay") {
			return make_element_by_type<nation_flag_frame>(state, id);
		} else if(name == "country_name") {
			return make_element_by_type<generic_name_text<dcon::nation_id>>(state, id);
		} else if(name == "country_status") {
			return make_element_by_type<nation_status_text>(state, id);
		} else if(name == "selected_nation_totalrank") {
			return make_element_by_type<nation_rank_text>(state, id);
		} else if(name == "ideology_icon") {
			return make_element_by_type<nation_ruling_party_ideology_plupp>(state, id);
		} else if(name == "country_party") {
			return make_element_by_type<nation_ruling_party_text>(state, id);
		} else if(name == "country_gov") {
			return make_element_by_type<nation_government_type_text>(state, id);
		} else if(name == "country_tech") {
			return make_element_by_type<nation_technology_admin_type_text>(state, id);
		} else if(name == "our_relation_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			country_relation_icon = ptr.get();
			return ptr;
		} else if(name == "our_relation") {
			auto ptr = make_element_by_type<nation_player_relations_text>(state, id);
			country_relation = ptr.get();
			return ptr;
		} else if(name == "country_prestige") {
			return make_element_by_type<nation_prestige_text>(state, id);
		} else if(name == "selected_prestige_rank") {
			return make_element_by_type<nation_prestige_rank_text>(state, id);
		} else if(name == "country_economic") {
			return make_element_by_type<nation_industry_score_text>(state, id);
		} else if(name == "selected_industry_rank") {
			return make_element_by_type<nation_industry_rank_text>(state, id);
		} else if(name == "country_military") {
			return make_element_by_type<nation_military_score_text>(state, id);
		} else if(name == "selected_military_rank") {
			return make_element_by_type<nation_military_rank_text>(state, id);
		} else if(name == "country_total") {
			return make_element_by_type<nation_total_score_text>(state, id);
		} else if(name == "selected_total_rank") {
			return make_element_by_type<nation_rank_text>(state, id);
		} else if(name == "country_population") {
			return make_element_by_type<nation_population_text>(state, id);
		} else if(name == "country_primary_cultures") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			country_primary_cultures = ptr.get();
			return ptr;
		} else if(name == "country_accepted_cultures") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			country_accepted_cultures = ptr.get();
			return ptr;
		} else if(name == "country_wars") {
			auto ptr = make_element_by_type<overlapping_enemy_flags>(state, id);
			ptr->base_data.position.y -= 8;
			return ptr;
		} else if(name == "country_allies") {
			auto ptr = make_element_by_type<overlapping_ally_flags>(state, id);
			ptr->base_data.position.y -= 8;
			return ptr;
		} else if(name == "country_protected") {
			auto ptr = make_element_by_type<overlapping_protected_flags>(state, id);
			ptr->base_data.position.y -= 8;
			return ptr;
		} else if(name == "country_truce") {
			auto ptr = make_element_by_type<overlapping_truce_flags>(state, id);
			ptr->base_data.position.y -= 8;
			return ptr;
		} else if(name == "infamy_text") {
			return make_element_by_type<nation_infamy_text>(state, id);
		} else if(name == "warexhastion_text") {
			return make_element_by_type<nation_war_exhaustion_text>(state, id);
		} else if(name == "brigade_text") {
			return make_element_by_type<nation_brigades_text>(state, id);
		} else if(name == "ships_text") {
			return make_element_by_type<nation_ships_text>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		auto fat_id = dcon::fatten(state.world, active_nation);

		{
			const auto culture_id = fat_id.get_primary_culture();
			auto culture = dcon::fatten(state.world, culture_id);
			const auto text = text::produce_simple_string(state, culture.get_name());
			country_primary_cultures->set_text(state, text);
		}

		{
			std::string text{};
			for(const auto culture_id : fat_id.get_accepted_cultures()) {
				auto culture = dcon::fatten(state.world, culture_id);
				text += text::produce_simple_string(state, culture.get_name()) + ", ";
			}
			country_accepted_cultures->set_text(state, "");
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(active_nation);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			active_nation = any_cast<dcon::nation_id>(payload);
			country_flag->on_update(state);

			country_relation->set_visible(state, active_nation != state.local_player_nation);
			country_relation_icon->set_visible(state, active_nation != state.local_player_nation);
			
			on_update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class overlapping_attacker_flags : public overlapping_flags_box {
protected:
	void populate_flags(sys::state& state) override {
		contents.clear();
		auto war = dcon::fatten(state.world, war_id);
		for(auto o : war.get_war_participant())
			if(o.get_is_attacker() == true)
				contents.push_back(o.get_nation().get_identity_from_identity_holder().id);
		update(state);
	}
public:
	dcon::war_id war_id{};
};
class overlapping_defender_flags : public overlapping_flags_box {
protected:
	void populate_flags(sys::state& state) override {
		contents.clear();
		auto war = dcon::fatten(state.world, war_id);
		for(auto o : war.get_war_participant())
			if(o.get_is_attacker() == false)
				contents.push_back(o.get_nation().get_identity_from_identity_holder().id);
		update(state);
	}
public:
	dcon::war_id war_id{};
};

class diplomacy_war_info : public listbox_row_element_base<dcon::war_id> {
	simple_text_element_base* war_name = nullptr;
	simple_text_element_base* attackers_strength_text = nullptr;
	simple_text_element_base* defenders_strength_text = nullptr;
	overlapping_attacker_flags* attackers_flags = nullptr;
	overlapping_defender_flags* defenders_flags = nullptr;
public:
	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base::on_create(state);
		base_data.position.x = base_data.position.y = 0;
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "diplo_war_entrybg") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "war_name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			war_name = ptr.get();
			return ptr;
		} else if(name == "attackers_mil_strength") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			attackers_strength_text = ptr.get();
			return ptr;
		} else if(name == "defenders_mil_strength") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			defenders_strength_text = ptr.get();
			return ptr;
		} else if(name == "warscore") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "diplo_war_progress_overlay") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "diplo_warscore_marker1") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "diplo_warscore_marker2") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "warscore_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "attackers") {
			auto ptr = make_element_by_type<overlapping_attacker_flags>(state, id);
			attackers_flags = ptr.get();
			attackers_flags->base_data.position.y -= 8;
			return ptr;
		} else if(name == "defenders") {
			auto ptr = make_element_by_type<overlapping_defender_flags>(state, id);
			defenders_flags = ptr.get();
			defenders_flags->base_data.position.y -= 8;
			return ptr;
		} else if(name == "join_attackers") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "join_defenders") {
			return make_element_by_type<button_element_base>(state, id);
		} else {
			return nullptr;
		}
	}

	void update(sys::state& state) noexcept override {
		attackers_flags->war_id = defenders_flags->war_id = content;

		auto war = dcon::fatten(state.world, content);
		war_name->set_text(state, text::produce_simple_string(state, war.get_name()));

		//fat_id.get_military_score()
		int32_t attackers_strenght = 0;
		int32_t defenders_strenght = 0;
		for(auto o : war.get_war_participant()) {
			if(o.get_is_attacker())
				attackers_strenght += o.get_nation().get_military_score();
			else
				defenders_strenght += o.get_nation().get_military_score();
		}
		attackers_strength_text->set_text(state, std::to_string(attackers_strenght));
		defenders_strength_text->set_text(state, std::to_string(defenders_strenght));
	}
};

class diplomacy_war_listbox : public listbox_element_base<diplomacy_war_info, dcon::war_id> {
protected:
	std::string_view get_row_element_name() override {
        return "diplomacy_war_info";
    }
public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		state.world.for_each_war([&](dcon::war_id id) {
			row_contents.push_back(id);
		});
		update(state);
	}
};

class diplomacy_window : public generic_tabbed_window<diplomacy_window_tab> {
private:
	diplomacy_country_listbox* country_listbox = nullptr;
	diplomacy_war_listbox* war_listbox = nullptr;
	diplomacy_country_facts* country_facts = nullptr;

	void filter_countries(sys::state& state, std::function<bool(dcon::nation_id)> filter_fun) {
		if(country_listbox) {
			country_listbox->row_contents.clear();
			state.world.for_each_nation([&](dcon::nation_id id) {
				if(filter_fun(id)) {
					country_listbox->row_contents.push_back(id);
				}
			});
			std::sort(country_listbox->row_contents.begin(), country_listbox->row_contents.end(), [&](auto a, auto b) {
				dcon::nation_fat_id a_fat_id = dcon::fatten(state.world, a);
				auto a_name = text::produce_simple_string(state, a_fat_id.get_name());

				dcon::nation_fat_id b_fat_id = dcon::fatten(state.world, b);
				auto b_name = text::produce_simple_string(state, b_fat_id.get_name());
				return a_name < b_name;
			});
			country_listbox->update(state);
		}
	}

	void filter_by_continent(sys::state& state, dcon::modifier_id mod_id) {
		filter_countries(state, [&](dcon::nation_id id) -> bool {
			dcon::nation_fat_id fat_id = dcon::fatten(state.world, id);
			auto cont_id = fat_id.get_capital().get_continent().id;
			return mod_id == cont_id;
		});
	}

public:
	void on_create(sys::state& state) noexcept override {
		generic_tabbed_window::on_create(state);
		set_visible(state, false);
		filter_countries(state, [](dcon::nation_id) { return true; });
		state.ui_state.diplomacy_subwindow = this;
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "gp_info") {
			auto ptr = make_element_by_type<generic_tab_button<diplomacy_window_tab>>(state, id);
			ptr->target = diplomacy_window_tab::great_powers;
			return ptr;
		} else if(name == "war_info") {
			auto ptr = make_element_by_type<generic_tab_button<diplomacy_window_tab>>(state, id);
			ptr->target = diplomacy_window_tab::wars;
			return ptr;
		} else if(name == "cb_info") {
			auto ptr = make_element_by_type<generic_tab_button<diplomacy_window_tab>>(state, id);
			ptr->target = diplomacy_window_tab::casus_belli;
			return ptr;
		} else if(name == "crisis_info") {
			auto ptr = make_element_by_type<generic_tab_button<diplomacy_window_tab>>(state, id);
			ptr->target = diplomacy_window_tab::crisis;
			return ptr;
		} else if(name == "filter_all") {
			auto ptr = make_element_by_type<generic_tab_button<country_list_filter>>(state, id);
			ptr->target = country_list_filter::all;
			return ptr;
		} else if(name == "sort_by_gpflag0") {
			return make_element_by_type<nation_gp_flag<0>>(state, id);
		} else if(name == "sort_by_gpflag1") {
			return make_element_by_type<nation_gp_flag<1>>(state, id);
		} else if(name == "sort_by_gpflag2") {
			return make_element_by_type<nation_gp_flag<2>>(state, id);
		} else if(name == "sort_by_gpflag3") {
			return make_element_by_type<nation_gp_flag<3>>(state, id);
		} else if(name == "sort_by_gpflag4") {
			return make_element_by_type<nation_gp_flag<4>>(state, id);
		} else if(name == "sort_by_gpflag5") {
			return make_element_by_type<nation_gp_flag<5>>(state, id);
		} else if(name == "sort_by_gpflag6") {
			return make_element_by_type<nation_gp_flag<6>>(state, id);
		} else if(name == "sort_by_gpflag7") {
			return make_element_by_type<nation_gp_flag<7>>(state, id);
		} else if(name.length() >= 7 && name.substr(0, 7) == "filter_") {
			const auto filter_name = name.substr(7);
			auto ptr = make_element_by_type<generic_tab_button<dcon::modifier_id>>(state, id);
			ptr->target = ([&]() {
				dcon::modifier_id filter_mod_id{ 0 };
				auto it = state.key_to_text_sequence.find(parsers::lowercase_str(filter_name));
				if(it != state.key_to_text_sequence.end())
					state.world.for_each_modifier([&](dcon::modifier_id mod_id) {
						auto fat_id = dcon::fatten(state.world, mod_id);
						if(it->second == fat_id.get_name())
							filter_mod_id = mod_id;
					});
				return filter_mod_id;
			})();
			return ptr;
		} else if(name == "cb_info_win") {
			auto ptr = make_element_immediate(state, id);
			casus_belli_window = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "crisis_info_win") {
			auto ptr = make_element_immediate(state, id);
			crisis_window = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "country_listbox") {
			auto ptr = make_element_by_type<diplomacy_country_listbox>(state, id);
			country_listbox = ptr.get();
			return ptr;
		} else if(name == "war_listbox") {
			auto ptr = make_element_by_type<diplomacy_war_listbox>(state, id);
			war_listbox = ptr.get();
			war_listbox->set_visible(state, false);
			return ptr;
		} else if(name == "diplomacy_country_facts") {
			auto ptr = make_element_by_type<diplomacy_country_facts>(state, id);
			country_facts = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void hide_tabs(sys::state& state) {
		war_listbox->set_visible(state, false);
		casus_belli_window->set_visible(state, false);
		crisis_window->set_visible(state, false);
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<diplomacy_window_tab>()) {
			auto enum_val = any_cast<diplomacy_window_tab>(payload);
			hide_tabs(state);
			switch(enum_val) {
				case diplomacy_window_tab::great_powers:
					break;
				case diplomacy_window_tab::wars:
					war_listbox->set_visible(state, true);
					break;
				case diplomacy_window_tab::casus_belli:
					casus_belli_window->set_visible(state, true);
					break;
				case diplomacy_window_tab::crisis:
					crisis_window->set_visible(state, true);
					break;
			}
			active_tab = enum_val;
			return message_result::consumed;
		} else if(payload.holds_type<country_list_filter>()) {
			auto filter = any_cast<country_list_filter>(payload);
			switch(filter) {
				case country_list_filter::all:
					filter_countries(state, [](auto) { return true; });
					break;
				default:
					break;
			}
		} else if(payload.holds_type<dcon::modifier_id>()) {
			auto mod_id = any_cast<dcon::modifier_id>(payload);
			filter_by_continent(state, mod_id);
		} else if(payload.holds_type<dcon::nation_id>()) {
			return country_facts->impl_set(state, payload);
		}
		return message_result::unseen;
	}

	element_base* casus_belli_window = nullptr;
	element_base* crisis_window = nullptr;
};

}
