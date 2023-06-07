#pragma once

#include "gui_element_types.hpp"
#include "gui_foreign_investment_window.hpp"

namespace ui {

class production_investment_country_select : public button_element_base {
public:
	message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override { return parent->impl_on_scroll(state, x, y, amount, mods); }

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = button_press_notification{};
			parent->impl_get(state, payload);
		}
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
			return make_element_by_type<nation_player_investment_text>(state, id);
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
	std::string_view get_row_element_name() override { return "investment_country_entry"; }
};

class production_sort_nation_gp_flag : public nation_gp_flag {
public:
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(has_tooltip(state) == tooltip_behavior::no_tooltip)
			return message_result::unseen;
		return type == mouse_probe_type::tooltip ? message_result::consumed : message_result::unseen;
	}
	void button_action(sys::state& state) noexcept override { }
};

class invest_brow_window : public window_element_base {
	production_country_listbox* country_listbox = nullptr;
	country_list_sort sort = country_list_sort::country;
	bool sort_ascend = false;

	void filter_countries(sys::state& state, std::function<bool(dcon::nation_id)> filter_fun) {
		if(country_listbox) {
			country_listbox->row_contents.clear();
			state.world.for_each_nation([&](dcon::nation_id id) {
				if(state.world.nation_get_owned_province_count(id) != 0 && filter_fun(id))
					country_listbox->row_contents.push_back(id);
			});
			std::sort(country_listbox->row_contents.begin(), country_listbox->row_contents.end(), [&](dcon::nation_id a, dcon::nation_id b) {
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
		window_element_base::on_create(state);
		set_visible(state, false);
		filter_countries(state, [](dcon::nation_id) { return true; });
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "country_listbox") {
			auto ptr = make_element_by_type<production_country_listbox>(state, id);
			ptr->base_data.position.x -= 8 + 5; // Nudge
			country_listbox = ptr.get();
			return ptr;
		} else if(name == "sort_by_my_flag") {
			auto ptr = make_element_by_type<nation_player_flag>(state, id);
			ptr->base_data.position.y -= 2; // Nudge
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
			auto ptr = make_element_by_type<generic_tab_button<country_list_filter>>(state, id);
			ptr->target = country_list_filter::all;
			return ptr;
		} else if(name == "filter_enemies") {
			auto ptr = make_element_by_type<generic_tab_button<country_list_filter>>(state, id);
			ptr->target = country_list_filter::enemies;
			return ptr;
		} else if(name == "filter_allies") {
			auto ptr = make_element_by_type<generic_tab_button<country_list_filter>>(state, id);
			ptr->target = country_list_filter::allies;
			return ptr;
		} else if(name == "filter_neighbours") {
			auto ptr = make_element_by_type<generic_tab_button<country_list_filter>>(state, id);
			ptr->target = country_list_filter::neighbors;
			return ptr;
		} else if(name == "filter_sphere") {
			auto ptr = make_element_by_type<generic_tab_button<country_list_filter>>(state, id);
			ptr->target = country_list_filter::sphere;
			return ptr;
		} else if(name.length() >= 7 && name.substr(0, 7) == "filter_") {
			auto const filter_name = name.substr(7);
			auto ptr = make_element_by_type<generic_tab_button<dcon::modifier_id>>(state, id);
			ptr->target = ([&]() {
				dcon::modifier_id filter_mod_id{0};
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
		} else if(name.substr(0, 14) == "sort_by_gpflag") {
			auto ptr = make_element_by_type<production_sort_nation_gp_flag>(state, id);
			ptr->rank = uint16_t(std::stoi(std::string{name.substr(14)}));
			ptr->base_data.position.y -= 2; // Nudge
			return ptr;
		} else if(name.substr(0, 10) == "sort_by_gp") {
			auto ptr = make_element_by_type<country_sort_button<country_list_sort::gp_influence>>(state, id);
			ptr->offset = uint8_t(std::stoi(std::string{name.substr(10)}));
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<country_list_filter>()) {
			auto filter = any_cast<country_list_filter>(payload);
			switch(filter) {
			case country_list_filter::all:
				filter_countries(state, [&](dcon::nation_id) { return true; });
				break;
			case country_list_filter::allies:
				filter_countries(state, [&](dcon::nation_id id) {
					if(id == state.local_player_nation)
						return false;
					auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(id, state.local_player_nation);
					return state.world.diplomatic_relation_get_are_allied(rel) || military::are_allied_in_war(state, state.local_player_nation, id);
				});
				break;
			case country_list_filter::enemies:
				filter_countries(state, [&](dcon::nation_id id) { return military::are_at_war(state, state.local_player_nation, id); });
				break;
			case country_list_filter::sphere:
				filter_countries(state, [&](dcon::nation_id id) { return state.world.nation_get_in_sphere_of(id) == state.local_player_nation; });
				break;
			case country_list_filter::neighbors:
				filter_countries(state, [&](dcon::nation_id id) { return bool(state.world.get_nation_adjacency_by_nation_adjacency_pair(state.local_player_nation, id)); });
				break;
			default:
				break;
			}
			return message_result::consumed;
		} else if(payload.holds_type<dcon::modifier_id>()) {
			auto mod_id = any_cast<dcon::modifier_id>(payload);
			filter_by_continent(state, mod_id);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<country_list_sort>>()) {
			auto new_sort = any_cast<element_selection_wrapper<country_list_sort>>(payload).data;
			sort_ascend = (new_sort == sort) ? !sort_ascend : true;
			sort = new_sort;
			sort_countries(state, country_listbox->row_contents, sort, sort_ascend);
			country_listbox->update(state);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

} // namespace ui
