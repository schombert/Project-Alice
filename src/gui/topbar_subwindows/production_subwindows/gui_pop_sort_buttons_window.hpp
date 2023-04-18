#pragma once

#include "gui_element_types.hpp"

namespace ui {

class pop_sort_buttons_window : public window_element_base {
	xy_pair sort_template_offset{};
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		xy_pair base_sort_template_offset = state.ui_defs.gui[state.ui_state.defs_by_name.find("sort_by_pop_template_offset")->second.definition].position;
		sort_template_offset = base_sort_template_offset;

		auto ptr = make_child(state, "clerk_sort", state.ui_state.defs_by_name.find("sort_by_pop_template")->second.definition);
		sort_template_offset.x = base_sort_template_offset.x * 0;
		add_child_to_back(std::move(ptr));

		auto ptr2 = make_child(state, "craftsman_sort", state.ui_state.defs_by_name.find("sort_by_pop_template")->second.definition);
		sort_template_offset.x = base_sort_template_offset.x * 1;
		add_child_to_back(std::move(ptr2));

		auto ptr3 = make_child(state, "capitalist_sort", state.ui_state.defs_by_name.find("sort_by_pop_template")->second.definition);
		sort_template_offset.x = base_sort_template_offset.x * 2;
		add_child_to_back(std::move(ptr3));

		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "clerk_sort") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->set_button_text(state, text::produce_simple_string(state, state.world.pop_type_get_name(state.culture_definitions.secondary_factory_worker)));
			ptr->base_data.position = sort_template_offset;
			return ptr;
		} else if(name == "craftsman_sort") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->set_button_text(state, text::produce_simple_string(state, state.world.pop_type_get_name(state.culture_definitions.primary_factory_worker)));
			ptr->base_data.position = sort_template_offset;
			return ptr;
		} else if(name == "capitalist_sort") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->set_button_text(state, text::produce_simple_string(state, state.world.pop_type_get_name(state.culture_definitions.capitalists)));
			ptr->base_data.position = sort_template_offset;
			return ptr;
		} else {
			return nullptr;
		}
	}
};

}
