#pragma once

#include "demographics.hpp"
#include "gui_element_types.hpp"
#include "system_state.hpp"
#include "text.hpp"
#include <vector>

namespace ui {

class map_state_select_title : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::produce_simple_string(state, "State select (ESC)"));
	}
};

class map_state_select_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::state_definition_id>(state, parent);
		set_button_text(state, text::produce_simple_string(state, state.world.state_definition_get_name(content)));

		auto it = std::find(state.selected_states.begin(), state.selected_states.end(), content);
		disabled = (it != state.selected_states.end());
	}
	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::state_definition_id>(state, parent);

		auto sdef = content;
		bool can_select = false;
		for(const auto s : state.selectable_states) {
			if(s == sdef) {
				can_select = true;
				break;
			}
		}
		if(can_select) {
			if(state.single_state_select) {
				if(!state.selected_states.empty() && state.selected_states[0] == sdef) {
					state.selected_states.clear();
				} else {
					state.selected_states.clear();
					state.selected_states.push_back(sdef);
				}
			} else {
				auto it = std::find(state.selected_states.begin(), state.selected_states.end(), sdef);
				if(it == state.selected_states.end()) {
					state.selected_states.push_back(sdef);
				} else {
					state.selected_states.erase(std::remove(state.selected_states.begin(), state.selected_states.end(), sdef), state.selected_states.end());
				}
			}
		}
	}
};

class map_state_select_entry : public listbox_row_element_base<dcon::state_definition_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "shield") {
			return make_element_by_type<flag_button>(state, id);
		} else if(name == "button") {
			return make_element_by_type<map_state_select_button>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			for(const auto as : state.world.state_definition_get_abstract_state_membership(content)) {
				auto n = as.get_province().get_state_membership().get_capital().get_province_control().get_nation();
				payload.emplace<dcon::nation_id>(n);
				return message_result::consumed;
			}
			payload.emplace<dcon::nation_id>(state.national_definitions.rebel_id);
			return message_result::consumed;
		}
		return listbox_row_element_base::get(state, payload);
	}
};

class map_state_select_listbox : public listbox_element_base<map_state_select_entry, dcon::state_definition_id> {
protected:
	std::string_view get_row_element_name() override {
		return "alice_select_legend_entry";
	}
public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		row_contents = state.selectable_states;
		update(state);
	}
};

class map_state_select_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "legend_title") {
			return make_element_by_type<map_state_select_title>(state, id);
		} else if(name == "selectable") {
			return make_element_by_type<map_state_select_listbox>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		on_update(state);
	}
};

} //namespace ui
