#pragma once

#include "dcon_generated.hpp"
#include "gui_element_types.hpp"
#include "gui_graphics.hpp"
#include "gui_province_window.hpp"
#include "province.hpp"
#include "text.hpp"
#include <algorithm>

namespace ui {

class province_search_edit : public edit_box_element_base {
public:
	void edit_box_update(sys::state& state, std::u16string_view s) noexcept override {
		auto utf8_version = simple_fs::utf16_to_utf8(s);
		std::string_view v{ utf8_version };
		send(state, parent, v);
	}
};

class province_search_list_item : public listbox_row_button_base<std::variant<dcon::province_id, dcon::nation_id, dcon::state_instance_id>> {
public:
	void button_action(sys::state& state) noexcept override {
		auto map_variant_id = content;
		if(std::holds_alternative<dcon::province_id>(map_variant_id)) {
			auto prov_id = std::get<dcon::province_id>(map_variant_id);
			if(prov_id) {
				if(prov_id.value < state.province_definitions.first_sea_province.value) {
					state.set_selected_province(prov_id);
					static_cast<province_view_window*>(state.ui_state.province_window)->set_active_province(state, prov_id);
				}
				state.map_state.center_map_on_province(state, prov_id);	
			}
		}
		else if(std::holds_alternative<dcon::state_instance_id>(map_variant_id)) {
			auto state_id = std::get<dcon::state_instance_id>(map_variant_id);
			if(state_id) {
				auto capital_prov = state.world.state_instance_get_capital(state_id);
				if(capital_prov) {
					state.set_selected_province(capital_prov);
					static_cast<province_view_window*>(state.ui_state.province_window)->set_active_province(state, capital_prov);
					state.map_state.center_map_on_province(state, capital_prov);
				}
			}
		}
		else if(std::holds_alternative<dcon::nation_id>(map_variant_id)) {
			auto nation_id = std::get<dcon::nation_id>(map_variant_id);
			if(nation_id && state.world.nation_get_owned_province_count(nation_id) > 0) {
				auto capital_prov = state.world.nation_get_capital(nation_id);
				if(capital_prov) {
					state.set_selected_province(capital_prov);
					static_cast<province_view_window*>(state.ui_state.province_window)->set_active_province(state, capital_prov);
					state.map_state.center_map_on_province(state, capital_prov);
				}
			}
		}
	}

	void update(sys::state& state) noexcept override {
		if(std::holds_alternative<dcon::province_id>(content)) {
			dcon::province_fat_id fat_id = dcon::fatten(state.world, std::get<dcon::province_id>(content));
			auto name = text::produce_simple_string(state, fat_id.get_name());
			set_button_text(state, name);
		}
		else if(std::holds_alternative<dcon::state_instance_id>(content)) {
			auto name = text::get_dynamic_state_name(state, std::get<dcon::state_instance_id>(content));
			set_button_text(state, name);
		}
		else if(std::holds_alternative<dcon::nation_id>(content)) {
			auto name = text::get_name_as_string(state, std::get<dcon::nation_id>(content));
			set_button_text(state, name);
		}
	}
};

class province_search_list : public listbox_element_base<province_search_list_item, std::variant<dcon::province_id, dcon::nation_id, dcon::state_instance_id>> {
protected:
	std::string_view get_row_element_name() override {
		return "goto_location_button";
	}
};

class province_search_window : public window_element_base {
private:
	province_search_list* search_listbox = nullptr;
	province_search_edit* edit_box = nullptr;

	std::vector<std::variant<dcon::province_id, dcon::nation_id, dcon::state_instance_id>> search_states_provinces_nations(sys::state& state, std::string_view search_term) noexcept {
		std::vector<std::variant<dcon::province_id, dcon::nation_id, dcon::state_instance_id>> results{};
		std::string search_term_lower = parsers::lowercase_str(search_term);

		if(!search_term.empty()) {
			state.world.for_each_nation([&](dcon::nation_id nation) {
				auto fat_nation = dcon::fatten(state.world, nation);
				if(fat_nation.get_owned_province_count() > 0) {
					auto name = parsers::lowercase_str(text::get_name_as_string(state, fat_nation));
					if(name.starts_with(search_term_lower)) {
						results.push_back(nation);
					}
				}

			 });
			state.world.for_each_state_instance([&](dcon::state_instance_id si) {
				auto name = parsers::lowercase_str(text::get_dynamic_state_name(state, si));
				if(name.starts_with(search_term_lower)) {
					results.push_back(si);
				}
			});

			state.world.for_each_province([&](dcon::province_id prov_id) {
				dcon::province_fat_id fat_id = dcon::fatten(state.world, prov_id);
				auto name = parsers::lowercase_str(text::produce_simple_string(state, fat_id.get_name()));
				if(name.starts_with(search_term_lower)) {
					results.push_back(prov_id);
				}
			});
		}
		return results;
	}

public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		move_child_to_front(search_listbox);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "cancel") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "goto_edit") {
			auto ptr = make_element_by_type<province_search_edit>(state, id);
			edit_box = ptr.get();
			return ptr;
		} else if(name == "provinces") {
			auto ptr = make_element_by_type<province_search_list>(state, id);
			search_listbox = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<std::string_view>()) {
			auto search_term = any_cast<std::string_view>(payload);
			search_listbox->row_contents = search_states_provinces_nations(state, search_term);
			search_listbox->update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}

	void on_visible(sys::state& state) noexcept override {
		state.ui_state.set_focus_target(state, edit_box);
	}

	void on_hide(sys::state& state) noexcept override {
		state.ui_state.set_focus_target(state, nullptr);
	}
};

} // namespace ui
