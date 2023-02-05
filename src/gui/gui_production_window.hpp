#pragma once

#include "gui_element_types.hpp"
#include <vector>

namespace ui {

enum class production_window_sub_window : uint8_t {
	factories = 0x0,
	investments = 0x1,
	projects = 0x2,
	goods = 0x3
};

class production_window_tab_button : public checkbox_button {
public:
	bool is_active(sys::state& state) noexcept override;
	void button_action(sys::state& state) noexcept override;
	production_window_sub_window target = production_window_sub_window::factories;
};

class production_window_close_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(state.ui_state.production_window) {
			state.ui_state.production_window->set_visible(state, false);
			state.ui_state.root->move_child_to_back(state.ui_state.production_window);
		}
	}
};

class production_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<production_window_close_button>(state, id);
		} else if(name == "tab_factories") {
			auto ptr = make_element_by_type<production_window_tab_button>(state, id);
			ptr->target = production_window_sub_window::factories;
			return ptr;
		} else if(name == "tab_invest") {
			auto ptr = make_element_by_type<production_window_tab_button>(state, id);
			ptr->target = production_window_sub_window::investments;
			return ptr;
		} else if(name == "tab_popprojects") {
			auto ptr = make_element_by_type<production_window_tab_button>(state, id);
			ptr->target = production_window_sub_window::projects;
			return ptr;
		} else if(name == "tab_goodsproduction") {
			auto ptr = make_element_by_type<production_window_tab_button>(state, id);
			ptr->target = production_window_sub_window::goods;
			return ptr;
		} else if(name == "factory_buttons") {
			auto ptr = make_element_immediate(state, id);
			factory_elements.push_back(ptr.get());
			return ptr;
		// } else if(name == "state_listbox") {
		// 	auto ptr = make_element_immediate(state, id);
		// 	factory_elements.push_back(ptr.get());
		// 	return ptr;
		} else if(name == "investment_browser") {
			auto ptr = make_element_immediate(state, id);
			investment_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "invest_buttons") {
			auto ptr = make_element_immediate(state, id);
			investment_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		// } else if(name == "state_listbox_invest") {
		// 	auto ptr = make_element_immediate(state, id);
		// 	investment_elements.push_back(ptr.get());
		// 	ptr->set_visible(state, false);
		// 	return ptr;
		} else if(name == "pop_sort_buttons_invest") {
			auto ptr = make_element_immediate(state, id);
			investment_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "sort_by_state") {
			auto ptr = make_element_immediate(state, id);
			project_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "sort_by_projects") {
			auto ptr = make_element_immediate(state, id);
			project_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "sort_by_completion") {
			auto ptr = make_element_immediate(state, id);
			project_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "sort_by_projecteers") {
			auto ptr = make_element_immediate(state, id);
			project_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "pop_sort_buttons") {
			auto ptr = make_element_immediate(state, id);
			project_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		// } else if(name == "project_listbox") {
		// 	auto ptr = make_element_immediate(state, id);
		// 	project_elements.push_back(ptr.get());
		// 	ptr->set_visible(state, false);
		// 	return ptr;
		} else if(name == "good_production") {
			auto ptr = make_element_immediate(state, id);
			good_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else {
			return nullptr;
		}
	}

	void hide_vector_elements(sys::state& state, std::vector<element_base*>& elements) {
		for(auto element : elements) {
			element->set_visible(state, false);
		}
	}

	void show_vector_elements(sys::state& state, std::vector<element_base*>& elements) {
		for(auto element : elements) {
			element->set_visible(state, true);
		}
	}

	void hide_sub_windows(sys::state& state) {
		hide_vector_elements(state, factory_elements);
		hide_vector_elements(state, investment_elements);
		hide_vector_elements(state, project_elements);
		hide_vector_elements(state, good_elements);
	}
	
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<production_window_sub_window>()) {
			auto enum_val = any_cast<production_window_sub_window>(payload);
			hide_sub_windows(state);
			switch(enum_val) {
				case production_window_sub_window::factories:
					show_vector_elements(state, factory_elements);
					break;
				case production_window_sub_window::investments:
					show_vector_elements(state, investment_elements);
					break;
				case production_window_sub_window::projects:
					show_vector_elements(state, project_elements);
					break;
				case production_window_sub_window::goods:
					show_vector_elements(state, good_elements);
					break;
			}
			active_sub_window = enum_val;
			return message_result::consumed;
		}
		return message_result::unseen;
	}
	
	std::vector<element_base*> factory_elements;
	std::vector<element_base*> investment_elements;
	std::vector<element_base*> project_elements;
	std::vector<element_base*> good_elements;
	production_window_sub_window active_sub_window = production_window_sub_window::factories;
};

}
