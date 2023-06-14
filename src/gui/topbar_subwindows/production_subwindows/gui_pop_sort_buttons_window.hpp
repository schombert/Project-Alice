#pragma once

#include "gui_element_types.hpp"

namespace ui {

class pop_sort_buttons_window : public window_element_base {
	xy_pair sort_template_offset{};

public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		xy_pair base_sort_template_offset =
				state.ui_defs.gui[state.ui_state.defs_by_name.find("sort_by_pop_template_offset")->second.definition].position;
		sort_template_offset = base_sort_template_offset;

		auto ptr = make_element_by_type<button_element_base>(state,
				state.ui_state.defs_by_name.find("sort_by_pop_template")->second.definition);
		ptr->set_button_text(state,
				text::produce_simple_string(state, state.world.pop_type_get_name(state.culture_definitions.secondary_factory_worker)));
		sort_template_offset.x = base_sort_template_offset.x * 0;
		ptr->base_data.position = sort_template_offset;
		add_child_to_back(std::move(ptr));

		auto ptr2 = make_element_by_type<button_element_base>(state,
				state.ui_state.defs_by_name.find("sort_by_pop_template")->second.definition);
		ptr2->set_button_text(state,
				text::produce_simple_string(state, state.world.pop_type_get_name(state.culture_definitions.primary_factory_worker)));
		sort_template_offset.x = base_sort_template_offset.x * 1;
		ptr2->base_data.position = sort_template_offset;
		add_child_to_back(std::move(ptr2));

		auto ptr3 = make_element_by_type<button_element_base>(state,
				state.ui_state.defs_by_name.find("sort_by_pop_template")->second.definition);
		ptr3->set_button_text(state,
				text::produce_simple_string(state, state.world.pop_type_get_name(state.culture_definitions.capitalists)));
		sort_template_offset.x = base_sort_template_offset.x * 2;
		ptr3->base_data.position = sort_template_offset;
		add_child_to_back(std::move(ptr3));

		set_visible(state, false);
	}
};

} // namespace ui
