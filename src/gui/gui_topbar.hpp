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

/*
 * NOTE -
 * 	None of the classes in here are "redundent" or "pointless",
 * 	there existance is to prevent tooltip cross contaimination
 * 	DO NOT FUCKING REMOVE THEM, its was enough of a issue having
 * 	to readd them the last time they got removed.
 */

class topbar_nation_name : public generic_name_text<dcon::nation_id> {
	public:
};

class topbar_flag_button : public flag_button {
	public:
};

class topbar_nation_prestige_text : public nation_prestige_text {
	public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, std::string_view("rank_prestige"), text::substitution_map{});
			text::add_line_break_to_layout_box(contents, state, box);
			switch(nations::get_status(state, nation_id)) {
			case(nations::status::great_power):
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_greatnation_status"),
						text::substitution_map{});
				break;
			case(nations::status::secondary_power):
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_colonialnation_status"),
						text::substitution_map{});
				break;
			case(nations::status::civilized):
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_civilizednation_status"),
						text::substitution_map{});
				// Civil
				break;
			case(nations::status::westernizing):
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_almost_western_nation_status"),
						text::substitution_map{});
				// ???
				break;
			case(nations::status::uncivilized):
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_uncivilizednation_status"),
						text::substitution_map{});
				// Nothing
				break;
			case(nations::status::primitive):
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_primitivenation_status"),
						text::substitution_map{});
				// Nothing
				break;
			default:
				break;
			};
			text::add_divider_to_layout_box(state, contents, box);
			text::localised_format_box(state, contents, box, std::string_view("rank_prestige_d"), text::substitution_map{});
			text::close_layout_box(contents, box);
		}
	}
};

class topbar_nation_industry_score_text : public nation_industry_score_text {
	public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, std::string_view("rank_industry"), text::substitution_map{});
			text::add_divider_to_layout_box(state, contents, box);
			text::localised_format_box(state, contents, box, std::string_view("rank_industry_d"), text::substitution_map{});
			text::close_layout_box(contents, box);
		}
	}
};

class topbar_nation_military_score_text : public nation_military_score_text {
	public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, std::string_view("rank_military"), text::substitution_map{});
			text::add_divider_to_layout_box(state, contents, box);
			text::localised_format_box(state, contents, box, std::string_view("rank_military_d"), text::substitution_map{});
			text::close_layout_box(contents, box);
		}
	}
};

class topbar_nation_total_score_text : public nation_total_score_text {
	public:
};

class topbar_nation_colonial_power_text : public nation_colonial_power_text {
	public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			float points = 0.f;
			state.world.for_each_technology([&](dcon::technology_id t) {
				if(state.world.nation_get_active_technologies(nation_id, t))
					points += float(state.world.technology_get_colonial_points(t));
			});

			text::substitution_map sub;
			std::string value;
			value = text::prettify(nations::free_colonial_points(state, nation_id));
			text::add_to_substitution_map(sub, text::variable_type::value, std::string_view(value));
			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, std::string_view("colonial_points"), sub);
			text::add_divider_to_layout_box(state, contents, box);
			text::localised_format_box(state, contents, box, std::string_view("available_colonial_power"), sub);
			text::add_line_break_to_layout_box(contents, state, box);
			text::localised_format_box(state, contents, box, std::string_view("from_technology"), sub);
			text::add_space_to_layout_box(contents, state, box);
			text::add_to_layout_box(contents, state, box, text::format_float(points, 0), text::text_color::green);

			text::close_layout_box(contents, box);
		}
	}
};

class topbar_nation_prestige_rank_text : public nation_prestige_rank_text {
	public:
};

class topbar_nation_industry_rank_text : public nation_industry_rank_text {
	public:
};

class topbar_nation_military_rank_text : public nation_military_rank_text {
	public:
};

class topbar_nation_rank_text : public nation_rank_text {
	public:
};

class topbar_nation_flag_frame : public nation_flag_frame {
	public:
};

class topbar_nation_budget_funds_text : public nation_budget_funds_text {
	public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);

			text::substitution_map sub{};

			auto total_income = economy::estimate_tax_income_by_strata(state, nation_id, culture::pop_strata::poor);
			total_income += economy::estimate_tax_income_by_strata(state, nation_id, culture::pop_strata::middle);
			total_income += economy::estimate_tax_income_by_strata(state, nation_id, culture::pop_strata::rich);
			total_income += economy::estimate_gold_income(state, nation_id);

			auto total_expense = economy::estimate_construction_spending(state, nation_id);
			total_expense += economy::estimate_land_spending(state, nation_id);
			total_expense += economy::estimate_naval_spending(state, nation_id);
			total_expense += economy::estimate_social_spending(state, nation_id);
			total_expense += economy::estimate_pop_payouts_by_income_type(state, nation_id, culture::income_type::education);
			total_expense += economy::estimate_pop_payouts_by_income_type(state, nation_id, culture::income_type::administration);
			total_expense += economy::estimate_pop_payouts_by_income_type(state, nation_id, culture::income_type::military);
			total_expense += economy::estimate_loan_payments(state, nation_id);
			total_expense += economy::estimate_subsidy_spending(state, nation_id);

			text::add_to_substitution_map(sub, text::variable_type::yesterday,
					text::fp_one_place{nations::get_yesterday_income(state, nation_id)});
			text::add_to_substitution_map(sub, text::variable_type::cash, text::fp_one_place{nations::get_treasury(state, nation_id)});

			text::localised_format_box(state, contents, box, std::string_view("topbar_funds"), sub); // $YESTERDAY && $CASH
			text::add_divider_to_layout_box(state, contents, box);
			text::localised_single_sub_box(state, contents, box, std::string_view("budget_total_income"), text::variable_type::val,
					text::fp_three_places{total_income}); // $VAL
			text::add_line_break_to_layout_box(contents, state, box);
			text::localised_single_sub_box(state, contents, box, std::string_view("taxes_poor"), text::variable_type::val,
					text::fp_one_place{economy::estimate_tax_income_by_strata(state, nation_id, culture::pop_strata::poor)}); // $VAL
			text::add_line_break_to_layout_box(contents, state, box);
			text::localised_single_sub_box(state, contents, box, std::string_view("taxes_middle"), text::variable_type::val,
					text::fp_one_place{economy::estimate_tax_income_by_strata(state, nation_id, culture::pop_strata::middle)}); // $VAL
			text::add_line_break_to_layout_box(contents, state, box);
			text::localised_single_sub_box(state, contents, box, std::string_view("taxes_rich"), text::variable_type::val,
					text::fp_one_place{economy::estimate_tax_income_by_strata(state, nation_id, culture::pop_strata::rich)}); // $VAL
			text::add_line_break_to_layout_box(contents, state, box);
			text::localised_single_sub_box(state, contents, box, std::string_view("tariffs_income"), text::variable_type::val,
					text::fp_one_place{economy::estimate_tariff_income(state, nation_id)}); // $VAL
			text::add_line_break_to_layout_box(contents, state, box);
			text::localised_format_box(state, contents, box, std::string_view("budget_exports"), text::substitution_map{}); // $VAL	TODO
			text::add_line_break_to_layout_box(contents, state, box);
			text::localised_single_sub_box(state, contents, box, std::string_view("budget_gold"), text::variable_type::val,
					text::fp_one_place{economy::estimate_gold_income(state, nation_id)}); // $VAL
			text::add_line_break_to_layout_box(contents, state, box);
			text::localised_single_sub_box(state, contents, box, std::string_view("budget_total_expense"), text::variable_type::val,
					text::fp_three_places{-total_expense}); // $VAL	TODO
			text::add_line_break_to_layout_box(contents, state, box);
			text::localised_single_sub_box(state, contents, box, std::string_view("budget_expense_slider_education"),
					text::variable_type::val,
					text::fp_three_places{
							-economy::estimate_pop_payouts_by_income_type(state, nation_id, culture::income_type::education)}); // $VAL
			text::add_line_break_to_layout_box(contents, state, box);
			text::localised_single_sub_box(state, contents, box, std::string_view("budget_slider_administration"),
					text::variable_type::val,
					text::fp_three_places{
							-economy::estimate_pop_payouts_by_income_type(state, nation_id, culture::income_type::administration)}); // $VAL
			text::add_line_break_to_layout_box(contents, state, box);
			text::localised_single_sub_box(state, contents, box, std::string_view("budget_slider_social_spending"),
					text::variable_type::val,
					text::fp_three_places{
							-economy::estimate_social_spending(state, nation_id)}); // $VAL - presumably loan payments == interest (?)
			text::add_line_break_to_layout_box(contents, state, box);
			text::localised_single_sub_box(state, contents, box, std::string_view("budget_slider_military_spending"),
					text::variable_type::val,
					text::fp_two_places{
							-economy::estimate_pop_payouts_by_income_type(state, nation_id, culture::income_type::military)}); // $VAL
			text::add_line_break_to_layout_box(contents, state, box);
			text::localised_single_sub_box(state, contents, box, std::string_view("budget_interest"), text::variable_type::val,
					text::fp_one_place{
							-economy::estimate_loan_payments(state, nation_id)}); // $VAL - presumably loan payments == interest (?)
			text::add_line_break_to_layout_box(contents, state, box);
			text::localised_single_sub_box(state, contents, box, std::string_view("budget_imports"), text::variable_type::val,
					text::fp_one_place{-economy::nation_total_imports(state,
							nation_id)}); // $VAL - presumably nation_total_imports is for national stockpile (?)
			text::add_line_break_to_layout_box(contents, state, box); // Intentional, theres meant to be two new lines here.
			text::add_line_break_to_layout_box(contents, state, box);
			text::localised_single_sub_box(state, contents, box, std::string_view("topbar_projected_income"), text::variable_type::val,
					text::fp_two_places{economy::estimate_daily_income(state, nation_id)}); // $VAL	TODO

			text::close_layout_box(contents, box);
		}
	}
};

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
			datapoints[i] = state.player_data_cache.treasury_record[(state.current_date.value + 1 + i) % 32] -
											state.player_data_cache.treasury_record[(state.current_date.value + 0 + i) % 32];
		datapoints[datapoints.size() - 1] = state.player_data_cache.treasury_record[(state.current_date.value + 1 + 31) % 32] -
																				state.player_data_cache.treasury_record[(state.current_date.value + 0 + 31) % 32];
		datapoints[0] = datapoints[1]; // stability trick -- this compesates for the fact that the day changes early in the
																	 // update, and you could see the line chart "shift" before the day actually updates

		set_data_points(state, datapoints);
	}
};

class topbar_nation_current_research_text : public nation_current_research_text {
	public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto tech_id = nations::current_research(state, nation_id);
			auto box = text::open_layout_box(contents, 0);
			// TODO - check for other research?
			if(tech_id) {
				auto tech_fat = dcon::fatten(state.world, tech_id);
				text::substitution_map sub;
				text::add_to_substitution_map(sub, text::variable_type::tech, tech_fat.get_name());
				text::add_to_substitution_map(sub, text::variable_type::date,
						nations::get_research_end_date(state, tech_id, state.local_player_nation));
				text::add_to_substitution_map(sub, text::variable_type::invested,
						text::fp_one_place{state.world.nation_get_research_points(nation_id)});
				text::add_to_substitution_map(sub, text::variable_type::cost, dcon::fatten(state.world, tech_id).get_cost());

				text::localised_format_box(state, contents, box, std::string_view("technologyview_research_tooltip"), sub);
				text::add_line_break_to_layout_box(contents, state, box);
				text::localised_format_box(state, contents, box, std::string_view("technologyview_research_invested_tooltip"), sub);
			} else {
				text::localised_format_box(state, contents, box, std::string_view("technologyview_no_research_tooltip"));
			}
			text::close_layout_box(contents, box);
		}
	}
};

class topbar_nation_technology_research_progress : public nation_technology_research_progress {
	public:
};

class topbar_nation_literacy_text : public nation_literacy_text {
	public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::substitution_map sub;
			auto literacy_change = (demographics::get_estimated_literacy_change(state, nation_id) / 30);
			text::add_to_substitution_map(sub, text::variable_type::val,
					text::fp_four_places{literacy_change}); // TODO - This needs to display the estimated literacy change -breizh
			auto avg_literacy = text::format_percentage((state.world.nation_get_demographics(nation_id, demographics::literacy) /
																											state.world.nation_get_demographics(nation_id, demographics::total)),
					1);
			text::add_to_substitution_map(sub, text::variable_type::avg, std::string_view(avg_literacy));
			text::localised_format_box(state, contents, box, std::string_view("topbar_avg_literacy"), sub);
			text::add_line_break_to_layout_box(contents, state, box);
			text::localised_format_box(state, contents, box, std::string_view("topbar_avg_change"), sub);
			text::close_layout_box(contents, box);
		}
	}
};

class topbar_nation_ruling_party_ideology_plupp : public nation_ruling_party_ideology_plupp {
	public:
};

class topbar_nation_ruling_party_text : public nation_ruling_party_text {
	public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto fat_id = dcon::fatten(state.world, nation_id);

			std::string ruling_party = text::get_name_as_string(state, fat_id.get_ruling_party());
			ruling_party = ruling_party + " (" +
										 text::get_name_as_string(state,
												 state.world.political_party_get_ideology(state.world.nation_get_ruling_party(nation_id))) +
										 ")";

			auto box = text::open_layout_box(contents, 0);
			text::localised_single_sub_box(state, contents, box, std::string_view("topbar_ruling_party"), text::variable_type::curr,
					std::string_view(ruling_party));
			text::add_divider_to_layout_box(state, contents, box);
			text::close_layout_box(contents, box);
		}
	}
};

class topbar_nation_suppression_points_text : public nation_suppression_points_text {
	public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, "suppression_points", text::substitution_map{});
			text::add_space_to_layout_box(contents, state, box);
			text::add_to_layout_box(contents, state, box, text::fp_two_places{nations::suppression_points(state, nation_id)});
			text::close_layout_box(contents, box);

			active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::suppression_points_modifier, false);
		}
	}
};

class topbar_nation_infamy_text : public nation_infamy_text {
	public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			auto fat_id = dcon::fatten(state.world, nation_id);
			text::localised_single_sub_box(state, contents, box, std::string_view("diplomacy_infamy"), text::variable_type::value,
					text::fp_two_places{fat_id.get_infamy()});
			text::add_divider_to_layout_box(state, contents, box);
			// text::add_to_layout_box(contents, state, box, text::format_ratio(fat_id.get_infamy(), ));
			text::add_to_layout_box(contents, state, box, text::fp_two_places{fat_id.get_infamy()});
			text::add_to_layout_box(contents, state, box, std::string_view("/"));
			text::add_to_layout_box(contents, state, box, text::fp_two_places{state.defines.badboy_limit});
			text::add_line_break_to_layout_box(contents, state, box);
			text::localised_format_box(state, contents, box, std::string_view("badboy_dro_1"));
			text::close_layout_box(contents, box);

			active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::badboy, false);
		}
	}
};

class topbar_nation_population_text : public nation_population_text {
	public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);

			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::curr,
					text::pretty_integer{int32_t(state.world.nation_get_demographics(nation_id, demographics::total))});

			text::localised_format_box(state, contents, box, std::string_view("topbar_population"), sub);
			text::add_divider_to_layout_box(state, contents, box);
			text::localised_single_sub_box(state, contents, box, std::string_view("topbar_population_visual"),
					text::variable_type::curr,
					text::pretty_integer{int32_t(state.world.nation_get_demographics(nation_id, demographics::total)) * 4});
			text::close_layout_box(contents, box);

			// if(state.user_settings.guimode == sys::gui_modes::inaccurate) {
			if(!state.user_settings.guimode) {
				active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::pop_growth, false);
			}
		}
	}
};

class topbar_nation_focus_allocation_text : public nation_focus_allocation_text {
	public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::substitution_map sub1;
			float relevant_pop = state.world.nation_get_demographics(nation_id,
					demographics::to_key(state, state.world.nation_get_primary_culture(nation_id)));
			for(auto ac : state.world.nation_get_accepted_cultures(nation_id)) {
				relevant_pop += state.world.nation_get_demographics(nation_id, demographics::to_key(state, ac));
			}
			text::add_to_substitution_map(sub1, text::variable_type::num, text::pretty_integer{(int64_t)relevant_pop});
			auto fPoints = relevant_pop / state.defines.national_focus_divider; // NOTE: Occasionally inaccurate by a few 0.01, this
																																					// doesnt really matter so im leaving it -breizh
			text::add_to_substitution_map(sub1, text::variable_type::focus, text::fp_two_places{fPoints});
			text::localised_format_box(state, contents, box, std::string_view("tb_nationalfocus_culture"), sub1);
			text::add_line_break_to_layout_box(contents, state, box);
			text::substitution_map sub2;
			text::add_to_substitution_map(sub2, text::variable_type::focus, nations::max_national_focuses(state, nation_id));
			text::localised_format_box(state, contents, box, std::string_view("tb_max_focus"), sub2);
			if(nations::national_focuses_in_use(state, nation_id) > 0) {
				text::add_divider_to_layout_box(state, contents, box);
				auto nation_fat_id = dcon::fatten(state.world, nation_id);
				nation_fat_id.for_each_state_ownership([&](dcon::state_ownership_id so) {
					auto fat_state_id = dcon::fatten(state.world, so);
					if(fat_state_id.is_valid()) {
						auto staat = fat_state_id.get_state();
						if(staat.is_valid()) {
							auto natl_fat_id = staat.get_owner_focus();
							if(natl_fat_id.is_valid()) {
								text::add_to_layout_box(contents, state, box, natl_fat_id.get_name());
								text::add_line_break_to_layout_box(contents, state, box);
							}
						}
					}
				});
			} else {
				text::add_divider_to_layout_box(state, contents, box);
				text::localised_format_box(state, contents, box, std::string_view("tb_nationalfocus_none"), text::substitution_map{});
			}
			text::close_layout_box(contents, box);

			// if(state.user_settings.guimode == sys::gui_modes::inaccurate) {
			if(!state.user_settings.guimode) {
				active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::max_national_focus, false);
			}
		}
	}
};

class topbar_nation_militancy_text : public nation_militancy_text {
	public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::substitution_map sub;
			auto mil_change = (demographics::get_estimated_mil_change(state, nation_id) / 30);
			text::add_to_substitution_map(sub, text::variable_type::avg,
					text::fp_two_places{(state.world.nation_get_demographics(nation_id, demographics::militancy) /
															 state.world.nation_get_demographics(nation_id, demographics::total))});
			text::add_to_substitution_map(sub, text::variable_type::val,
					text::fp_four_places{mil_change}); // TODO - This needs to display the estimated militancy change -breizh
			text::localised_format_box(state, contents, box, std::string_view("topbar_avg_mil"), sub);
			text::add_line_break_to_layout_box(contents, state, box);
			text::localised_format_box(state, contents, box, std::string_view("topbar_avg_change"), sub);
			text::close_layout_box(contents, box);

			// if(state.user_settings.guimode == sys::gui_modes::inaccurate) {
			if(!state.user_settings.guimode) {
				active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::global_pop_militancy_modifier,
						true);
				active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::core_pop_militancy_modifier, true);
				active_modifiers_description(state, contents, nation_id, 0,
						sys::national_mod_offsets::non_accepted_pop_militancy_modifier, true);
			}
		}
	}
};

class topbar_nation_consciousness_text : public nation_consciousness_text {
	public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::substitution_map sub;
			auto con_change = (demographics::get_estimated_con_change(state, nation_id) / 30);
			text::add_to_substitution_map(sub, text::variable_type::avg,
					text::fp_two_places{(state.world.nation_get_demographics(nation_id, demographics::consciousness) /
															 state.world.nation_get_demographics(nation_id, demographics::total))});
			// text::add_to_substitution_map(sub, text::variable_type::val, text::format_float(fDailyUpdate, 2);
			text::add_to_substitution_map(sub, text::variable_type::val,
					text::fp_four_places{con_change}); // TODO - This needs to display the estimated consciousness change -breizh
			text::localised_format_box(state, contents, box, std::string_view("topbar_avg_con"), sub);
			text::add_line_break_to_layout_box(contents, state, box);
			text::localised_format_box(state, contents, box, std::string_view("topbar_avg_change"), sub);
			text::close_layout_box(contents, box);

			// if(state.user_settings.guimode == sys::gui_modes::inaccurate) {
			if(!state.user_settings.guimode) {
				active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::global_pop_consciousness_modifier,
						true);
				active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::core_pop_consciousness_modifier,
						true);
				active_modifiers_description(state, contents, nation_id, 0,
						sys::national_mod_offsets::non_accepted_pop_consciousness_modifier, true);
			}
		}
	}
};

class topbar_overlapping_enemy_flags : public overlapping_enemy_flags {
	public:
};

class topbar_nation_diplomatic_points_text : public nation_diplomatic_points_text {
	public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::curr,
					text::fp_one_place{nations::diplomatic_points(state, nation_id)});
			text::localised_format_box(state, contents, box, std::string_view("topbar_diplopoints"), sub);
			text::add_divider_to_layout_box(state, contents, box);
			text::localised_format_box(state, contents, box, std::string_view("topbar_diplopoints_gain"), sub);
			text::add_line_break_to_layout_box(contents, state, box);
			text::localised_format_box(state, contents, box, std::string_view("topbar_diplopoints_basegain"));
			text::close_layout_box(contents, box);

			active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::diplomatic_points_modifier, false);
		}
	}
};

class topbar_nation_brigade_allocation_text : public nation_brigade_allocation_text {
	public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::curr, state.world.nation_get_active_regiments(nation_id));
			text::add_to_substitution_map(sub, text::variable_type::max,
					(state.world.nation_get_recruitable_regiments(nation_id) + state.world.nation_get_active_regiments(nation_id)));
			text::localised_format_box(state, contents, box, std::string_view("topbar_army_tooltip"), sub);
			text::close_layout_box(contents, box);
		}
	}
};

class topbar_nation_navy_allocation_text : public nation_brigade_allocation_text {
	public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::curr, military::naval_supply_points_used(state, nation_id));
			text::localised_format_box(state, contents, box, std::string_view("topbar_ship_tooltip"), sub);
			text::add_line_break_to_layout_box(contents, state, box);
			text::add_line_break_to_layout_box(contents, state, box);
			text::add_to_substitution_map(sub, text::variable_type::tot,
					text::pretty_integer{military::naval_supply_points(state, nation_id)});
			text::add_to_substitution_map(sub, text::variable_type::req,
					text::pretty_integer{military::naval_supply_points_used(state, nation_id)});
			text::localised_format_box(state, contents, box, std::string_view("supply_load_status_desc_basic"), sub);
			text::add_line_break_to_layout_box(contents, state, box);
			text::localised_format_box(state, contents, box, std::string_view("supply_load_status_desc_detailed_list"), sub);
			text::close_layout_box(contents, box);
		}
	}
};

class topbar_nation_mobilization_size_text : public nation_mobilization_size_text {
	public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto issue_id = politics::get_issue_by_name(state, std::string_view("war_policy"));
			int32_t possible_sum = 0;
			int32_t raised_sum = 0;
			auto fat_id = dcon::fatten(state.world, nation_id);
			for(auto prov_own : fat_id.get_province_ownership_as_nation()) {
				auto prov = prov_own.get_province();
				possible_sum += military::mobilized_regiments_possible_from_province(state, prov.id);
			}
			for(auto prov_own : fat_id.get_province_ownership_as_nation()) {
				auto prov = prov_own.get_province();
				raised_sum += military::mobilized_regiments_created_from_province(state, prov.id);
			}
			auto box = text::open_layout_box(contents, 0);
			text::substitution_map sub2;
			text::add_to_substitution_map(sub2, text::variable_type::curr, raised_sum);
			// TODO - we (might) want to give the value the current war policy provides, though its more transparent perhaps to
			// just give the NV + Mob. Impact Modifier?
			text::add_to_substitution_map(sub2, text::variable_type::impact,
					text::fp_percentage{
							(state.world.nation_get_modifier_values(nation_id, sys::national_mod_offsets::mobilization_impact))});
			text::add_to_substitution_map(sub2, text::variable_type::policy, fat_id.get_issues(issue_id).get_name());
			text::add_to_substitution_map(sub2, text::variable_type::units, possible_sum);

			text::localised_single_sub_box(state, contents, box, std::string_view("topbar_mobilize_tooltip"), text::variable_type::curr,
					possible_sum);
			text::add_line_break_to_layout_box(contents, state, box);
			text::localised_format_box(state, contents, box, std::string_view("mobilization_impact_limit_desc"), sub2);
			text::add_line_break_to_layout_box(contents, state, box);
			text::localised_format_box(state, contents, box, std::string_view("mobilization_impact_limit_desc2"), sub2);

			text::close_layout_box(contents, box);
		}
	}
};

class topbar_nation_leadership_points_text : public nation_leadership_points_text {
	private:
	float getResearchPointsFromPop(sys::state& state, dcon::pop_type_id pop, dcon::nation_id n) {
		auto fat_nation = dcon::fatten(state.world, n);
		auto fat_pop = dcon::fatten(state.world, pop);
		/*
		Now imagine that Rock Hudson is standing at the top of the water slide hurling Nintendo consoles down the water slide.
		If it weren't for the ladders, which allow the water to pass through but not the Nintendo consoles,
		the Nintendo consoles could hit someone in the wave pool on the head, in which case the water park could get sued.
		*/
		// auto sum =  (fat_pop.get_research_points() * ((state.world.nation_get_demographics(n, demographics::to_key(state,
		// fat_pop)) / state.world.nation_get_demographics(n, demographics::total)) / fat_pop.get_research_optimum() ));
		auto sum = ((state.world.nation_get_demographics(n, demographics::to_key(state, pop)) /
										state.world.nation_get_demographics(n, demographics::total)) /
								state.world.pop_type_get_research_optimum(state.culture_definitions.officers));

		return sum;
	}

	public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::substitution_map sub;
			// text::add_to_substitution_map(sub, text::variable_type::curr, text::fp_one_place{nations::leadership_points(state,
			// nation_id)});
			text::add_to_substitution_map(sub, text::variable_type::poptype,
					state.world.pop_type_get_name(state.culture_definitions.officers));
			text::add_to_substitution_map(sub, text::variable_type::value,
					text::fp_two_places{getResearchPointsFromPop(state, state.culture_definitions.officers, nation_id)});
			text::add_to_substitution_map(sub, text::variable_type::fraction,
					text::fp_two_places{
							(state.world.nation_get_demographics(nation_id, demographics::to_key(state, state.culture_definitions.officers)) /
									state.world.nation_get_demographics(nation_id, demographics::total)) *
							100});
			text::add_to_substitution_map(sub, text::variable_type::optimal,
					text::fp_two_places{(state.world.pop_type_get_research_optimum(state.culture_definitions.officers) * 100)});
			text::localised_format_box(state, contents, box, std::string_view("tech_daily_leadership_tooltip"),
					sub); // Hey, dont fucking change CSV values into completely invalid
								// ones, the previously commented out CSV key was commented out
								// for good reason
			text::close_layout_box(contents, box);

			active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::leadership, false);
			active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::leadership_modifier, false);
		}
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

	bool is_active(sys::state& state) noexcept override {
		return state.ui_state.topbar_subwindow == topbar_subwindow && state.ui_state.topbar_subwindow->is_visible();
	}

	element_base* topbar_subwindow = nullptr;
};

class topbar_population_view_button : public topbar_tab_button {
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
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::date_to_string(state, state.current_date));
	}

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

	void on_update(sys::state& state) noexcept override {
		disabled = state.internally_paused;
	}
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

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

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

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

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
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return int32_t(!(nations::is_great_power(state, nation_id) &&
										 state.world.nation_get_rank(nation_id) > uint16_t(state.defines.great_nations_count)));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			if(!nations::is_great_power(state, nation_id)) {
				text::localised_format_box(state, contents, box, std::string_view("countryalert_no_gpstatus"), text::substitution_map{});
			} else if(state.world.nation_get_rank(nation_id) > uint16_t(state.defines.great_nations_count)) {
				text::localised_format_box(state, contents, box, std::string_view("countryalert_loosinggpstatus"),
						text::substitution_map{});
			} else if(state.world.nation_get_rank(nation_id) <= uint16_t(state.defines.great_nations_count)) {
				text::localised_format_box(state, contents, box, std::string_view("countryalert_no_loosinggpstatus"),
						text::substitution_map{});
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
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return int32_t(!economy::nation_is_constructing_factories(state, nation_id));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

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
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return int32_t(!economy::nation_has_closed_factories(state, nation_id));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

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
								text::add_to_layout_box(contents, state, box, factloc.get_factory().get_building_type().get_name(),
										text::text_color::yellow);
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
		return state.world.nation_get_demographics(nation_id, total_key) -
					 state.world.nation_get_demographics(nation_id, employment_key);
	}

	public:
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto primary_unemployed = get_num_unemployed(state, state.culture_definitions.primary_factory_worker, nation_id);
		auto secondary_unemployed = get_num_unemployed(state, state.culture_definitions.secondary_factory_worker, nation_id);
		return int32_t(primary_unemployed + secondary_unemployed <= 1.0f);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

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
						if(get_num_unemployed(state, state.culture_definitions.primary_factory_worker, nation_id) >= 10.0f &&
								state_instance.get_demographics(demographics::to_key(state, state.culture_definitions.primary_factory_worker)) >
										10.0f) {
							text::add_line_break_to_layout_box(contents, state, box);
							text::substitution_map sub;

							auto popFat = dcon::fatten(state.world, state.culture_definitions.primary_factory_worker);
							auto numUnemployed = int32_t(
									state_instance.get_demographics(demographics::to_key(state, state.culture_definitions.primary_factory_worker)) -
									state_instance.get_demographics(
											demographics::to_employment_key(state, state.culture_definitions.primary_factory_worker)));
							auto numWorkers = int32_t(state_instance.get_demographics(
									demographics::to_employment_key(state, state.culture_definitions.primary_factory_worker)));

							auto percUnemployed = text::fp_two_places{
									numUnemployed /
									state_instance.get_demographics(demographics::to_key(state, state.culture_definitions.primary_factory_worker))};

							// auto pop_fat = dcon::fatten(state.world, state.culture_definitions.primary_factory_worker);
							// auto pop_name = text::produce_simple_string(state, pop_fat.get_name());
							// auto numUnemployed = int32_t(state_instance.get_demographics(demographics::total) -
							// state_instance.get_demographics(demographics::employed)); auto stateDef =
							// state_instance.get_definition(); auto stateName = text::produce_simple_string(state,
							// stateDef.get_name()); auto percUnemployed = text::format_percentage(numUnemployed /
							// state_instance.get_demographics(demographics::total));
							text::add_to_substitution_map(sub, text::variable_type::num, numUnemployed);
							text::add_to_substitution_map(sub, text::variable_type::type, popFat.get_name());
							text::add_to_substitution_map(sub, text::variable_type::state, state_instance.get_definition().get_name());
							text::add_to_substitution_map(sub, text::variable_type::perc, percUnemployed);
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
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return int32_t(!nations::has_reform_available(state, nation_id));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			if(!nations::has_reform_available(state, nation_id)) {
				text::localised_format_box(state, contents, box, std::string_view("countryalert_no_candoreforms"),
						text::substitution_map{});
			} else if(nations::has_reform_available(state, nation_id)) {
				text::localised_format_box(state, contents, box, std::string_view("countryalert_candoreforms"), text::substitution_map{});
				text::add_divider_to_layout_box(state, contents, box);
				// Display Avaliable Reforms
				// Mostly a copy of nations::has_reform_avaliable
				auto last_date = state.world.nation_get_last_issue_or_reform_change(nation_id);
				if(bool(last_date) && (last_date + int32_t(state.defines.min_delay_between_reforms * 30.0f)) > state.current_date) {
					text::close_layout_box(contents, box);
					return;
				}
				if(state.world.nation_get_is_civilized(nation_id)) {
					for(auto i : state.culture_definitions.political_issues) {
						auto current = state.world.nation_get_issues(nation_id, i);
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
						auto current = state.world.nation_get_issues(nation_id, i);
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
					auto stored_rp = state.world.nation_get_research_points(nation_id);
					for(auto i : state.culture_definitions.military_issues) {
						auto current = state.world.nation_get_reforms(nation_id, i);
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
						auto current = state.world.nation_get_reforms(nation_id, i);
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
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return int32_t(!nations::has_decision_available(state, nation_id));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			if(!nations::has_decision_available(state, nation_id)) {
				text::localised_format_box(state, contents, box, std::string_view("countryalert_no_candodecisions"),
						text::substitution_map{});
			} else if(nations::has_decision_available(state, nation_id)) {
				text::localised_format_box(state, contents, box, std::string_view("countryalert_candodecisions"),
						text::substitution_map{});
				// Display Avaliable Decisions
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
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override {
		return int32_t(!politics::is_election_ongoing(state, nation_id));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			if(politics::has_elections(state, nation_id)) {
				if(!politics::is_election_ongoing(state, nation_id)) {
					text::localised_format_box(state, contents, box, std::string_view("countryalert_no_isinelection"),
							text::substitution_map{});
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

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			auto rebellions_iter = state.world.nation_get_rebellion_within(nation_id);
			if(rebellions_iter.begin() == rebellions_iter.end()) {
				text::localised_format_box(state, contents, box, std::string_view("countryalert_no_haverebels"),
						text::substitution_map{});
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

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

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

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

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

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

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
					text::localised_format_box(state, contents, box, std::string_view("remove_countryalert_canincreaseopinion"),
							text::substitution_map{});
				} else if(rebel::sphere_member_has_ongoing_revolt(state, nation_id)) {
					text::add_to_layout_box(contents, state, box,
							std::string_view("FIXME: gui/gui_topbar.hpp:404")); // TODO - if a sphere member is having a revolt then
																																	// we might have to display text -breizh
				} else {
					text::localised_format_box(state, contents, box, std::string_view("remove_countryalert_no_canincreaseopinion"),
							text::substitution_map{});
				}
			}
			text::close_layout_box(contents, box);
		}
	}
};

class topbar_nation_daily_research_points_text : public nation_daily_research_points_text {
	public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);
			auto fat = dcon::fatten(state.world, nation_id);

			auto tech_id = nations::current_research(state, nation_id);

			auto box = text::open_layout_box(contents, 0);
			bool clergy_passed = false; // Ugly fix but it should work -breizh
			if(ceil(state.world.nation_get_demographics(nation_id, demographics::to_key(state, state.culture_definitions.clergy))) >
					1.0f) {
				text::substitution_map sub1;
				clergy_passed = true;
				text::add_to_substitution_map(sub1, text::variable_type::poptype,
						state.world.pop_type_get_name(state.culture_definitions.clergy));
				text::add_to_substitution_map(sub1, text::variable_type::value,
						text::fp_two_places{get_research_points_from_pop(state, state.culture_definitions.clergy, nation_id)});
				text::add_to_substitution_map(sub1, text::variable_type::fraction,
						text::fp_two_places{
								(state.world.nation_get_demographics(nation_id, demographics::to_key(state, state.culture_definitions.clergy)) /
										state.world.nation_get_demographics(nation_id, demographics::total)) *
								100});
				text::add_to_substitution_map(sub1, text::variable_type::optimal,
						text::fp_two_places{(state.world.pop_type_get_research_optimum(state.culture_definitions.clergy) * 100)});
				text::localised_format_box(state, contents, box, std::string_view("tech_daily_researchpoints_tooltip"), sub1);
			}

			if(ceil(state.world.nation_get_demographics(nation_id,
						 demographics::to_key(state, state.culture_definitions.secondary_factory_worker))) > 1.0f) {
				text::substitution_map sub2;
				if(clergy_passed) {
					text::add_line_break_to_layout_box(contents, state, box);
				}
				text::add_to_substitution_map(sub2, text::variable_type::poptype,
						state.world.pop_type_get_name(state.culture_definitions.secondary_factory_worker));
				text::add_to_substitution_map(sub2, text::variable_type::value,
						text::fp_two_places{
								get_research_points_from_pop(state, state.culture_definitions.secondary_factory_worker, nation_id)});
				text::add_to_substitution_map(sub2, text::variable_type::fraction,
						text::fp_two_places{(state.world.nation_get_demographics(nation_id,
																		 demographics::to_key(state, state.culture_definitions.secondary_factory_worker)) /
																		state.world.nation_get_demographics(nation_id, demographics::total)) *
																100});
				text::add_to_substitution_map(sub2, text::variable_type::optimal,
						text::fp_two_places{
								(state.world.pop_type_get_research_optimum(state.culture_definitions.secondary_factory_worker) * 100)});
				text::localised_format_box(state, contents, box, std::string_view("tech_daily_researchpoints_tooltip"), sub2);
			}
			text::close_layout_box(contents, box);

			active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::research_points_modifier, false);
			active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::research_points, false);

			if(!bool(tech_id)) {
				auto box2 = text::open_layout_box(contents, 0);
				text::add_divider_to_layout_box(state, contents, box2);
				text::localised_single_sub_box(state, contents, box2, std::string_view("rp_accumulated"), text::variable_type::val,
						text::fp_one_place{fat.get_research_points()});

				text::close_layout_box(contents, box2);
			}
		}
	}
};

class topbar_commodity_xport_icon : public image_element_base {
	public:
	uint8_t slot = 0;
	dcon::commodity_id commodity_id{};
	float amount = 0.f;

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(contents, state, box,
					text::produce_simple_string(state, state.world.commodity_get_name(commodity_id)), text::text_color::white);
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
			auto btn = make_element_by_type<topbar_population_view_button>(state, id);
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
			return make_element_by_type<topbar_nation_name>(state, id);
		} else if(name == "player_flag") {
			return make_element_by_type<topbar_flag_button>(state, id);
		} else if(name == "country_prestige") {
			return make_element_by_type<topbar_nation_prestige_text>(state, id);
		} else if(name == "country_economic") {
			return make_element_by_type<topbar_nation_industry_score_text>(state, id);
		} else if(name == "country_military") {
			return make_element_by_type<topbar_nation_military_score_text>(state, id);
		} else if(name == "country_total") {
			return make_element_by_type<topbar_nation_total_score_text>(state, id);
		} else if(name == "country_colonial_power") {
			return make_element_by_type<topbar_nation_colonial_power_text>(state, id);
		} else if(name == "selected_prestige_rank") {
			return make_element_by_type<topbar_nation_prestige_rank_text>(state, id);
		} else if(name == "selected_industry_rank") {
			return make_element_by_type<topbar_nation_industry_rank_text>(state, id);
		} else if(name == "selected_military_rank") {
			return make_element_by_type<topbar_nation_military_rank_text>(state, id);
		} else if(name == "nation_totalrank") {
			return make_element_by_type<topbar_nation_rank_text>(state, id);
		} else if(name == "topbar_flag_overlay") {
			return make_element_by_type<topbar_nation_flag_frame>(state, id);
		} else if(name == "alert_building_factories") {
			return make_element_by_type<topbar_building_factories_icon>(state, id);
		} else if(name == "alert_closed_factories") {
			return make_element_by_type<topbar_closed_factories_icon>(state, id);
		} else if(name == "alert_unemployed_workers") {
			return make_element_by_type<topbar_unemployment_icon>(state, id);
		} else if(name == "budget_linechart") {
			return make_element_by_type<topbar_budget_line_graph>(state, id);
		} else if(name == "budget_funds") {
			return make_element_by_type<topbar_nation_budget_funds_text>(state, id);
		} else if(name == "topbar_tech_progress") {
			return make_element_by_type<nation_technology_research_progress>(state, id);
		} else if(name == "tech_current_research") {
			return make_element_by_type<topbar_nation_current_research_text>(state, id);
		} else if(name == "topbar_researchpoints_value") {
			return make_element_by_type<topbar_nation_daily_research_points_text>(state, id);
		} else if(name == "tech_literacy_value") {
			return make_element_by_type<topbar_nation_literacy_text>(state, id);
		} else if(name == "politics_party_icon") {
			return make_element_by_type<topbar_nation_ruling_party_ideology_plupp>(state, id);
		} else if(name == "politics_ruling_party") {
			return make_element_by_type<topbar_nation_ruling_party_text>(state, id);
		} else if(name == "politics_supressionpoints_value") {
			return make_element_by_type<topbar_nation_suppression_points_text>(state, id);
		} else if(name == "politics_infamy_value") {
			return make_element_by_type<topbar_nation_infamy_text>(state, id);
		} else if(name == "alert_can_do_reforms") {
			return make_element_by_type<topbar_available_reforms_icon>(state, id);
		} else if(name == "alert_can_do_decisions") {
			return make_element_by_type<topbar_available_decisions_icon>(state, id);
		} else if(name == "alert_is_in_election") {
			return make_element_by_type<topbar_ongoing_election_icon>(state, id);
		} else if(name == "alert_have_rebels") {
			return make_element_by_type<topbar_rebels_icon>(state, id);
		} else if(name == "population_total_value") {
			return make_element_by_type<topbar_nation_population_text>(state, id); // TODO: display daily change alongside total
		} else if(name == "topbar_focus_value") {
			return make_element_by_type<topbar_nation_focus_allocation_text>(state, id);
		} else if(name == "population_avg_mil_value") {
			return make_element_by_type<topbar_nation_militancy_text>(state, id);
		} else if(name == "population_avg_con_value") {
			return make_element_by_type<topbar_nation_consciousness_text>(state, id);
		} else if(name == "diplomacy_status") {
			return make_element_by_type<topbar_at_peace_text>(state, id);
		} else if(name == "diplomacy_at_war") {
			auto ptr = make_element_by_type<topbar_overlapping_enemy_flags>(state, id);
			ptr->base_data.position.y -= ptr->base_data.position.y / 4;
			return ptr;
		} else if(name == "diplomacy_diplopoints_value") {
			return make_element_by_type<topbar_nation_diplomatic_points_text>(state, id);
		} else if(name == "alert_colony") {
			return make_element_by_type<topbar_colony_icon>(state, id);
		} else if(name == "alert_crisis") {
			return make_element_by_type<topbar_crisis_icon>(state, id);
		} else if(name == "alert_can_increase_opinion") {
			return make_element_by_type<topbar_sphere_icon>(state, id);
		} else if(name == "alert_loosing_gp") {
			return make_element_by_type<topbar_losing_gp_status_icon>(state, id);
		} else if(name == "military_army_value") {
			return make_element_by_type<topbar_nation_brigade_allocation_text>(state, id);
		} else if(name == "military_navy_value") {
			return make_element_by_type<topbar_nation_navy_allocation_text>(state, id);
		} else if(name == "military_manpower_value") {
			return make_element_by_type<topbar_nation_mobilization_size_text>(state, id);
		} else if(name == "military_leadership_value") {
			return make_element_by_type<topbar_nation_leadership_points_text>(state, id);
		} else if(name == "topbar_outlinerbutton") {
			// Fake button isn't used - we create it manually instead...
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name.substr(0, 13) == "topbar_import") {
			auto ptr = make_element_by_type<topbar_commodity_xport_icon>(state, id);
			std::string var = std::string{name.substr(13)};
			var.empty() ? ptr->slot = uint8_t(0) : ptr->slot = uint8_t(std::stoi(var));
			import_icons.push_back(ptr.get());
			return ptr;
		} else if(name.substr(0, 13) == "topbar_export") {
			auto ptr = make_element_by_type<topbar_commodity_xport_icon>(state, id);
			std::string var = std::string{name.substr(13)};
			var.empty() ? ptr->slot = uint8_t(0) : ptr->slot = uint8_t(std::stoi(var));
			export_icons.push_back(ptr.get());
			return ptr;
		} else if(name.substr(0, 15) == "topbar_produced") {
			auto ptr = make_element_by_type<topbar_commodity_xport_icon>(state, id);
			std::string var = std::string{name.substr(15)};
			var.empty() ? ptr->slot = uint8_t(0) : ptr->slot = uint8_t(std::stoi(var));
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
				if(sys::commodity_group(state.world.commodity_get_commodity_group(cid)) != sys::commodity_group::military_goods &&
						sys::commodity_group(state.world.commodity_get_commodity_group(cid)) != sys::commodity_group::raw_material_goods &&
						sys::commodity_group(state.world.commodity_get_commodity_group(cid)) != sys::commodity_group::industrial_goods &&
						sys::commodity_group(state.world.commodity_get_commodity_group(cid)) != sys::commodity_group::consumer_goods)
					return;
				float produced = state.world.nation_get_domestic_market_pool(state.local_player_nation, cid);
				float consumed = state.world.nation_get_real_demand(state.local_player_nation, cid) *
												 state.world.nation_get_demand_satisfaction(state.local_player_nation, cid);
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
				if(sys::commodity_group(state.world.commodity_get_commodity_group(cid)) != sys::commodity_group::military_goods &&
						sys::commodity_group(state.world.commodity_get_commodity_group(cid)) != sys::commodity_group::raw_material_goods &&
						sys::commodity_group(state.world.commodity_get_commodity_group(cid)) != sys::commodity_group::industrial_goods &&
						sys::commodity_group(state.world.commodity_get_commodity_group(cid)) != sys::commodity_group::consumer_goods)
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
