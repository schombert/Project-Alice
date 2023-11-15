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
		set_text(state, text::produce_simple_string(state, "alice_state_select_title"));
	}
};

class map_state_select_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::state_definition_id>(state, parent);
		set_button_text(state, text::produce_simple_string(state, state.world.state_definition_get_name(content)));
		//auto it = std::find(state.selected_states.begin(), state.selected_states.end(), content);
		//disabled = (it != state.selected_states.end());
	}
	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::state_definition_id>(state, parent);
		state.state_select(content);
	}
};

class map_state_select_entry : public listbox_row_element_base<dcon::state_definition_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "button") {
			return make_element_by_type<map_state_select_button>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			auto memb = state.world.state_definition_get_abstract_state_membership(content);
			if(memb.begin() == memb.end()) {
				payload.emplace<dcon::nation_id>(state.national_definitions.rebel_id);
				return message_result::consumed;
			}
			auto n = (*(memb.begin())).get_province().get_state_membership().get_capital().get_province_control().get_nation();
			payload.emplace<dcon::nation_id>(n);
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
		assert(state.state_selection.has_value());
		row_contents = state.state_selection->selectable_states;
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
};

} //namespace ui
