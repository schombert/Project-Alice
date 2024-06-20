#pragma once

#include "gui_element_types.hpp"
#include "gui_foreign_investment_window.hpp"

namespace ui {

class production_investment_country_select : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto for_nation = retrieve<dcon::nation_id>(state, parent);

		auto for_rules = state.world.nation_get_combined_issue_rules(for_nation);

		if(!nations::is_great_power(state, state.local_player_nation)) {
			disabled = true;
			return;
		}
		if(nations::is_great_power(state, for_nation) || !state.world.nation_get_is_civilized(for_nation)) {
			disabled = true;
			return;
		}
		if((for_rules & issue_rule::allow_foreign_investment) == 0) {
			disabled = true;
			return;
		}
		disabled = false;
	}

	void button_action(sys::state& state) noexcept override {
		auto for_nation = retrieve<dcon::nation_id>(state, parent);
		open_foreign_investment(state, for_nation);
	}
};

class production_investment_country_info : public listbox_row_element_base<dcon::nation_id> {
private:
	flag_button* country_flag = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "country_select") {
			return make_element_by_type<production_investment_country_select>(state, id);
		} else if(name == "country_flag") {
			auto ptr = make_element_by_type<flag_button>(state, id);
			ptr->base_data.position.y -= 2; // Nudge
			country_flag = ptr.get();
			return ptr;
		} else if(name == "country_name") {
			return make_element_by_type<generic_name_text<dcon::nation_id>>(state, id);
		} else if(name == "invest") {
			auto ptr = make_element_by_type<nation_player_investment_text>(state, id);
			ptr->base_data.position.x -= 4;
			return ptr;
		} else if(name == "factories") {
			return make_element_by_type<nation_industries_text>(state, id);
		} else if(name == "country_boss_flag") {
			auto ptr = make_element_by_type<nation_overlord_flag>(state, id);
			ptr->base_data.position.y -= 2; // Nudge
			return ptr;
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
		} else if(name.substr(0, 10) == "country_gp") {
			auto ptr = make_element_by_type<nation_gp_investment_text>(state, id);
			ptr->rank = uint16_t(std::stoi(std::string{name.substr(10)}));
			ptr->base_data.position.x -= 4;
			return ptr;
		} else {
			return nullptr;
		}
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

class production_country_listbox : public listbox_element_base<production_investment_country_info, dcon::nation_id> {
protected:
	std::string_view get_row_element_name() override {
		return "investment_country_entry";
	}

	void on_update(sys::state& state) noexcept override {
		auto current_filter = retrieve< country_filter_setting>(state, parent);
		auto current_sort = retrieve<country_sort_setting>(state, parent);
		row_contents.clear();
		state.world.for_each_nation([&](dcon::nation_id id) {
			if(state.world.nation_get_owned_province_count(id) != 0) {
				bool passes_filter = country_category_filter_check(state, current_filter.general_category, state.local_player_nation, id);
				bool right_continent = !current_filter.continent || state.world.nation_get_capital(id).get_continent() == current_filter.continent;

				if(passes_filter && right_continent)
					row_contents.push_back(id);
			}
		});
		sort_countries(state, row_contents, current_sort.sort, current_sort.sort_ascend);
		update(state);
	}
};

class production_sort_nation_gp_flag : public nation_gp_flag {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void button_action(sys::state& state) noexcept override { }
};

class production_sort_my_nation_flag : public flag_button {
public:
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		return state.world.nation_get_identity_from_identity_holder(state.local_player_nation);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void button_action(sys::state& state) noexcept override { }
};

class invest_brow_window : public window_element_base {
	production_country_listbox* country_listbox = nullptr;
	country_filter_setting filter = country_filter_setting{};
	country_sort_setting sort = country_sort_setting{};

public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
		country_listbox->list_scrollbar->base_data.position.x += 13;
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "country_listbox") {
			auto ptr = make_element_by_type<production_country_listbox>(state, id);
			ptr->base_data.position.x -= 8 + 5; // Nudge
			country_listbox = ptr.get();
			return ptr;
		} else if(name == "sort_by_my_flag") {
			auto ptr = make_element_by_type<production_sort_my_nation_flag>(state, id);
			ptr->base_data.position.y -= 2; // Nudge
			return ptr;
		} else if(name == "sort_by_country") {
			auto ptr = make_element_by_type<country_sort_button<country_list_sort::country>>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "sort_by_boss") {
			auto ptr = make_element_by_type<country_sort_button<country_list_sort::boss>>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "sort_by_prestige") {
			auto ptr = make_element_by_type<country_sort_button<country_list_sort::prestige_rank>>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "sort_by_economic") {
			auto ptr = make_element_by_type<country_sort_button<country_list_sort::economic_rank>>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "sort_by_military") {
			auto ptr = make_element_by_type<country_sort_button<country_list_sort::military_rank>>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "sort_by_total") {
			auto ptr = make_element_by_type<country_sort_button<country_list_sort::total_rank>>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "sort_by_relation") {
			auto ptr = make_element_by_type<country_sort_button<country_list_sort::relation>>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "sort_by_opinion") {
			return make_element_by_type<country_sort_button<country_list_sort::opinion>>(state, id);
		} else if(name == "sort_by_prio") {
			return make_element_by_type<country_sort_button<country_list_sort::priority>>(state, id);
		} else if(name == "filter_all") {
			return make_element_by_type<category_filter_button<country_list_filter::all>>(state, id);
		} else if(name == "filter_enemies") {
			return make_element_by_type<category_filter_button<country_list_filter::enemies>>(state, id);
		} else if(name == "filter_allies") {
			return make_element_by_type<category_filter_button<country_list_filter::allies>>(state, id);
		} else if(name == "filter_neighbours") {
			return make_element_by_type<category_filter_button<country_list_filter::neighbors>>(state, id);
		} else if(name == "filter_sphere") {
			return make_element_by_type<category_filter_button<country_list_filter::sphere>>(state, id);
		} else if(name == "sort_by_invest_factories") {
			auto ptr = make_element_by_type<country_sort_button<country_list_sort::factories>>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name.length() >= 7 && name.substr(0, 7) == "filter_") {
			auto const filter_name = name.substr(7);
			auto ptr = make_element_by_type<continent_filter_button>(state, id);
			auto k = state.lookup_key(name);
			if(k) {
				for(auto m : state.world.in_modifier) {
					if(m.get_name() == k) {
						ptr->continent = m;
						break;
					}
				}
			}
			return ptr;
		} else if(name.substr(0, 14) == "sort_by_gpflag") {
			auto ptr = make_element_by_type<production_sort_nation_gp_flag>(state, id);
			ptr->rank = uint16_t(std::stoi(std::string{name.substr(14)}));
			ptr->base_data.position.y -= 2; // Nudge
			return ptr;
		} else if(name.substr(0, 10) == "sort_by_gp") {
			auto ptr = make_element_by_type<country_sort_button<country_list_sort::gp_investment>>(state, id);
			ptr->offset = uint8_t(std::stoi(std::string{name.substr(10)}));
			return ptr;
		} else if(name == "sort_by_my_invest") {
			return make_element_by_type<country_sort_by_player_investment>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<country_sort_setting>()) {
			payload.emplace<country_sort_setting>(sort);
			return message_result::consumed;
		} else if(payload.holds_type< country_filter_setting>()) {
			payload.emplace<country_filter_setting>(filter);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<country_list_sort>>()) {
			auto new_sort = any_cast<element_selection_wrapper<country_list_sort>>(payload).data;
			sort.sort_ascend = (new_sort == sort.sort) ? !sort.sort_ascend : true;
			sort.sort = new_sort;
			country_listbox->impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<country_list_filter>()) {
			auto temp = any_cast<country_list_filter>(payload);
			filter.general_category = filter.general_category != temp ? temp : country_list_filter::all;
			country_listbox->impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::modifier_id>()) {
			auto temp_c = any_cast<dcon::modifier_id>(payload);
			filter.continent = filter.continent == temp_c ? dcon::modifier_id{} : temp_c;
			country_listbox->impl_on_update(state);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

} // namespace ui
