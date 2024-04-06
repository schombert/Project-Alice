#pragma once

#include "gui_element_types.hpp"
#include "gui_leaders_window.hpp"
#include "gui_stats_window.hpp"
#include "gui_units_window.hpp"
#include "gui_build_unit_large_window.hpp"

namespace ui {

class military_mob_button : public right_click_button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		command::toggle_mobilization(state, state.local_player_nation);
	}

	void button_right_action(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		command::toggle_mobilization(state, n);
		command::toggle_mobilization(state, n);
	}

	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		if(state.world.nation_get_is_mobilized(n)) {
			set_button_text(state, text::produce_simple_string(state, "alice_demobilize"));
		} else {
			set_button_text(state, text::produce_simple_string(state, "alice_mobilize"));
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		active_modifiers_description(state, contents, n, 0, sys::national_mod_offsets::mobilization_impact, true);
		active_modifiers_description(state, contents, n, 0, sys::national_mod_offsets::mobilization_size, true);
		text::add_line(state, contents, "alice_mob_controls");
	}
};

class military_mob_progress_bar : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto n = state.local_player_nation;
		if(state.world.nation_get_is_mobilized(n) == false) {
			progress = 0.0f;
			return;
		}
		int32_t cur_mobilization = 0;
		for(const auto ac : state.world.nation_get_army_control_as_controller(n)) {
			for(const auto am : ac.get_army().get_army_membership()) {
				auto pop = am.get_regiment().get_pop_from_regiment_source();
				if(pop.get_poptype() != state.culture_definitions.soldiers
					&& pop.get_poptype() != state.culture_definitions.slaves
					&& pop.get_is_primary_or_accepted_culture()
					&& pop.get_poptype().get_strata() == uint8_t(culture::pop_strata::poor)) {
					cur_mobilization += int32_t(pop.get_size() * military::mobilization_size(state, n) / state.defines.pop_size_per_regiment);
				}
			}
		}
		int32_t rem_mobilization = 0;
		auto schedule_array = state.world.nation_get_mobilization_schedule(n);
		for(const auto s : schedule_array) {
			for(const auto pl : state.world.province_get_pop_location(s.where)) {
				auto pop = pl.get_pop();
				if(pop.get_poptype() != state.culture_definitions.soldiers
					&& pop.get_poptype() != state.culture_definitions.slaves
					&& pop.get_is_primary_or_accepted_culture()
					&& pop.get_poptype().get_strata() == uint8_t(culture::pop_strata::poor)) {
					rem_mobilization += int32_t(pop.get_size() * military::mobilization_size(state, n) / state.defines.pop_size_per_regiment);
				}
			}
		}
		int32_t total_mobilization = cur_mobilization + rem_mobilization;
		if(total_mobilization == 0) {
			progress = 1.f;
			return;
		}
		progress = float(cur_mobilization) / float(total_mobilization);
	}
};

class military_mob_progress_bar_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto n = state.local_player_nation;
		if(state.world.nation_get_is_mobilized(n) == false) {
			set_text(state, "0%");
			return;
		}
		int32_t cur_mobilization = 0;
		for(const auto ac : state.world.nation_get_army_control_as_controller(n)) {
			for(const auto am : ac.get_army().get_army_membership()) {
				auto pop = am.get_regiment().get_pop_from_regiment_source();
				if(military::pop_eligible_for_mobilization(state, pop)) {
					cur_mobilization += int32_t(pop.get_size() * military::mobilization_size(state, n) / state.defines.pop_size_per_regiment);
				}
			}
		}
		int32_t rem_mobilization = 0;
		auto schedule_array = state.world.nation_get_mobilization_schedule(n);
		for(const auto s : schedule_array) {
			for(const auto pl : state.world.province_get_pop_location(s.where)) {
				auto pop = pl.get_pop();
				if(military::pop_eligible_for_mobilization(state, pop)) {
					rem_mobilization += int32_t(pop.get_size() * military::mobilization_size(state, n) / state.defines.pop_size_per_regiment);
				}
			}
		}
		int32_t total_mobilization = cur_mobilization + rem_mobilization;
		if(total_mobilization == 0) {
			set_text(state, "100%");
			return;
		}
		set_text(state, text::format_percentage(float(cur_mobilization) / float(total_mobilization), 0));
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "mobilization_progress_not_mobilized");
	}
};

class military_mob_size_text : public nation_mobilization_size_text {
public:
	void on_update(sys::state& state) noexcept override {
		auto real_regs = std::max(int32_t(state.world.nation_get_recruitable_regiments(state.local_player_nation)), int32_t(state.defines.min_mobilize_limit));
		auto mob_size = std::min(int32_t(real_regs * state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::mobilization_impact)), military::mobilized_regiments_pop_limit(state, state.local_player_nation));
		set_text(state, std::to_string(mob_size));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		active_modifiers_description(state, contents, n, 0, sys::national_mod_offsets::mobilization_size, false);
	}
};

class military_mob_impact_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto v = 1.0f - military::mobilization_impact(state, state.local_player_nation);
		set_text(state, text::format_percentage(v, 0));
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		active_modifiers_description(state, contents, n, 0, sys::national_mod_offsets::mobilization_impact, false);
	}
};

class military_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		state.ui_state.military_subwindow = this;

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
			return make_element_by_type<opaque_element_base>(state, id);
		} else if(name == "headline_military") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "icon_army") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "icon_navy") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "mobilize") {
			return make_element_by_type<military_mob_button>(state, id);
		} else if(name == "demobilize") {
			return make_element_by_type<invisible_element>(state, id);
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

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(state.local_player_nation);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

} // namespace ui
