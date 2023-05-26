#pragma once

#include "gui_element_types.hpp"

namespace ui {

class factory_prod_subsidise_all_button : public button_element_base {
public:
	message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
		return message_result::consumed;
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
	message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
		return message_result::consumed;
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
	message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
		return message_result::consumed;
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
	message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
		return message_result::consumed;
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
	message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
		return message_result::consumed;
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
	message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
		return message_result::consumed;
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

	message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
		return message_result::consumed;
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

class factory_good_filter_button : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(contents, state, box, std::string_view("UwU"));
		text::close_layout_box(contents, box);
	}
};

class factory_good_filter_template : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "filter_button") {
			///return make_element_by_type<factory_good_filter_button>(state, id);
			int32_t x = base_data.position.x;
			x = x;
			int32_t y = base_data.position.y;
			y = y;
			state.world.for_each_commodity([&](dcon::commodity_id cid) {
				//auto ptr = make_element_by_type<factory_good_filter_button>(state, "goods_filter_template");
				auto ptr = make_element_by_type<factory_good_filter_button>(state, id);
				if(cid.value == (state.world.commodity_size() / 2)) {
					x = base_data.position.x;
					y = base_data.position.y - ptr->base_data.size.y;
				} else {
					x += ptr->base_data.size.x;
				}
				ptr->base_data.position.x = x;
				ptr->base_data.position.y = y;
				add_child_to_front(std::move(ptr));
			});
		} else if(name == "filter_enabled") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "goods_type") {
			return make_element_by_type<image_element_base>(state, id);
		}
		return nullptr;
	}
};

class factory_filter_button : public container_base {
public:
	void on_create(sys::state& state) noexcept override {
		make_element_by_type<factory_good_filter_template>(state, "goods_filter_template");
	}
};

class factory_buttons_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

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
			return make_element_by_type<factory_filter_button>(state, id);

		} else {
			return nullptr;
		}
	}
};

}
