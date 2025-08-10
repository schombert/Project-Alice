#pragma once

#include "gui_element_types.hpp"
#include "gui_commodity_filters_window.hpp"
#include "gui_production_enum.hpp"

namespace ui {

class factory_prod_subsidise_all_button : public tinted_button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(!parent)
			return;

		for(auto p : state.world.nation_get_province_ownership(state.local_player_nation)) {
			for(auto fac : p.get_province().get_factory_location()) {
				if(!fac.get_factory().get_subsidized()) {
					Cyto::Any payload = commodity_filter_query_data{fac.get_factory().get_building_type().get_output(), false};
					parent->impl_get(state, payload);
					bool is_set = any_cast<commodity_filter_query_data>(payload).filter;

					if(is_set) {
						command::change_factory_settings(state, state.local_player_nation, fac.get_factory(),
								uint8_t(economy::factory_priority(state, fac.get_factory())), true);
					}
				}
			}
		}
	}

	void on_update(sys::state& state) noexcept override {
		auto rules = state.world.nation_get_combined_issue_rules(state.local_player_nation);
		disabled = (rules & issue_rule::can_subsidise) == 0;
		color = sys::pack_color(255, 255, 255);
		if(state.user_settings.color_blind_mode == sys::color_blind_mode::deutan || state.user_settings.color_blind_mode == sys::color_blind_mode::protan) {
			color = sys::pack_color(114, 150, 77); //remap to yellow
		} else if(state.user_settings.color_blind_mode == sys::color_blind_mode::achroma) {
			color = sys::pack_color(128, 128, 128);
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

class factory_prod_unsubsidise_all_button : public tinted_button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(!parent)
			return;

		for(auto p : state.world.nation_get_province_ownership(state.local_player_nation)) {
			for(auto fac : p.get_province().get_factory_location()) {
				if(fac.get_factory().get_subsidized()) {
					Cyto::Any payload = commodity_filter_query_data{fac.get_factory().get_building_type().get_output(), false};
					parent->impl_get(state, payload);
					bool is_set = any_cast<commodity_filter_query_data>(payload).filter;

					if(is_set) {
						command::change_factory_settings(state, state.local_player_nation, fac.get_factory(),
								uint8_t(economy::factory_priority(state, fac.get_factory())), false);
					}
				}
			}
		}
	}

	void on_update(sys::state& state) noexcept override {
		auto rules = state.world.nation_get_combined_issue_rules(state.local_player_nation);
		disabled = (rules & issue_rule::can_subsidise) == 0;
		color = sys::pack_color(255, 255, 255);
		if(state.user_settings.color_blind_mode == sys::color_blind_mode::deutan || state.user_settings.color_blind_mode == sys::color_blind_mode::protan) {
			color = sys::pack_color(255, 100, 255); //remap to blue
		} else if(state.user_settings.color_blind_mode == sys::color_blind_mode::achroma) {
			color = sys::pack_color(196, 196, 196);
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
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		set_visible(state, false);
	}
	void button_action(sys::state& state) noexcept override {
		
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
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		set_visible(state, false);
	}
	void button_action(sys::state& state) noexcept override {
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

enum factory_select_all_button_state {
	select_all, deselect_all
};

class factory_select_all_button : public button_element_base {
public:
	factory_select_all_button_state btn_state;

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			if(btn_state == factory_select_all_button_state::select_all) {
				for(auto com : state.world.in_commodity) {
					Cyto::Any payload = commodity_filter_query_data{ com.id, false };
					parent->impl_get(state, payload);
					bool is_set = any_cast<commodity_filter_query_data>(payload).filter;

					if(!is_set) {
						Cyto::Any payloadb = commodity_filter_toggle_data{ com.id };
						parent->impl_get(state, payloadb);
					}
				}

				btn_state = factory_select_all_button_state::deselect_all;
				impl_on_update(state);
			}
			else if(btn_state == factory_select_all_button_state::deselect_all) {
				for(auto com : state.world.in_commodity) {
					Cyto::Any payload = commodity_filter_query_data{ com.id, false };
					parent->impl_get(state, payload);
					bool is_set = any_cast<commodity_filter_query_data>(payload).filter;

					if(is_set) {
						Cyto::Any payloadb = commodity_filter_toggle_data{ com.id };
						parent->impl_get(state, payloadb);
					}
				}

				btn_state = factory_select_all_button_state::select_all;
				impl_on_update(state);
			}
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(btn_state == factory_select_all_button_state::select_all) {
			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, std::string_view("production_select_all_tooltip"));
			text::close_layout_box(contents, box);
		}
		else if(btn_state == factory_select_all_button_state::deselect_all) {
			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, std::string_view("production_deselect_all_tooltip"));
			text::close_layout_box(contents, box);
		}
	}

	void on_update(sys::state& state) noexcept override {
		set_button_text(state,
					text::produce_simple_string(state, btn_state == factory_select_all_button_state::select_all ? "PRODUCTION_SELECT_ALL" : "PRODUCTION_DESELECT_ALL"));

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

	void on_update(sys::state& state) noexcept override {
		set_button_text(state,
					text::produce_simple_string(state, retrieve<bool>(state, parent) ? "production_hide_empty_states" : "production_show_empty_states"));
		
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("production_show_empty_tooltip"));
		text::close_layout_box(contents, box);
	}
};

class factory_name_sort : public button_element_base {
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = production_sort_order::name;
			parent->impl_get(state, payload);
		}
	}
	void button_shift_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = production_sort_order::state_name;
			parent->impl_get(state, payload);
		}
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::position_sensitive_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "alice_production_name_sort_tip_province");
		text::add_line(state, contents, "alice_production_name_sort_tip_state");
	}
};

class factory_infrastructure_sort : public button_element_base {
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = production_sort_order::infrastructure;
			parent->impl_get(state, payload);
		}
	}
};

class factory_count_sort : public button_element_base {
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = production_sort_order::factories;
			parent->impl_get(state, payload);
		}
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
			return make_element_by_type<invisible_element>(state, id);

		} else if(name == "show_empty_states") {
			return make_element_by_type<factory_show_empty_states_button>(state, id);

		} else if(name == "sort_by_name") {
			return make_element_by_type<factory_name_sort>(state, id);

		} else if(name == "sort_by_factories") {
			return make_element_by_type<factory_count_sort>(state, id);

		} else if(name == "sort_by_infra") {
			return make_element_by_type<factory_infrastructure_sort>(state, id);

		} else if(name == "filter_bounds") {
			return make_element_by_type<commodity_filters_window>(state, id);

		} else {
			return nullptr;
		}
	}
};

} // namespace ui
