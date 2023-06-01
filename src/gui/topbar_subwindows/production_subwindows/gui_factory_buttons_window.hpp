#pragma once

#include "gui_element_types.hpp"
#include "gui_commodity_filters_window.hpp"
#include "gui_production_enum.hpp"

namespace ui {

class factory_prod_subsidise_all_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<production_action>{production_action{production_action::subsidise_all}};
			parent->impl_get(state, payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("subsidize_all_tooltip"));
		text::close_layout_box(contents, box);
	}
};

class factory_prod_unsubsidise_all_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<production_action>{production_action{production_action::unsubsidise_all}};
			parent->impl_get(state, payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("unsubsidize_all_tooltip"));
		text::close_layout_box(contents, box);
	}
};

class factory_prod_open_all_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<production_action>{production_action{production_action::open_all}};
			parent->impl_get(state, payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("open_all_tooltip"));
		text::close_layout_box(contents, box);
	}
};

class factory_prod_close_all_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<production_action>{production_action{production_action::close_all}};
			parent->impl_get(state, payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("close_all_tooltip"));
		text::close_layout_box(contents, box);
	}
};

class factory_select_all_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<production_action>{production_action{production_action::filter_select_all}};
			parent->impl_get(state, payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("production_select_all_tooltip"));
		text::close_layout_box(contents, box);
	}
};

class factory_deselect_all_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<production_action>{production_action{production_action::filter_deselect_all}};
			parent->impl_get(state, payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("production_deselect_all_tooltip"));
		text::close_layout_box(contents, box);
	}
};

class factory_show_empty_states_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = bool{};
			parent->impl_get(state, payload);
			auto content = any_cast<bool>(payload);

			Cyto::Any payload2 = element_selection_wrapper<bool>{!content};
			parent->impl_get(state, payload2);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("production_show_empty_tooltip"));
		text::close_layout_box(contents, box);
	}
};

class factory_buttons_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "prod_subsidize_all") {
			return make_element_by_type<factory_prod_subsidise_all_button>(state, id);

		} else if(name == "prod_unsubsidize_all") {
			return make_element_by_type<factory_prod_unsubsidise_all_button>(state, id);

		} else if(name == "prod_open_all_factories") {
			return make_element_by_type<factory_prod_open_all_button>(state, id);

		} else if(name == "prod_close_all_factories") {
			return make_element_by_type<factory_prod_close_all_button>(state, id);

		} else if(name == "select_all") {
			return make_element_by_type<factory_select_all_button>(state, id);

		} else if(name == "deselect_all") {
			return make_element_by_type<factory_deselect_all_button>(state, id);

		} else if(name == "show_empty_states") {
			return make_element_by_type<factory_show_empty_states_button>(state, id);

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
};

}
