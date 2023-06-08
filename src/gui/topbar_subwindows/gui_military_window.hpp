#pragma once

#include "gui_element_types.hpp"
#include "gui_leaders_window.hpp"
#include "gui_stats_window.hpp"
#include "gui_units_window.hpp"
#include "gui_build_unit_large_window.hpp"

namespace ui {

class military_mob_button : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);

		text::localised_format_box(state, contents, box, std::string_view("military_mobilize"));
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("mob_size_iro"));
		text::add_line_break_to_layout_box(state, contents, box);
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("mobilization_impact_limit_desc"));
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("mobilization_impact_limit_desc2"));
		text::add_divider_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("military_mobilize_desc"));

		text::close_layout_box(contents, box);
	}
};

class military_demob_button : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);

		text::localised_format_box(state, contents, box, std::string_view("military_demobilize"));
		text::add_divider_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("military_demobilize_desc"));

		text::close_layout_box(contents, box);
	}
};

class military_mob_progress_bar : public progress_bar {
public:
};

class military_mob_progress_bar_text : public simple_text_element_base {
public:
};

class military_mob_size_text : public nation_mobilization_size_text {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("mob_size_iro"));
		text::add_line_break_to_layout_box(state, contents, box);
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("mobilization_impact_limit_desc"));
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("mobilization_impact_limit_desc2"));
		text::close_layout_box(contents, box);
	}
};

class military_mob_impact_text : public simple_text_element_base {
public:
};

class military_window : public window_element_base {
private:
	button_element_base* mob_button = nullptr;
	button_element_base* demob_button = nullptr;
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		// Unit information comes first
		auto win1 = make_element_by_type<military_units_window<dcon::army_id>>(state,
				state.ui_state.defs_by_name.find("unit_window")->second.definition);
		win1->base_data.position = state.ui_defs.gui[state.ui_state.defs_by_name.find("army_pos")->second.definition].position;
		state.ui_state.unit_window_army = win1.get();
		add_child_to_front(std::move(win1));

		// Navy information is right next to the army information
		auto win2 = make_element_by_type<military_units_window<dcon::navy_id>>(state,
				state.ui_state.defs_by_name.find("unit_window")->second.definition);
		win2->base_data.position = state.ui_defs.gui[state.ui_state.defs_by_name.find("navy_pos")->second.definition].position;
		state.ui_state.unit_window_navy = win2.get();
		add_child_to_front(std::move(win2));

		auto build_units_window = make_element_by_type<build_unit_large_window>(state,
				state.ui_state.defs_by_name.find("build_unit_view_large")->second.definition);
		build_units_window->base_data.position.y -= 63;
		build_units_window->base_data.position.x += 1;
		state.ui_state.build_unit_window = build_units_window.get();
		build_units_window->set_visible(state, false);
		add_child_to_front(std::move(build_units_window));

		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "main_bg") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "bg_military") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "headline_military") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);

		} else if(name == "icon_army") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "icon_navy") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "mobilize") {
			auto ptr = make_element_by_type<military_mob_button>(state, id);
			ptr->set_visible(state, true);
			mob_button = ptr.get();
			return ptr;

		} else if(name == "demobilize") {
			auto ptr = make_element_by_type<military_demob_button>(state, id);
			ptr->set_visible(state, false);
			demob_button = ptr.get();
			return ptr;

		} else if(name == "mobilize_progress") {
			return make_element_by_type<military_mob_progress_bar>(state, id);

		} else if(name == "mobilize_progress_frame") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "mobilize_progress_text") {
			return make_element_by_type<military_mob_progress_bar_text>(state, id);

		} else if(name == "mob_size_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "mob_size") {
			return make_element_by_type<military_mob_size_text>(state, id);

		} else if(name == "mob_impact_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "mob_impact") {
			return make_element_by_type<military_mob_impact_text>(state, id);

		} else if(name == "leaders") {
			return make_element_by_type<leaders_window>(state, id);

		} else if(name == "stats") {
			return make_element_by_type<stats_window>(state, id);

		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		auto fat = dcon::fatten(state.world, state.local_player_nation);
		if(fat.get_is_mobilized()) {
			demob_button->set_visible(state, true);
			mob_button->set_visible(state, false);
		} else {
			demob_button->set_visible(state, false);
			mob_button->set_visible(state, true);
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(state.local_player_nation);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

} // namespace ui
