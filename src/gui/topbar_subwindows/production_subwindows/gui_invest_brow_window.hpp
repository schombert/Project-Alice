#pragma once

#include "gui_element_types.hpp"

namespace ui {

class production_investment_country_select : public button_element_base {
public:
	message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override {
		return parent->impl_on_scroll(state, x, y, amount, mods);
	}

	void button_action(sys::state& state) noexcept override {
		Cyto::Any payload = button_press_notification{};
		parent->impl_get(state, payload);
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
			country_flag = ptr.get();
			return ptr;
		} else if(name == "country_name") {
			return make_element_by_type<generic_name_text<dcon::nation_id>>(state, id);
		} else if(name == "country_gp0") {
			return make_element_by_type<nation_gp_investment_text<0>>(state, id);
		} else if(name == "country_gp1") {
			return make_element_by_type<nation_gp_investment_text<1>>(state, id);
		} else if(name == "country_gp2") {
			return make_element_by_type<nation_gp_investment_text<2>>(state, id);
		} else if(name == "country_gp3") {
			return make_element_by_type<nation_gp_investment_text<3>>(state, id);
		} else if(name == "country_gp4") {
			return make_element_by_type<nation_gp_investment_text<4>>(state, id);
		} else if(name == "country_gp5") {
			return make_element_by_type<nation_gp_investment_text<5>>(state, id);
		} else if(name == "country_gp6") {
			return make_element_by_type<nation_gp_investment_text<6>>(state, id);
		} else if(name == "country_gp7") {
			return make_element_by_type<nation_gp_investment_text<7>>(state, id);
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

class production_country_listbox : public listbox_element_base<production_investment_country_info, dcon::nation_id> {
protected:
	std::string_view get_row_element_name() override {
        return "investment_country_entry";
    }
};

class invest_brow_window : public window_element_base {
	production_country_listbox* country_listbox = nullptr;

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
		window_element_base::on_create(state);
		set_visible(state, false);
		filter_countries(state, [](dcon::nation_id) { return true; });
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "filter_all") {
			auto ptr = make_element_by_type<generic_tab_button<country_list_filter>>(state, id);
			ptr->target = country_list_filter::all;
			return ptr;
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
		} else if(name == "country_listbox") {
			auto ptr = make_element_by_type<production_country_listbox>(state, id);
			country_listbox = ptr.get();
			return ptr;
		} else if(name.substr(0, 10) == "sort_by_gp") {
			auto gp_nth = std::stoi(std::string{ name.substr(10) });
			auto ptr = make_element_by_type<button_element_base>(state, id);
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
					filter_countries(state, [](auto) { return true; });
					break;
				default:
					break;
			}
			return message_result::consumed;
		} else if(payload.holds_type<dcon::modifier_id>()) {
			auto mod_id = any_cast<dcon::modifier_id>(payload);
			filter_by_continent(state, mod_id);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

}
