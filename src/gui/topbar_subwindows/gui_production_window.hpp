#pragma once

#include "gui_element_types.hpp"
#include "gui_factory_buttons_window.hpp"
#include "gui_invest_brow_window.hpp"
#include "gui_invest_buttons_window.hpp"
#include "gui_pop_sort_buttons_window.hpp"
#include "gui_goods_filter_window.hpp"
#include <vector>

namespace ui {

enum class production_window_tab : uint8_t {
	factories = 0x0,
	investments = 0x1,
	projects = 0x2,
	goods = 0x3
};

class production_state_info : public listbox_row_element_base<dcon::state_instance_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "state_focus") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "state_name") {
			return make_element_by_type<state_name_text>(state, id);
		} else if(name == "factory_count") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "build_new_factory") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "avg_infra_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

class production_state_listbox : public listbox_element_base<production_state_info, dcon::state_instance_id> {
protected:
	std::string_view get_row_element_name() override {
        return "state_info";
    }
public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		state.world.for_each_state_instance([&](dcon::state_instance_id id) {
			auto nation_id = state.world.state_instance_get_nation_from_state_ownership(id);
			if(state.local_player_nation == nation_id)
				row_contents.push_back(id);
		});
		update(state);
	}
};

class production_window : public generic_tabbed_window<production_window_tab> {
	production_state_listbox* state_listbox = nullptr;
public:
	void on_create(sys::state& state) noexcept override {
		generic_tabbed_window::on_create(state);
		auto ptr = make_child(state, "goods_filter_template", state.ui_state.defs_by_name.find("goods_filter_template")->second.definition);
		this->add_child_to_front(std::move(ptr));
		set_visible(state, false);
	}
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
			factory_elements.push_back(ptr.get());
			ptr->set_visible(state, true);
			return ptr;
		} else if(name == "state_listbox") {
			auto ptr = make_element_by_type<production_state_listbox>(state, id);
			state_listbox = ptr.get();
			factory_elements.push_back(ptr.get());
			ptr->set_visible(state, true);
			return ptr;
		} else if(name == "investment_browser") {
			auto ptr = make_element_by_type<invest_brow_window>(state, id);
			investment_brow_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "invest_buttons") {
			auto ptr = make_element_by_type<invest_buttons_window>(state, id);
			investment_button_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "sort_by_state") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			project_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "sort_by_projects") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			project_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "sort_by_completion") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			project_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "sort_by_projecteers") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			project_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "pop_sort_buttons") {
			auto ptr = make_element_by_type<pop_sort_buttons_window>(state, id);
			factory_elements.push_back(ptr.get());
			ptr->set_visible(state, true);
			return ptr;
		} else if(name == "goods_filter_template") {
			auto ptr = make_element_by_type<goods_filter_window>(state, id);
			factory_elements.push_back(ptr.get());
			ptr->set_visible(state, true);
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
		hide_vector_elements(state, investment_brow_elements);
		hide_vector_elements(state, investment_button_elements);
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
					break;
				case production_window_tab::investments:
					show_vector_elements(state, investment_brow_elements);
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
	std::vector<element_base*> investment_brow_elements;
	std::vector<element_base*> investment_button_elements;
	std::vector<element_base*> project_elements;
	std::vector<element_base*> good_elements;
};

}
