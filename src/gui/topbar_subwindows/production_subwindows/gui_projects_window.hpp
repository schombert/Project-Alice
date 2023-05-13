#pragma once

#include <variant>
#include "gui_element_types.hpp"

namespace ui {

typedef std::variant<
    dcon::province_building_construction_id,
    dcon::state_building_construction_id
> production_project_data;

class production_project_info : public listbox_row_element_base<production_project_data> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "state_bg") {
			return make_element_by_type<image_element_base>(state, id);
        } else if(name == "state_name") {
            return make_element_by_type<simple_text_element_base>(state, id);
        } else if(name == "project_icon") {
            return make_element_by_type<image_element_base>(state, id);
        } else if(name == "infra") {
            return make_element_by_type<image_element_base>(state, id);
        } else if(name == "project_name") {
            return make_element_by_type<simple_text_element_base>(state, id);
        } else if(name == "project_cost") {
            return make_element_by_type<simple_text_element_base>(state, id);
        } else if(name == "project_cost") {
            return make_element_by_type<simple_text_element_base>(state, id);
        } else if(name == "pop_icon") {
            return make_element_by_type<simple_text_element_base>(state, id);
        } else if(name == "pop_amount") {
            return make_element_by_type<simple_text_element_base>(state, id);
        } else if(name == "invest_project") {
            return make_element_by_type<button_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

class production_project_listbox : public listbox_element_base<production_project_info, production_project_data> {
protected:
	std::string_view get_row_element_name() override {
        return "project_info";
    }
};
}
