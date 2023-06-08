#pragma once

#include "gui_production_window.hpp"
#include "gui_production_enum.hpp"

namespace ui {

class production_foreign_investment_window : public window_element_base {
	private:
	dcon::nation_id curr_nation{};

	public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "prod_investment_bg") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "country_flag") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "country_flag_overlay") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "country_name") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "country_investment") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "invest_country_browse") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "select_all") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "deselect_all") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "show_empty_states") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "sort_by_name") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "sort_by_factories") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "sort_by_infra") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "filter_bounds") {
			return make_element_by_type<commodity_filters_window>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<element_selection_wrapper<dcon::nation_id>>()) {
			curr_nation = any_cast<element_selection_wrapper<dcon::nation_id>>(payload).data;
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

} // namespace ui
