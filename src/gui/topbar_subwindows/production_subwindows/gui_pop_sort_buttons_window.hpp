#pragma once

#include "gui_element_types.hpp"

namespace ui {

class pop_sort_buttons_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		auto ptr = make_child(state, "clerk_sort", dcon::gui_def_id(1369));
		this->add_child_to_back(std::move(ptr));
		auto ptr2 = make_child(state, "craftsman_sort", dcon::gui_def_id(1369));
		this->add_child_to_back(std::move(ptr2));
		auto ptr3 = make_child(state, "capitalist_sort", dcon::gui_def_id(1369));
		this->add_child_to_back(std::move(ptr3));
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {

		if(name == "clerk_sort") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->set_button_text(state, text::produce_simple_string(state, state.world.pop_type_get_name(state.culture_definitions.secondary_factory_worker)));
			ptr->base_data.position.y = 0;
			ptr->base_data.position.x = 0;
			return ptr;
		} else if(name == "craftsman_sort") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->set_button_text(state, text::produce_simple_string(state, state.world.pop_type_get_name(state.culture_definitions.primary_factory_worker)));
			ptr->base_data.position.y = 0;
			ptr->base_data.position.x = 131;
			return ptr;
		} else if(name == "capitalist_sort") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->set_button_text(state, text::produce_simple_string(state, state.world.pop_type_get_name(state.culture_definitions.capitalists)));
			ptr->base_data.position.y = 0;
			ptr->base_data.position.x = 262;
			return ptr;
		} else {
			return nullptr;
		}
	}
};

}
