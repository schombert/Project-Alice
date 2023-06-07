#pragma once

#include "demographics.hpp"
#include "gui_element_types.hpp"
#include "gui_production_window.hpp"
#include "gui_diplomacy_window.hpp"
#include "gui_technology_window.hpp"
#include "gui_politics_window.hpp"
#include "gui_budget_window.hpp"
#include "gui_trade_window.hpp"
#include "gui_population_window.hpp"
#include "gui_military_window.hpp"
#include "gui_common_elements.hpp"
#include "nations.hpp"
#include "politics.hpp"
#include "rebels.hpp"
#include "system_state.hpp"
#include "text.hpp"
#include <vector>

namespace ui {

class topbar_budget_line_graph : public line_graph {
public:
	topbar_budget_line_graph() : line_graph(32) { }

	void on_create(sys::state& state) noexcept override {
		line_graph::on_create(state);
		on_update(state);
	}

	void on_update(sys::state& state) noexcept override {
		std::vector<float> datapoints(size_t(32));
		for(size_t i = 0; i < state.player_data_cache.treasury_record.size(); ++i)
			datapoints[i] = state.player_data_cache.treasury_record[(state.current_date.value + 1 + i) % 32] - state.player_data_cache.treasury_record[(state.current_date.value + 0 + i) % 32];
		datapoints[datapoints.size() - 1] = state.player_data_cache.treasury_record[(state.current_date.value + 1 + 31) % 32] - state.player_data_cache.treasury_record[(state.current_date.value + 0 + 31) % 32];
		datapoints[0] = datapoints[1]; // stability trick -- this compesates for the fact that the day changes early in the update, and
									   // you could see the line chart "shift" before the day actually updates

		set_data_points(state, datapoints);
	}
};

class background_image : public opaque_element_base {
public:
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		base_data.size.x = int16_t(ui_width(state));
		base_data.size.y = int16_t(ui_height(state));
		opaque_element_base::render(state, x, y);
	}

	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type t) noexcept override {
		if(is_visible()) {
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class topbar_tab_button : public checkbox_button {
public:
	void button_action(sys::state& state) noexcept override {
		auto const override_and_show_tab = [&]() {
			topbar_subwindow->set_visible(state, true);
			topbar_subwindow->impl_on_update(state);
			state.ui_state.root->move_child_to_front(topbar_subwindow);
			state.ui_state.topbar_subwindow = topbar_subwindow;
		};

		if(state.ui_state.topbar_subwindow->is_visible()) {
			state.ui_state.topbar_subwindow->set_visible(state, false);
			if(state.ui_state.topbar_subwindow != topbar_subwindow) {
				override_and_show_tab();
			}
		} else {
			override_and_show_tab();
		}
	}

	bool is_active(sys::state& state) noexcept override { return state.ui_state.topbar_subwindow == topbar_subwindow && state.ui_state.topbar_subwindow->is_visible(); }

	element_base* topbar_subwindow = nullptr;
};

class population_view_button : public topbar_tab_button {
public:
	void button_action(sys::state& state) noexcept override {
		auto const override_and_show_tab = [&]() {
			topbar_subwindow->set_visible(state, true);
			topbar_subwindow->impl_on_update(state);

			Cyto::Any payload = pop_list_filter(state.local_player_nation);
			topbar_subwindow->impl_set(state, payload);

			state.ui_state.root->move_child_to_front(topbar_subwindow);
			state.ui_state.topbar_subwindow = topbar_subwindow;
		};

		if(state.ui_state.topbar_subwindow->is_visible()) {
			state.ui_state.topbar_subwindow->set_visible(state, false);
			if(state.ui_state.topbar_subwindow != topbar_subwindow) {
				override_and_show_tab();
			}
		} else {
			override_and_show_tab();
		}
	}
};

class topbar_date_text : public simple_text_element_base {

public:
	void on_update(sys::state& state) noexcept override { set_text(state, text::date_to_string(state, state.current_date)); }

	void on_create(sys::state& state) noexcept override {
		simple_text_element_base::on_create(state);
		on_update(state);
	}
};

class topbar_pause_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(state.actual_game_speed <= 0) {
			state.actual_game_speed = state.ui_state.held_game_speed;
		} else {
			state.ui_state.held_game_speed = state.actual_game_speed.load();
			state.actual_game_speed = 0;
		}
	}

	void on_update(sys::state& state) noexcept override { disabled = state.internally_paused; }
};

class topbar_speedup_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		base_data.data.button.shortcut = sys::virtual_key::ADD;
	}

	void button_action(sys::state& state) noexcept override {
		if(state.actual_game_speed > 0) {
			state.actual_game_speed = std::min(5, state.actual_game_speed.load() + 1);
		} else {
			state.ui_state.held_game_speed = std::min(5, state.ui_state.held_game_speed + 1);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override { return tooltip_behavior::tooltip; }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("topbar_inc_speed"));
		text::close_layout_box(contents, box);
	}
};

class topbar_speeddown_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		base_data.data.button.shortcut = sys::virtual_key::MINUS;
	}

	void button_action(sys::state& state) noexcept override {
		if(state.actual_game_speed > 0) {
			state.actual_game_speed = std::max(1, state.actual_game_speed.load() - 1);
		} else {
			state.ui_state.held_game_speed = std::max(1, state.ui_state.held_game_speed - 1);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override { return tooltip_behavior::tooltip; }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("topbar_dec_speed"));
		text::close_layout_box(contents, box);
	}
};

class topbar_speed_indicator : public topbar_pause_button {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		base_data.data.button.shortcut = sys::virtual_key::SPACE;
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(state.internally_paused) {
			frame = 0;
		} else {
			frame = state.actual_game_speed;
		}
		topbar_pause_button::render(state, x, y);
	}
};

class topbar_losing_gp_status_icon : public standard_nation_icon {
public:
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override { return int32_t(!(nations::is_great_power(state, nation_id) && state.world.nation_get_rank(nation_id) > uint16_t(state.defines.great_nations_count))); }

	tooltip_behavior has_tooltip(sys::state& state) noexcept override { return tooltip_behavior::variable_tooltip; }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			if(!nations::is_great_power(state, nation_id)) {
				text::localised_format_box(state, contents, box, std::string_view("countryalert_no_gpstatus"), text::substitution_map{});
			} else if(state.world.nation_get_rank(nation_id) > uint16_t(state.defines.great_nations_count)) {
				text::localised_format_box(state, contents, box, std::string_view("countryalert_loosinggpstatus"), text::substitution_map{});
			} else if(state.world.nation_get_rank(nation_id) <= uint16_t(state.defines.great_nations_count)) {
				text::localised_format_box(state, contents, box, std::string_view("countryalert_no_loosinggpstatus"), text::substitution_map{});
			}
			text::close_layout_box(contents, box);
		}
	}
};

class topbar_at_peace_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		set_visible(state, state.world.nation_get_is_at_war(nation_id));
		return text::produce_simple_string(state, "atpeace");
	}
};

class topbar_building_factories_icon : public standard_nation_icon {
public:
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override { return int32_t(!economy::nation_is_constructing_factories(state, nation_id)); }

	tooltip_behavior has_tooltip(sys::state& state) noexcept override { return tooltip_behavior::variable_tooltip; }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			if(!economy::nation_is_constructing_factories(state, nation_id)) {
				text::localised_format_box(state, contents, box, "countryalert_no_isbuildingfactories", text::substitution_map{});
			} else if(economy::nation_is_constructing_factories(state, nation_id)) {
				text::localised_format_box(state, contents, box, "countryalert_isbuilding_factories", text::substitution_map{});
				auto nation_fat_id = dcon::fatten(state.world, nation_id);
				nation_fat_id.for_each_state_building_construction([&](dcon::state_building_construction_id building_slim_id) {
					auto building_fat_id = dcon::fatten(state.world, building_slim_id);
					auto stateName = building_fat_id.get_state().get_definition().get_name();
					auto factoryType = building_fat_id.get_type().get_name();

					text::add_line_break_to_layout_box(contents, state, box);
					text::add_to_layout_box(contents, state, box, stateName);
					text::add_space_to_layout_box(contents, state, box);
					text::add_to_layout_box(contents, state, box, factoryType);
				});
			}
			text::close_layout_box(contents, box);
		}
	}
};

class topbar_closed_factories_icon : public standard_nation_icon {
public:
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override { return int32_t(!economy::nation_has_closed_factories(state, nation_id)); }

	tooltip_behavior has_tooltip(sys::state& state) noexcept override { return tooltip_behavior::variable_tooltip; }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			if(!economy::nation_has_closed_factories(state, nation_id)) {
				text::localised_format_box(state, contents, box, "remove_countryalert_no_hasclosedfactories", text::substitution_map{});
			} else if(economy::nation_has_closed_factories(state, nation_id)) {
				text::localised_format_box(state, contents, box, "remove_countryalert_hasclosedfactories", text::substitution_map{});
				text::add_line_break_to_layout_box(contents, state, box);
				auto nation_fat = dcon::fatten(state.world, nation_id);
				for(auto staat_owner : nation_fat.get_state_ownership()) {
					auto staat = staat_owner.get_state().get_definition();
					bool new_staat = true;
					for(auto abstract : staat.get_abstract_state_membership()) {
						auto prov = abstract.get_province();
						for(auto factloc : prov.get_factory_location()) {
							auto scale = factloc.get_factory().get_production_scale();
							if(scale < 0.05) {
								if(new_staat) {
									text::add_to_layout_box(contents, state, box, staat.get_name(), text::text_color::yellow);
									// text::add_divider_to_layout_box(state, contents, box);		// TODO - Parity needed!
									text::add_line_break_to_layout_box(contents, state, box);
									new_staat = false;
								}
								text::add_to_layout_box(contents, state, box, std::string_view(" - "));
								text::add_to_layout_box(contents, state, box, factloc.get_factory().get_building_type().get_name(), text::text_color::yellow);
								text::add_line_break_to_layout_box(contents, state, box);
							}
						}
					}
				}
			}
			text::close_layout_box(contents, box);
		}
	}
};

class topbar_unemployment_icon : public standard_nation_icon {
private:
	float get_num_unemployed(sys::state& state, dcon::pop_type_id pop_type, dcon::nation_id nation_id) noexcept {
		auto total_key = demographics::to_key(state, pop_type);
		auto employment_key = demographics::to_employment_key(state, pop_type);
		return state.world.nation_get_demographics(nation_id, total_key) - state.world.nation_get_demographics(nation_id, employment_key);
	}

public:
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto primary_unemployed = get_num_unemployed(state, state.culture_definitions.primary_factory_worker, nation_id);
		auto secondary_unemployed = get_num_unemployed(state, state.culture_definitions.secondary_factory_worker, nation_id);
		return int32_t(primary_unemployed + secondary_unemployed <= 1.0f);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override { return tooltip_behavior::variable_tooltip; }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto primary_unemployed = get_num_unemployed(state, state.culture_definitions.primary_factory_worker, nation_id);
			auto secondary_unemployed = get_num_unemployed(state, state.culture_definitions.secondary_factory_worker, nation_id);

			auto box = text::open_layout_box(contents, 0);
			if(primary_unemployed + secondary_unemployed <= 0.f) {
				text::localised_format_box(state, contents, box, "countryalert_no_hasunemployedworkers", text::substitution_map{});
			} else if(primary_unemployed + secondary_unemployed >= 0.f) {
				text::localised_format_box(state, contents, box, "remove_countryalert_hasunemployedworkers", text::substitution_map{});
				auto nation_fat_id = dcon::fatten(state.world, nation_id);
				nation_fat_id.for_each_state_ownership_as_nation([&](dcon::state_ownership_id state_slim) {
					auto fat_state = dcon::fatten(state.world, state_slim);
					if(fat_state.is_valid()) {
						auto state_instance = fat_state.get_state();
						if(get_num_unemployed(state, state.culture_definitions.primary_factory_worker, nation_id) >= 10.0f && state_instance.get_demographics(demographics::to_key(state, state.culture_definitions.primary_factory_worker)) > 10.0f) {
							text::add_line_break_to_layout_box(contents, state, box);
							text::substitution_map sub;

							auto pop_fat = dcon::fatten(state.world, state.culture_definitions.primary_factory_worker);
							int32_t num_unemployed = int32_t(state_instance.get_demographics(demographics::to_key(state, state.culture_definitions.primary_factory_worker)) - state_instance.get_demographics(demographics::to_employment_key(state, state.culture_definitions.primary_factory_worker)));
							auto perc_unemployed = text::fp_two_places{num_unemployed / state_instance.get_demographics(demographics::to_key(state, state.culture_definitions.primary_factory_worker))};

							// auto pop_fat = dcon::fatten(state.world, state.culture_definitions.primary_factory_worker);
							// auto pop_name = text::produce_simple_string(state, pop_fat.get_name());
							// auto num_unemployed = int32_t(state_instance.get_demographics(demographics::total) -
							// state_instance.get_demographics(demographics::employed)); auto stateDef = state_instance.get_definition();
							// auto stateName = text::produce_simple_string(state, stateDef.get_name());
							// auto perc_unemployed = text::format_percentage(num_unemployed /
							// state_instance.get_demographics(demographics::total));
							text::add_to_substitution_map(sub, text::variable_type::num, num_unemployed);
							text::add_to_substitution_map(sub, text::variable_type::type, pop_fat.get_name());
							text::add_to_substitution_map(sub, text::variable_type::state, state_instance.get_definition().get_name());
							text::add_to_substitution_map(sub, text::variable_type::perc, perc_unemployed);
							text::localised_format_box(state, contents, box, std::string_view("topbar_unemployed"), sub);
						}
					}
				});
			} else {
				text::add_to_layout_box(contents, state, box, std::string_view("Error!"));
			}
			text::close_layout_box(contents, box);
		}
	}
};

class topbar_available_reforms_icon : public standard_nation_button {
public:
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override { return int32_t(!nations::has_reform_available(state, nation_id)); }

	tooltip_behavior has_tooltip(sys::state& state) noexcept override { return tooltip_behavior::variable_tooltip; }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			if(!nations::has_reform_available(state, nation_id)) {
				text::localised_format_box(state, contents, box, std::string_view("countryalert_no_candoreforms"), text::substitution_map{});
			} else if(nations::has_reform_available(state, nation_id)) {
				text::localised_format_box(state, contents, box, std::string_view("countryalert_candoreforms"), text::substitution_map{});
				text::add_divider_to_layout_box(state, contents, box);
				// Display Available Reforms
				// Mostly a copy of nations::has_reform_available
				auto last_date = state.world.nation_get_last_issue_or_reform_change(nation_id);
				if(bool(last_date) && (last_date + int32_t(state.defines.min_delay_between_reforms * 30.0f)) > state.current_date) {
					text::close_layout_box(contents, box);
					return;
				}
				if(state.world.nation_get_is_civilized(nation_id)) {
					for(auto i : state.culture_definitions.political_issues) {
						for(auto o : state.world.issue_get_options(i)) {
							if(o && politics::can_enact_political_reform(state, nation_id, o)) {
								auto fat_id = dcon::fatten(state.world, o);
								auto name = text::produce_simple_string(state, fat_id.get_name());
								text::add_to_layout_box(contents, state, box, name);
								text::add_line_break_to_layout_box(contents, state, box);
							}
						}
					}
					for(auto i : state.culture_definitions.social_issues) {
						for(auto o : state.world.issue_get_options(i)) {
							if(o && politics::can_enact_social_reform(state, nation_id, o)) {
								auto fat_id = dcon::fatten(state.world, o);
								auto name = text::produce_simple_string(state, fat_id.get_name());
								text::add_to_layout_box(contents, state, box, name);
								text::add_line_break_to_layout_box(contents, state, box);
							}
						}
					}
					text::close_layout_box(contents, box);
					return;
				} else {
					for(auto i : state.culture_definitions.military_issues) {
						for(auto o : state.world.reform_get_options(i)) {
							if(o && politics::can_enact_military_reform(state, nation_id, o)) {
								auto fat_id = dcon::fatten(state.world, o);
								auto name = text::produce_simple_string(state, fat_id.get_name());
								text::add_to_layout_box(contents, state, box, name);
								text::add_line_break_to_layout_box(contents, state, box);
							}
						}
					}
					for(auto i : state.culture_definitions.economic_issues) {
						for(auto o : state.world.reform_get_options(i)) {
							if(o && politics::can_enact_economic_reform(state, nation_id, o)) {
								auto fat_id = dcon::fatten(state.world, o);
								auto name = text::produce_simple_string(state, fat_id.get_name());
								text::add_to_layout_box(contents, state, box, name);
								text::add_line_break_to_layout_box(contents, state, box);
							}
						}
					}
				}
			}
			text::close_layout_box(contents, box);
		}
	}
};

class topbar_available_decisions_icon : public standard_nation_button {
public:
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override { return int32_t(!nations::has_decision_available(state, nation_id)); }

	tooltip_behavior has_tooltip(sys::state& state) noexcept override { return tooltip_behavior::variable_tooltip; }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			if(!nations::has_decision_available(state, nation_id)) {
				text::localised_format_box(state, contents, box, std::string_view("countryalert_no_candodecisions"), text::substitution_map{});
			} else if(nations::has_decision_available(state, nation_id)) {
				text::localised_format_box(state, contents, box, std::string_view("countryalert_candodecisions"), text::substitution_map{});
				// Display Available Decisions
				state.world.for_each_decision([&](dcon::decision_id di) {
					if(nation_id != state.local_player_nation || !state.world.decision_get_hide_notification(di)) {
						auto lim = state.world.decision_get_potential(di);
						if(!lim || trigger::evaluate(state, lim, trigger::to_generic(nation_id), trigger::to_generic(nation_id), 0)) {
							auto allow = state.world.decision_get_allow(di);
							if(!allow || trigger::evaluate(state, allow, trigger::to_generic(nation_id), trigger::to_generic(nation_id), 0)) {
								text::add_line_break_to_layout_box(contents, state, box);
								auto fat_id = dcon::fatten(state.world, di);
								text::add_to_layout_box(contents, state, box, fat_id.get_name(), text::text_color::yellow);
							}
						}
					}
				});
			}
			text::close_layout_box(contents, box);
		}
	}
};

class topbar_ongoing_election_icon : public standard_nation_icon {
public:
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override { return int32_t(!politics::is_election_ongoing(state, nation_id)); }

	tooltip_behavior has_tooltip(sys::state& state) noexcept override { return tooltip_behavior::variable_tooltip; }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			if(politics::has_elections(state, nation_id)) {
				if(!politics::is_election_ongoing(state, nation_id)) {
					text::localised_format_box(state, contents, box, std::string_view("countryalert_no_isinelection"), text::substitution_map{});
				} else if(politics::is_election_ongoing(state, nation_id)) {
					text::substitution_map sub;
					text::add_to_substitution_map(sub, text::variable_type::date, dcon::fatten(state.world, nation_id).get_election_ends());
					text::localised_format_box(state, contents, box, std::string_view("countryalert_isinelection"), sub);
				}
			} else {
				text::localised_format_box(state, contents, box, std::string_view("term_for_life"));
			}
			text::close_layout_box(contents, box);
		}
	}
};

class topbar_rebels_icon : public standard_nation_button {
public:
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto rebellions_iter = state.world.nation_get_rebellion_within(nation_id);
		return int32_t(rebellions_iter.begin() == rebellions_iter.end());
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override { return tooltip_behavior::variable_tooltip; }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			auto rebellions_iter = state.world.nation_get_rebellion_within(nation_id);
			if(rebellions_iter.begin() == rebellions_iter.end()) {
				text::localised_format_box(state, contents, box, std::string_view("countryalert_no_haverebels"), text::substitution_map{});
			} else if(rebellions_iter.begin() != rebellions_iter.end()) {
				text::localised_format_box(state, contents, box, std::string_view("countryalert_haverebels"), text::substitution_map{});
				auto nation_fat_id = dcon::fatten(state.world, nation_id);
				nation_fat_id.for_each_rebellion_within([&](dcon::rebellion_within_id rbl) {
					auto fat_id = dcon::fatten(state.world, rbl);
					auto rbl_fact_fat_id = fat_id.get_rebels();
					auto rbl_type_fat_id = rbl_fact_fat_id.get_type();
					auto rbl_fact_slim_id = rbl_fact_fat_id.id;
					text::add_line_break_to_layout_box(contents, state, box);
					text::substitution_map sub;
					auto rebelname = text::produce_simple_string(state, rbl_type_fat_id.get_name());
					auto rebelsize = text::prettify(rebel::get_faction_brigades_active(state, rbl_fact_slim_id));
					auto rebelOrg = text::format_percentage(rebel::get_faction_organization(state, rbl_fact_slim_id));
					text::add_to_substitution_map(sub, text::variable_type::name, std::string_view(rebelname));
					text::add_to_substitution_map(sub, text::variable_type::strength, std::string_view(rebelsize));
					text::add_to_substitution_map(sub, text::variable_type::org, std::string_view(rebelOrg));
					text::localised_format_box(state, contents, box, std::string_view("topbar_faction"), sub);
				});
				// text::add_line_break_to_layout_box(contents, state, box);
			}
			text::close_layout_box(contents, box);
		}
	}
};

class topbar_colony_icon : public standard_nation_button {
public:
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override {
		if(nations::can_expand_colony(state, nation_id)) {
			return 0;
		} else if(nations::is_losing_colonial_race(state, nation_id)) {
			return 1;
		} else {
			return 2;
		}
	}
	// TODO - when the player clicks on the colony icon and theres colonies to expand then we want to teleport their camera to the
	// colonies position & open the prov window

	tooltip_behavior has_tooltip(sys::state& state) noexcept override { return tooltip_behavior::variable_tooltip; }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto nation_fat_id = dcon::fatten(state.world, nation_id);
			auto box = text::open_layout_box(contents, 0);
			if(nations::can_expand_colony(state, nation_id)) {
				nation_fat_id.for_each_colonization([&](dcon::colonization_id colony) {
					auto colony_fat_id = dcon::fatten(state.world, colony);
					auto colState = colony_fat_id.get_state();
					auto colonyName = colState.get_name();
					text::substitution_map sub;
					text::add_to_substitution_map(sub, text::variable_type::region, colonyName);
					text::localised_format_box(state, contents, box, std::string_view("countryalert_colonialgood_state"), sub);
				});
			} else if(nations::is_losing_colonial_race(state, nation_id)) {
				nation_fat_id.for_each_colonization([&](dcon::colonization_id colony) {
					auto colony_fat_id = dcon::fatten(state.world, colony);
					auto colState = colony_fat_id.get_state();
					auto colonyName = colState.get_name();
					text::substitution_map sub;
					text::add_to_substitution_map(sub, text::variable_type::region, colonyName);
					text::localised_format_box(state, contents, box, std::string_view("countryalert_colonialbad_influence"), sub);
				});
			} else {
				text::localised_format_box(state, contents, box, std::string_view("countryalert_no_colonial"), text::substitution_map{});
			}
			text::close_layout_box(contents, box);
		}
	}
};

class topbar_crisis_icon : public standard_nation_icon {
public:
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override {
		if(state.current_crisis == sys::crisis_type::none) {
			return 2;
		} else if(state.crisis_temperature > 0.8f) {
			return 1;
		} else {
			return 0;
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override { return tooltip_behavior::variable_tooltip; }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::temperature, text::fp_two_places{state.crisis_temperature});
			if(state.current_crisis == sys::crisis_type::none) {
				text::localised_format_box(state, contents, box, std::string_view("countryalert_no_crisis"), sub);
			} else if(state.crisis_temperature > 0.8f) {
				text::localised_format_box(state, contents, box, std::string_view("countryalert_crisis"), sub);
			} else {
#define STRINGIFY(x) #x
				text::add_to_layout_box(contents, state, box, std::string_view(__FILE__ ":" STRINGIFY(__LINE__)));
#undef STRINGIFY
			}
			text::close_layout_box(contents, box);
		}
	}
};

class topbar_sphere_icon : public standard_nation_icon {
public:
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override {
		if(nations::sphereing_progress_is_possible(state, nation_id)) {
			return 0;
		} else if(rebel::sphere_member_has_ongoing_revolt(state, nation_id)) {
			return 2;
		} else {
			return 1;
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override { return tooltip_behavior::variable_tooltip; }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			if(!nations::is_great_power(state, nation_id)) {
				text::localised_format_box(state, contents, box, std::string_view("countryalert_no_gpstatus"), text::substitution_map{});
			} else {
				if(nations::sphereing_progress_is_possible(state, nation_id)) {
					text::localised_format_box(state, contents, box, std::string_view("remove_countryalert_canincreaseopinion"), text::substitution_map{});
				} else if(rebel::sphere_member_has_ongoing_revolt(state, nation_id)) {
					text::add_to_layout_box(contents, state, box,
						std::string_view("FIXME: gui/gui_topbar.hpp:404")); // TODO - if a sphere member is having a revolt then we might
																			// have to display text -breizh
				} else {
					text::localised_format_box(state, contents, box, std::string_view("remove_countryalert_no_canincreaseopinion"), text::substitution_map{});
				}
			}
			text::close_layout_box(contents, box);
		}
	}
};

class topbar_commodity_xport_icon : public image_element_base {
public:
	uint8_t slot = 0;
	dcon::commodity_id commodity_id{};
	float amount = 0.f;

	tooltip_behavior has_tooltip(sys::state& state) noexcept override { return tooltip_behavior::variable_tooltip; }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, state.world.commodity_get_name(commodity_id)), text::text_color::white);
			text::add_to_layout_box(contents, state, box, std::string_view(":"), text::text_color::white);
			text::add_space_to_layout_box(contents, state, box);
			text::add_to_layout_box(contents, state, box, text::format_float(amount, 2), text::text_color::white);
			text::close_layout_box(contents, box);
		}
	}
};

class topbar_window : public window_element_base {
private:
	dcon::nation_id current_nation{};
	std::vector<topbar_commodity_xport_icon*> import_icons;
	std::vector<topbar_commodity_xport_icon*> export_icons;
	std::vector<topbar_commodity_xport_icon*> produced_icons;

public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		auto bg_pic = make_element_by_type<background_image>(state, "bg_main_menus");
		background_pic = bg_pic.get();
		add_child_to_back(std::move(bg_pic));
		state.ui_state.topbar_window = this;
		on_update(state);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "topbarbutton_production") {
			auto btn = make_element_by_type<topbar_tab_button>(state, id);

			auto tab = make_element_by_type<production_window>(state, "country_production");
			state.ui_state.production_subwindow = state.ui_state.topbar_subwindow = btn->topbar_subwindow = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "topbarbutton_budget") {
			auto btn = make_element_by_type<topbar_tab_button>(state, id);

			auto tab = make_element_by_type<budget_window>(state, "country_budget");
			btn->topbar_subwindow = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "topbarbutton_tech") {
			auto btn = make_element_by_type<topbar_tab_button>(state, id);

			auto tab = make_element_by_type<technology_window>(state, "country_technology");
			btn->topbar_subwindow = tab.get();

			state.ui_state.technology_subwindow = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "topbarbutton_politics") {
			auto btn = make_element_by_type<topbar_tab_button>(state, id);
			auto tab = make_element_by_type<politics_window>(state, "country_politics");
			btn->topbar_subwindow = tab.get();

			state.ui_state.politics_subwindow = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "topbarbutton_pops") {
			auto btn = make_element_by_type<population_view_button>(state, id);
			auto tab = make_element_by_type<population_window>(state, "country_pop");
			btn->topbar_subwindow = tab.get();

			state.ui_state.population_subwindow = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "topbarbutton_trade") {
			auto btn = make_element_by_type<topbar_tab_button>(state, id);

			auto tab = make_element_by_type<trade_window>(state, "country_trade");
			btn->topbar_subwindow = tab.get();

			state.ui_state.trade_subwindow = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "topbarbutton_diplomacy") {
			auto btn = make_element_by_type<topbar_tab_button>(state, id);

			auto tab = make_element_by_type<diplomacy_window>(state, "country_diplomacy");
			btn->topbar_subwindow = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "topbarbutton_military") {
			auto btn = make_element_by_type<topbar_tab_button>(state, id);

			auto tab = make_element_by_type<military_window>(state, "country_military");
			btn->topbar_subwindow = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "button_speedup") {
			return make_element_by_type<topbar_speedup_button>(state, id);
		} else if(name == "button_speeddown") {
			return make_element_by_type<topbar_speeddown_button>(state, id);
		} else if(name == "pause_bg") {
			return make_element_by_type<topbar_pause_button>(state, id);
		} else if(name == "speed_indicator") {
			return make_element_by_type<topbar_speed_indicator>(state, id);
		} else if(name == "datetext") {
			return make_element_by_type<topbar_date_text>(state, id);
		} else if(name == "countryname") {
			return make_element_by_type<generic_name_text<dcon::nation_id>>(state, id);
		} else if(name == "player_flag") {
			return make_element_by_type<flag_button>(state, id);
		} else if(name == "country_prestige") {
			return make_element_by_type<nation_prestige_text>(state, id);
		} else if(name == "country_economic") {
			return make_element_by_type<nation_industry_score_text>(state, id);
		} else if(name == "country_military") {
			return make_element_by_type<nation_military_score_text>(state, id);
		} else if(name == "country_total") {
			return make_element_by_type<nation_total_score_text>(state, id);
		} else if(name == "country_colonial_power") {
			return make_element_by_type<nation_colonial_power_text>(state, id);
		} else if(name == "selected_prestige_rank") {
			return make_element_by_type<nation_prestige_rank_text>(state, id);
		} else if(name == "selected_industry_rank") {
			return make_element_by_type<nation_industry_rank_text>(state, id);
		} else if(name == "selected_military_rank") {
			return make_element_by_type<nation_military_rank_text>(state, id);
		} else if(name == "nation_totalrank") {
			return make_element_by_type<nation_rank_text>(state, id);
		} else if(name == "topbar_flag_overlay") {
			return make_element_by_type<nation_flag_frame>(state, id);
		} else if(name == "alert_building_factories") {
			return make_element_by_type<topbar_building_factories_icon>(state, id);
		} else if(name == "alert_closed_factories") {
			return make_element_by_type<topbar_closed_factories_icon>(state, id);
		} else if(name == "alert_unemployed_workers") {
			return make_element_by_type<topbar_unemployment_icon>(state, id);
		} else if(name == "budget_linechart") {
			return make_element_by_type<topbar_budget_line_graph>(state, id);
		} else if(name == "budget_funds") {
			return make_element_by_type<nation_budget_funds_text>(state, id);
		} else if(name == "topbar_tech_progress") {
			return make_element_by_type<nation_technology_research_progress>(state, id);
		} else if(name == "tech_current_research") {
			return make_element_by_type<nation_current_research_text>(state, id);
		} else if(name == "topbar_researchpoints_value") {
			return make_element_by_type<nation_daily_research_points_text>(state, id);
		} else if(name == "tech_literacy_value") {
			return make_element_by_type<nation_literacy_text>(state, id);
		} else if(name == "politics_party_icon") {
			return make_element_by_type<nation_ruling_party_ideology_plupp>(state, id);
		} else if(name == "politics_ruling_party") {
			return make_element_by_type<nation_ruling_party_text>(state, id);
		} else if(name == "politics_supressionpoints_value") {
			return make_element_by_type<nation_suppression_points_text>(state, id);
		} else if(name == "politics_infamy_value") {
			return make_element_by_type<nation_infamy_text>(state, id);
		} else if(name == "alert_can_do_reforms") {
			return make_element_by_type<topbar_available_reforms_icon>(state, id);
		} else if(name == "alert_can_do_decisions") {
			return make_element_by_type<topbar_available_decisions_icon>(state, id);
		} else if(name == "alert_is_in_election") {
			return make_element_by_type<topbar_ongoing_election_icon>(state, id);
		} else if(name == "alert_have_rebels") {
			return make_element_by_type<topbar_rebels_icon>(state, id);
		} else if(name == "population_total_value") {
			return make_element_by_type<nation_population_text>(state, id); // TODO: display daily change alongside total
		} else if(name == "topbar_focus_value") {
			return make_element_by_type<nation_focus_allocation_text>(state, id);
		} else if(name == "population_avg_mil_value") {
			return make_element_by_type<nation_militancy_text>(state, id);
		} else if(name == "population_avg_con_value") {
			return make_element_by_type<nation_consciousness_text>(state, id);
		} else if(name == "diplomacy_status") {
			return make_element_by_type<topbar_at_peace_text>(state, id);
		} else if(name == "diplomacy_at_war") {
			auto ptr = make_element_by_type<overlapping_enemy_flags>(state, id);
			ptr->base_data.position.y -= ptr->base_data.position.y / 4;
			return ptr;
		} else if(name == "diplomacy_diplopoints_value") {
			return make_element_by_type<nation_diplomatic_points_text>(state, id);
		} else if(name == "alert_colony") {
			return make_element_by_type<topbar_colony_icon>(state, id);
		} else if(name == "alert_crisis") {
			return make_element_by_type<topbar_crisis_icon>(state, id);
		} else if(name == "alert_can_increase_opinion") {
			return make_element_by_type<topbar_sphere_icon>(state, id);
		} else if(name == "alert_loosing_gp") {
			return make_element_by_type<topbar_losing_gp_status_icon>(state, id);
		} else if(name == "military_army_value") {
			return make_element_by_type<nation_brigade_allocation_text>(state, id);
		} else if(name == "military_navy_value") {
			return make_element_by_type<nation_navy_allocation_text>(state, id);
		} else if(name == "military_manpower_value") {
			return make_element_by_type<nation_mobilization_size_text>(state, id);
		} else if(name == "military_leadership_value") {
			return make_element_by_type<nation_leadership_points_text>(state, id);
		} else if(name == "topbar_outlinerbutton") {
			// Fake button isn't used - we create it manually instead...
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "topbar_import") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "topbar_export") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "topbar_produced") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name.substr(0, 13) == "topbar_import") {
			auto ptr = make_element_by_type<topbar_commodity_xport_icon>(state, id);
			ptr->slot = uint8_t(std::stoi(std::string{name.substr(13)}));
			import_icons.push_back(ptr.get());
			return ptr;
		} else if(name.substr(0, 13) == "topbar_export") {
			auto ptr = make_element_by_type<topbar_commodity_xport_icon>(state, id);
			ptr->slot = uint8_t(std::stoi(std::string{name.substr(13)}));
			export_icons.push_back(ptr.get());
			return ptr;
		} else if(name.substr(0, 15) == "topbar_produced") {
			auto ptr = make_element_by_type<topbar_commodity_xport_icon>(state, id);
			ptr->slot = uint8_t(std::stoi(std::string{name.substr(15)}));
			produced_icons.push_back(ptr.get());
			return ptr;
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(state.local_player_nation != current_nation) {
			current_nation = state.local_player_nation;
			Cyto::Any payload = current_nation;
			impl_set(state, payload);
		}

		for(auto& e : export_icons)
			e->set_visible(state, false);
		for(auto& e : import_icons)
			e->set_visible(state, false);
		for(auto& e : produced_icons)
			e->set_visible(state, false);

		{
			std::map<float, int32_t> v;
			for(dcon::commodity_id cid : state.world.in_commodity) {
				if(sys::commodity_group(state.world.commodity_get_commodity_group(cid)) != sys::commodity_group::military_goods && sys::commodity_group(state.world.commodity_get_commodity_group(cid)) != sys::commodity_group::raw_material_goods &&
					sys::commodity_group(state.world.commodity_get_commodity_group(cid)) != sys::commodity_group::industrial_goods && sys::commodity_group(state.world.commodity_get_commodity_group(cid)) != sys::commodity_group::consumer_goods)
					return;
				float produced = state.world.nation_get_domestic_market_pool(state.local_player_nation, cid);
				float consumed = state.world.nation_get_real_demand(state.local_player_nation, cid) * state.world.nation_get_demand_satisfaction(state.local_player_nation, cid);
				v.insert({produced - consumed, cid.index()});
			}

			uint8_t slot = 0;
			for(auto it = std::rbegin(v); it != std::rend(v); it++) {
				for(auto const& e : export_icons)
					if(e->slot == slot) {
						dcon::commodity_id cid = dcon::commodity_id(dcon::commodity_id::value_base_t(it->second));
						e->frame = state.world.commodity_get_icon(cid);
						e->commodity_id = cid;
						e->amount = it->first;
						e->set_visible(state, true);
					}
				++slot;
			}
			slot = 0;
			for(auto it = v.begin(); it != v.end(); it++) {
				for(auto const& e : import_icons)
					if(e->slot == slot) {
						dcon::commodity_id cid = dcon::commodity_id(dcon::commodity_id::value_base_t(it->second));
						e->frame = state.world.commodity_get_icon(cid);
						e->commodity_id = cid;
						e->amount = it->first;
						e->set_visible(state, true);
					}
				++slot;
			}
		}

		{
			std::map<float, int32_t> v;
			for(dcon::commodity_id cid : state.world.in_commodity) {
				if(sys::commodity_group(state.world.commodity_get_commodity_group(cid)) != sys::commodity_group::military_goods && sys::commodity_group(state.world.commodity_get_commodity_group(cid)) != sys::commodity_group::raw_material_goods &&
					sys::commodity_group(state.world.commodity_get_commodity_group(cid)) != sys::commodity_group::industrial_goods && sys::commodity_group(state.world.commodity_get_commodity_group(cid)) != sys::commodity_group::consumer_goods)
					return;
				v.insert({state.world.nation_get_domestic_market_pool(state.local_player_nation, cid), cid.index()});
			}

			uint8_t slot = 0;
			for(auto it = std::rbegin(v); it != std::rend(v); it++) {
				for(auto const& e : produced_icons)
					if(e->slot == slot) {
						dcon::commodity_id cid = dcon::commodity_id(dcon::commodity_id::value_base_t(it->second));
						e->frame = state.world.commodity_get_icon(cid);
						e->commodity_id = cid;
						e->amount = it->first;
						e->set_visible(state, true);
					}
				++slot;
			}
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(state.ui_state.topbar_subwindow->is_visible()) {
			background_pic->set_visible(state, true);
		} else {
			background_pic->set_visible(state, false);
		}
		window_element_base::render(state, x, y);
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(state.local_player_nation);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}

private:
	element_base* background_pic = nullptr;

	friend class topbar_tab_button;
};

} // namespace ui
