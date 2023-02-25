#pragma once

#include "dcon_generated.hpp"
#include "gui_element_types.hpp"
#include "text.hpp"
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
	north_america,
	south_america,
	europe,
	africa,
	asia,
	oceania,
	neighbors,
	sphere,
	enemies,
	allies
};

class diplomacy_country_select : public button_element_base {
public:
	message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override {
		return parent->impl_on_scroll(state, x, y, amount, mods);
	}
};

class diplomacy_country_info : public listbox_row_element_base<dcon::nation_id> {
private:
	flag_button* country_flag = nullptr;
	simple_text_element_base* country_name_box = nullptr;

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
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			country_name_box = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void update(sys::state& state) noexcept override {
		country_flag->on_update(state);

		dcon::nation_fat_id fat_id = dcon::fatten(state.world, content);
		auto country_name = text::produce_simple_string(state, fat_id.get_name());
		country_name_box->set_text(state, country_name);
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(content);
			return message_result::consumed;
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

class diplomacy_window : public generic_tabbed_window<diplomacy_window_tab> {
private:
	diplomacy_country_listbox* country_listbox = nullptr;

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

	void filter_by_continent(sys::state& state, country_list_filter filter) {
		dcon::modifier_id target_cont{};
		switch(filter) {
			case country_list_filter::north_america:
				target_cont = state.province_definitions.north_america;
				break;
			case country_list_filter::south_america:
				target_cont = state.province_definitions.south_america;
				break;
			case country_list_filter::europe:
				target_cont = state.province_definitions.europe;
				break;
			case country_list_filter::africa:
				target_cont = state.province_definitions.africa;
				break;
			case country_list_filter::asia:
				target_cont = state.province_definitions.asia;
				break;
			case country_list_filter::oceania:
				target_cont = state.province_definitions.oceania;
				break;
			default:
				break;
		}
		filter_countries(state, [&](dcon::nation_id id) -> bool {
			dcon::nation_fat_id fat_id = dcon::fatten(state.world, id);
			auto cont_id = fat_id.get_capital().get_continent().id;
			return target_cont == cont_id;
		});
	}

public:
	void on_create(sys::state& state) noexcept override {
		generic_tabbed_window::on_create(state);
		set_visible(state, false);
		filter_countries(state, [](dcon::nation_id) { return true; });
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
		} else if(name == "filter_north_america") {
			auto ptr = make_element_by_type<generic_tab_button<country_list_filter>>(state, id);
			ptr->target = country_list_filter::north_america;
			return ptr;
		} else if(name == "filter_south_america") {
			auto ptr = make_element_by_type<generic_tab_button<country_list_filter>>(state, id);
			ptr->target = country_list_filter::south_america;
			return ptr;
		} else if(name == "filter_europe") {
			auto ptr = make_element_by_type<generic_tab_button<country_list_filter>>(state, id);
			ptr->target = country_list_filter::europe;
			return ptr;
		} else if(name == "filter_africa") {
			auto ptr = make_element_by_type<generic_tab_button<country_list_filter>>(state, id);
			ptr->target = country_list_filter::africa;
			return ptr;
		} else if(name == "filter_asia") {
			auto ptr = make_element_by_type<generic_tab_button<country_list_filter>>(state, id);
			ptr->target = country_list_filter::asia;
			return ptr;
		} else if(name == "filter_oceania") {
			auto ptr = make_element_by_type<generic_tab_button<country_list_filter>>(state, id);
			ptr->target = country_list_filter::oceania;
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
		} else {
			return nullptr;
		}
	}

	void hide_tabs(sys::state& state) {
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
				case country_list_filter::north_america:
				case country_list_filter::south_america:
				case country_list_filter::europe:
				case country_list_filter::africa:
				case country_list_filter::asia:
				case country_list_filter::oceania:
					filter_by_continent(state, filter);
					break;
				default:
					break;
			}
		}
		return message_result::unseen;
	}
	
	element_base* casus_belli_window = nullptr;
	element_base* crisis_window = nullptr;
};

}
