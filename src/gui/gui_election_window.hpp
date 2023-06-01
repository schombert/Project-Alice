#pragma once

#include "dcon_generated.hpp"
#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"
#include "nations.hpp"
#include "system_state.hpp"
#include "text.hpp"

namespace ui {

/*	##   ##		       ##   ##
    ##   ##  ##   ##   ##  ##   ##
    ##   ##  ##   ##   ##  ##   ##
    ##   ##  ##   ##   ##  ##   ##
     ## ##    ## #### ##    ## ##
      ###      ###  ###      ###
*/

class election_event_window : public window_element_base {
  public:
	void on_create(sys::state &state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state &state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if (name == "background") {
			return make_element_by_type<draggable_target>(state, id);

		} else if (name == "title") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if (name == "subtitle") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if (name == "description") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if (name == "state_name") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if (name == "population_amount") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if (name == "perc_of_parlament") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if (name == "ideology_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if (name == "parties_listbox") {
			return nullptr; // This should be a listbox >w<

		} else if (name == "popularity_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if (name == "chart_popularity") {
			return make_element_by_type<image_element_base>(state, id);

		} else if (name == "popularity_overlay") {
			return make_element_by_type<image_element_base>(state, id);

		} else if (name == "popularity_listbox") {
			return nullptr; // This should also be a listbox UwU

		} else if (name == "voter_issues_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if (name == "sort_by_issue_name") {
			return make_element_by_type<button_element_base>(state, id);

		} else if (name == "sort_by_voters") {
			return make_element_by_type<button_element_base>(state, id);

		} else if (name == "issues_listbox") {
			return nullptr; // OwO *notices another listbox to add*

		} else {
			return nullptr;
		}
	}
};

} // namespace ui
