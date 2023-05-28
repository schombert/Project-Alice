#pragma once

#include "gui_element_types.hpp"

namespace ui {

class project_investment_header : public simple_text_element_base {
public:
	std::string get_text(sys::state& state, std::string csv) noexcept {
		return text::produce_simple_string(state, csv);
	}

	void on_update(sys::state& state) noexcept override {
		set_text(state, get_text(state, "invest_project_title"));
	}
};

class project_investment_current_funds : public simple_text_element_base {
public:
	std::string get_text(sys::state& state) noexcept {
		return text::format_money(nations::get_treasury(state, state.local_player_nation));
	}

	void on_update(sys::state& state) noexcept override {
		set_text(state, get_text(state));
	}
};

class project_investment_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "window_bg") {
			auto ptr = make_element_by_type<draggable_target>(state, id);
			return ptr;
		} else if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "header") {
			return make_element_by_type<project_investment_header>(state, id);
		} else if(name == "invest") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "max") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "current_budget_label") {
			return make_element_by_type<simple_text_element_base>(state, id);		// NOT
		} else if(name == "your_investment_label") {
			return make_element_by_type<simple_text_element_base>(state, id);		// NOT
		} else if(name == "left_value") {
			return make_element_by_type<project_investment_current_funds>(state, id);
		} else if(name == "your_investment") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "right_value") {
			return make_element_by_type<simple_text_element_base>(state, id);		// NOT
		} else if(name == "transfer_slider") {
			return make_element_by_type<scrollbar>(state, id);
		} else {
			return nullptr;
		}
	}
};

}
