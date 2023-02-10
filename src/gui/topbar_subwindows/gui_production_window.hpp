#pragma once

#include "gui_element_types.hpp"
#include "gui_factory_buttons_window.hpp"
#include "gui_invest_brow_window.hpp"
#include "gui_invest_buttons_window.hpp"
#include <vector>

namespace ui {

enum class production_window_tab : uint8_t {
	factories = 0x0,
	investments = 0x1,
	projects = 0x2,
	goods = 0x3
};

class production_window : public generic_tabbed_window<production_window_tab> {
public:
	void on_create(sys::state& state) noexcept override {
		generic_tabbed_window::on_create(state);
		set_visible(state, false);
	}
	factory_buttons_window* factbutwin = nullptr;
	invest_brow_window* investbrowwin = nullptr;
	invest_buttons_window* investbutwin = nullptr;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "tab_factories") {
			auto ptr = make_element_by_type<generic_tab_button<production_window_tab>>(state, id);
			ptr->target = production_window_tab::factories;
			return ptr;
		} else if(name == "tab_invest") {
			auto ptr = make_element_by_type<generic_tab_button<production_window_tab>>(state, id);
			ptr->target = production_window_tab::investments;
			return ptr;
		} else if(name == "tab_popprojects") {
			auto ptr = make_element_by_type<generic_tab_button<production_window_tab>>(state, id);
			ptr->target = production_window_tab::projects;
			return ptr;
		} else if(name == "tab_goodsproduction") {
			auto ptr = make_element_by_type<generic_tab_button<production_window_tab>>(state, id);
			ptr->target = production_window_tab::goods;
			return ptr;
		} else if(name == "tab_factories_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "tab_invest_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "tab_goodsproduction_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "tab_popprojects_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "factory_buttons") {
			auto ptr = make_element_by_type<factory_buttons_window>(state, id);
			factbutwin = ptr.get();
			ptr->set_visible(state, true);
			this->add_child_to_front(std::move(ptr));
			return ptr;
		} else if(name == "investment_browser") {
			auto ptr = make_element_by_type<invest_brow_window>(state, id);
			investbrowwin = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "invest_buttons") {
			auto ptr = make_element_by_type<invest_buttons_window>(state, id);
			investbutwin = ptr.get();
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
		factbutwin->set_visible(state, false);
		hide_vector_elements(state, investment_elements);
		investbrowwin->set_visible(state, false);
		investbutwin->set_visible(state, false);
		hide_vector_elements(state, project_elements);
		hide_vector_elements(state, good_elements);
	}
	
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<production_window_tab>()) {
			auto enum_val = any_cast<production_window_tab>(payload);
			hide_sub_windows(state);
			switch(enum_val) {
				case production_window_tab::factories:
					show_vector_elements(state, factory_elements);
					factbutwin->set_visible(state, true);
					this->move_child_to_front(factbutwin);
					break;
				case production_window_tab::investments:
					show_vector_elements(state, investment_elements);
					investbrowwin->set_visible(state, true);
					this->move_child_to_front(investbrowwin);
					investbutwin->set_visible(state, true);
					this->move_child_to_front(investbutwin);
					break;
				case production_window_tab::projects:
					show_vector_elements(state, project_elements);
					break;
				case production_window_tab::goods:
					show_vector_elements(state, good_elements);
					break;
			}
			active_tab = enum_val;
			return message_result::consumed;
		}
		return message_result::unseen;
	}
	
	std::vector<element_base*> factory_elements;
	std::vector<element_base*> investment_elements;
	std::vector<element_base*> project_elements;
	std::vector<element_base*> good_elements;
};

}
