#pragma once

#include "demographics.hpp"
#include "gui_element_types.hpp"
#include "gui_production_window.hpp"
#include "gui_diplomacy_window.hpp"
#include "gui_technology_window.hpp"
#include "gui_politics_window.hpp"
#include "gui_population_window.hpp"
#include "gui_military_window.hpp"
#include "gui_chat_window.hpp"
#include "gui_common_elements.hpp"
#include "gui_diplomacy_request_topbar.hpp"
#include "nations.hpp"
#include "politics.hpp"
#include "rebels.hpp"
#include "system_state.hpp"
#include "text.hpp"
#include "gui_event.hpp"

#include "gui_armygroups.hpp"

namespace ui {

class topbar_nation_name : public generic_name_text<dcon::nation_id> {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(state.cheat_data.show_province_id_tooltip) {
			auto box = text::open_layout_box(contents);
			text::add_to_layout_box(state, contents, box, std::string_view("Nation ID:"));
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, std::to_string(state.local_player_nation.value));
			text::close_layout_box(contents, box);
		}
	}
};

class topbar_nation_prestige_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, std::to_string(int32_t(nations::prestige_score(state, retrieve<dcon::nation_id>(state, parent)))));
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("rank_prestige"), text::substitution_map{});
		text::add_line_break_to_layout_box(state, contents, box);
		switch(nations::get_status(state, nation_id)) {
		case(nations::status::great_power):
			text::localised_format_box(state, contents, box, std::string_view("diplomacy_greatnation_status"), text::substitution_map{});
			break;
		case(nations::status::secondary_power):
			text::localised_format_box(state, contents, box, std::string_view("diplomacy_colonialnation_status"), text::substitution_map{});
			break;
		case(nations::status::civilized):
			text::localised_format_box(state, contents, box, std::string_view("diplomacy_civilizednation_status"), text::substitution_map{});
			break;
		case(nations::status::westernizing):
			text::localised_format_box(state, contents, box, std::string_view("diplomacy_almost_western_nation_status"), text::substitution_map{});
			break;
		case(nations::status::uncivilized):
			text::localised_format_box(state, contents, box, std::string_view("diplomacy_uncivilizednation_status"), text::substitution_map{});
			break;
		case(nations::status::primitive):
			text::localised_format_box(state, contents, box, std::string_view("diplomacy_primitivenation_status"), text::substitution_map{});
			break;
		default:
			break;
		};
		text::close_layout_box(contents, box);
		text::add_line_break_to_layout(state, contents);
		active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::prestige, true);
		text::add_line_break_to_layout(state, contents);
		active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::permanent_prestige, true);
	}
};

class topbar_nation_industry_score_text : public nation_industry_score_text {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("rank_industry"), text::substitution_map{});
		text::add_divider_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("rank_industry_d"), text::substitution_map{});
		text::close_layout_box(contents, box);
	}
};

class topbar_nation_military_score_text : public nation_military_score_text {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("rank_military"), text::substitution_map{});
		text::add_divider_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("rank_military_d"), text::substitution_map{});
		text::close_layout_box(contents, box);
	}
};

class topbar_nation_colonial_power_text : public nation_colonial_power_text {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);

		text::substitution_map sub;
		std::string value;
		value = text::prettify(nations::free_colonial_points(state, nation_id));
		text::add_to_substitution_map(sub, text::variable_type::value, std::string_view(value));
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("colonial_points"), sub);
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("explain_colonial_points"), sub);
		text::add_divider_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("available_colonial_power"), sub);
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("colonial_points_from_technology"), sub);
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::format_float(nations::colonial_points_from_technology(state, nation_id), 0), text::text_color::green);
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("colonial_points_from_naval_bases"), sub);
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::format_float(nations::colonial_points_from_naval_bases(state, nation_id), 0), text::text_color::green);
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("colonial_points_from_ships"), sub);
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::format_float(nations::colonial_points_from_ships(state, nation_id), 0), text::text_color::green);
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("used_colonial_maintenance"), sub);
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::format_float(nations::used_colonial_points(state, nation_id), 0), text::text_color::red);
		text::close_layout_box(contents, box);
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

		if(auto* cache = state.find_player_data_cache(state.local_player_nation)) {
			for(size_t i = 0; i < (*cache).treasury_record.size(); ++i)
				datapoints[i] = (*cache).treasury_record[(state.ui_date.value + 1 + i) % 32] - (*cache).treasury_record[(state.ui_date.value + 0 + i) % 32];
			datapoints[datapoints.size() - 1] = (*cache).treasury_record[(state.ui_date.value + 1 + 31) % 32] - (*cache).treasury_record[(state.ui_date.value + 0 + 31) % 32];
			datapoints[0] = datapoints[1]; // otherwise you will store the difference between two non-consecutive days here
			set_data_points(state, datapoints);
		}
	}
};

class topbar_nation_literacy_text : public expanded_hitbox_text {
public:
	void on_create(sys::state& state) noexcept override {
		top_left_extension = ui::xy_pair{ 25, 3 };
		bottom_right_extension = ui::xy_pair{ -10, 2 };
		expanded_hitbox_text::on_create(state);
	}

	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		auto literacy = nations::get_avg_non_colonial_literacy(state, n);
		set_text(state, text::format_percentage(literacy, 1));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);

		auto box = text::open_layout_box(contents, 0);
		text::substitution_map sub;
		auto literacy_change = demographics::get_estimated_literacy_change(state, nation_id);
		text::add_to_substitution_map(sub, text::variable_type::val, text::fp_percentage_two_places{literacy_change});
		auto avg_non_colonial_literacy = nations::get_avg_non_colonial_literacy(state, nation_id);
		text::add_to_substitution_map(sub, text::variable_type::x, text::fp_percentage_one_place{avg_non_colonial_literacy });

		auto avg_literacy = nations::get_avg_total_literacy(state, nation_id);
		text::add_to_substitution_map(sub, text::variable_type::avg, text::fp_percentage_one_place{ avg_literacy });

		text::localised_format_box(state, contents, box, std::string_view("alice_topbar_avg_literacy_in_states"), sub);
		text::add_line_break_to_layout_box(state, contents, box);

		text::localised_format_box(state, contents, box, std::string_view("topbar_avg_literacy"), sub);
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("topbar_avg_change"), sub);
		text::close_layout_box(contents, box);
	}
};

class topbar_nation_infamy_text : public expanded_hitbox_text {
public:
	void on_create(sys::state& state) noexcept override {
		top_left_extension = ui::xy_pair{ 25, 3 };
		bottom_right_extension = ui::xy_pair{ 0, 2 };
		expanded_hitbox_text::on_create(state);
	}
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::format_float(state.world.nation_get_infamy(retrieve<dcon::nation_id>(state, parent)), 2));
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		auto box = text::open_layout_box(contents, 0);
		auto fat_id = dcon::fatten(state.world, nation_id);
		text::localised_format_box(state, contents, box, "infamy");
		text::add_to_layout_box(state, contents, box, std::string_view(":"));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::fp_two_places{fat_id.get_infamy()});
		text::add_to_layout_box(state, contents, box, std::string_view("/"));
		text::add_to_layout_box(state, contents, box, text::fp_two_places{state.defines.badboy_limit});
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, "badboy_dro_1");
		text::close_layout_box(contents, box);
		active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::badboy, false);
	}
};

class topbar_nation_population_text : public multiline_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		auto total_pop = state.world.nation_get_demographics(n, demographics::total);

		if(auto* cache = state.find_player_data_cache(state.local_player_nation)) {
			auto pop_amount = (*cache).population_record[state.ui_date.value % 32];
			auto pop_change = state.ui_date.value <= 32
				? (state.ui_date.value <= 2 ? 0.0f : pop_amount - (*cache).population_record[2])
				: (pop_amount - (*cache).population_record[(state.ui_date.value - 30) % 32]);

			text::text_color color = pop_change < 0 ? text::text_color::red : text::text_color::green;
			if(pop_change == 0)
				color = text::text_color::white;

			auto layout = text::create_endless_layout(state, internal_layout,
			text::layout_parameters{ 0, 0, int16_t(base_data.size.x), int16_t(base_data.size.y), base_data.data.text.font_handle, 0, text::alignment::left, text::text_color::black, false });
			auto box = text::open_layout_box(layout, 0);
			text::add_to_layout_box(state, layout, box, text::prettify(int32_t(total_pop)));
			text::add_to_layout_box(state, layout, box, std::string(" ("));
			if(pop_change > 0) {
				text::add_to_layout_box(state, layout, box, std::string("+"), text::text_color::green);
			}
			text::add_to_layout_box(state, layout, box, text::pretty_integer{ int64_t(pop_change) }, color);
			text::add_to_layout_box(state, layout, box, std::string(")"));
			text::close_layout_box(layout, box);
		}
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {

		auto nation_id = retrieve<dcon::nation_id>(state, parent);

		if(auto* cache = state.find_player_data_cache(state.local_player_nation)) {
			auto pop_amount = (*cache).population_record[state.ui_date.value % 32];
			auto pop_change = state.ui_date.value <= 30 ? 0.0f : (pop_amount - (*cache).population_record[(state.ui_date.value - 30) % 32]);

			text::add_line(state, contents, "pop_growth_topbar_3", text::variable_type::curr, text::pretty_integer{ int64_t(state.world.nation_get_demographics(nation_id, demographics::total)) });
			text::add_line(state, contents, "pop_growth_topbar_2", text::variable_type::x, text::pretty_integer{ int64_t(pop_change) });
			text::add_line(state, contents, "pop_growth_topbar", text::variable_type::x, text::pretty_integer{ int64_t(nations::get_monthly_pop_increase_of_nation(state, nation_id)) });
			text::add_line(state, contents, "pop_growth_topbar_4", text::variable_type::val, text::pretty_integer{ int64_t(state.world.nation_get_demographics(nation_id, demographics::total) * 4) });

			text::add_line_break_to_layout(state, contents);
		}

		active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::pop_growth, true);
	}

};

class topbar_treasury_text : public multiline_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);

		auto layout = text::create_endless_layout(state, internal_layout,
		text::layout_parameters{ 0, 0, int16_t(base_data.size.x), int16_t(base_data.size.y), base_data.data.text.font_handle, 0, text::alignment::center, text::text_color::black, false });
		auto box = text::open_layout_box(layout, 0);

		if(auto* cache = state.find_player_data_cache(state.local_player_nation)) {
			auto current_day_record = (*cache).treasury_record[state.ui_date.value % 32];
			auto previous_day_record = (*cache).treasury_record[(state.ui_date.value + 31) % 32];
			auto change = current_day_record - previous_day_record;

			text::add_to_layout_box(state, layout, box, text::prettify_currency(nations::get_treasury(state, n)));
			text::add_to_layout_box(state, layout, box, std::string(" ("));
			if(change > 0) {
				text::add_to_layout_box(state, layout, box, std::string("+"), text::text_color::green);
				text::add_to_layout_box(state, layout, box, text::prettify_currency(change), text::text_color::green);
			} else if(change == 0) {
				text::add_to_layout_box(state, layout, box, text::prettify_currency(change), text::text_color::white);
			} else {
				text::add_to_layout_box(state, layout, box, text::prettify_currency(change), text::text_color::red);
			}
			text::add_to_layout_box(state, layout, box, std::string(")"));
		}
		text::close_layout_box(layout, box);

	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		/*
		// SCHOMBERT: A good portion of this is wrong because it is showing maximum values for some of these expense categories
		// rather than my scaling them to what the actual spending settings are

		auto nation_id = retrieve<dcon::nation_id>(state, parent);

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
		total_expense += economy::interest_payment(state, nation_id);
		total_expense += economy::estimate_subsidy_spending(state, nation_id);

		text::add_to_substitution_map(sub, text::variable_type::yesterday,
				text::fp_one_place{ nations::get_yesterday_income(state, nation_id) });
		text::add_to_substitution_map(sub, text::variable_type::cash, text::fp_one_place{ nations::get_treasury(state, nation_id) });

		{
			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, std::string_view("topbar_funds"), sub); // $YESTERDAY && $CASH
			text::close_layout_box(contents, box);
		}
		text::add_line_break_to_layout(state, contents);

		text::add_line(state, contents, std::string_view("budget_total_income"), text::variable_type::val, text::fp_three_places{ total_income }); // $VAL
		text::add_line(state, contents, std::string_view("taxes_poor"), text::variable_type::val, text::fp_one_place{ economy::estimate_tax_income_by_strata(state, nation_id, culture::pop_strata::poor) }); // $VAL
		text::add_line(state, contents, std::string_view("taxes_middle"), text::variable_type::val,
					text::fp_one_place{ economy::estimate_tax_income_by_strata(state, nation_id, culture::pop_strata::middle) }); // $VAL
		text::add_line(state, contents, std::string_view("taxes_rich"), text::variable_type::val,
				text::fp_one_place{ economy::estimate_tax_income_by_strata(state, nation_id, culture::pop_strata::rich) }); // $VAL
		text::add_line(state, contents, std::string_view("tariffs_income"), text::variable_type::val,
					text::fp_one_place{ economy::estimate_tariff_income(state, nation_id) }); // $VAL
		text::add_line(state, contents, std::string_view("budget_exports")); // $VAL	TODO
		text::add_line(state, contents, std::string_view("budget_gold"), text::variable_type::val, text::fp_one_place{ economy::estimate_gold_income(state, nation_id) }); // $VAL
		text::add_line(state, contents, std::string_view("budget_total_expense"), text::variable_type::val,
					text::fp_three_places{ -total_expense }); // $VAL TODO
		text::add_line(state, contents, std::string_view("budget_expense_slider_education"),
					text::variable_type::val,
					text::fp_three_places{
							-economy::estimate_pop_payouts_by_income_type(state, nation_id, culture::income_type::education) }); // $VAL
		text::add_line(state, contents, std::string_view("budget_slider_administration"),
					text::variable_type::val,
					text::fp_three_places{
							-economy::estimate_pop_payouts_by_income_type(state, nation_id, culture::income_type::administration) }); // $VAL
		text::add_line(state, contents, std::string_view("budget_slider_social_spending"),
					text::variable_type::val,
					text::fp_three_places{
							-economy::estimate_social_spending(state, nation_id) }); // $VAL - presumably loan payments == interest (?)
		text::add_line(state, contents, std::string_view("budget_slider_military_spending"),
				text::variable_type::val,
				text::fp_two_places{
						-economy::estimate_pop_payouts_by_income_type(state, nation_id, culture::income_type::military) }); // $VAL
		text::add_line(state, contents, std::string_view("budget_interest"), text::variable_type::val,
				text::fp_one_place{
						-economy::interest_payment(state, nation_id) }); // $VAL - presumably loan payments == interest (?)
		text::add_line(state, contents, std::string_view("budget_imports"), text::variable_type::val,
				text::fp_one_place{ -economy::nation_total_imports(state,
						nation_id) }); // $VAL - presumably nation_total_imports is for national stockpile (?)

		text::add_line_break_to_layout(state, contents);

		text::add_line(state, contents, std::string_view("topbar_projected_income"), text::variable_type::val,
				text::fp_two_places{ economy::estimate_daily_income(state, nation_id) });

		*/

		{
			text::substitution_map sub{};
			text::add_to_substitution_map(sub, text::variable_type::x, text::fp_currency{ state.world.nation_get_private_investment(state.local_player_nation) });
			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, "investment_pool", sub);
			text::close_layout_box(contents, box);
		}
		{
			text::substitution_map sub{};
			text::add_to_substitution_map(sub, text::variable_type::x, text::fp_currency{ state.ui_state.last_tick_investment_pool_change });
			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, "investment_pool_income_1", sub);
			text::close_layout_box(contents, box);
		}
		{
			auto capitalists_investment_ratio = state.defines.alice_invest_capitalist + state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::capitalist_reinvestment);
			auto middle_class_investment_ratio = state.defines.alice_invest_middle_class + state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::middle_class_reinvestment);
			auto farmers_investment_ratio = state.defines.alice_invest_farmer + state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::farmers_reinvestment);
			auto landowners_investment_ratio = state.defines.alice_invest_aristocrat + state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::aristocrat_reinvestment);

			if(capitalists_investment_ratio > 0.f) {
				text::add_line(state, contents, "capitalists_investment_ratio", text::variable_type::x, text::fp_percentage{ capitalists_investment_ratio }, 15);
			}
			if(landowners_investment_ratio > 0.f) {
				text::add_line(state, contents, "landowners_investment_ratio", text::variable_type::x, text::fp_percentage{ landowners_investment_ratio }, 15);
			}
			if(middle_class_investment_ratio > 0.f) {
				text::add_line(state, contents, "middle_class_investment_ratio", text::variable_type::x, text::fp_percentage{ middle_class_investment_ratio }, 15);
			}
			if(farmers_investment_ratio > 0.f) {
				text::add_line(state, contents, "farmers_investment_ratio", text::variable_type::x, text::fp_percentage{ farmers_investment_ratio }, 15);
			}
		}
		{
			auto domestic_investment = economy::estimate_current_domestic_investment(state, state.local_player_nation);

			if(domestic_investment > 0.f) {
				text::substitution_map sub{};
				text::add_to_substitution_map(sub, text::variable_type::x, text::fp_currency{ domestic_investment });
				text::add_to_substitution_map(sub, text::variable_type::country, state.local_player_nation);
				auto box = text::open_layout_box(contents, 0);
				text::localised_format_box(state, contents, box, "investment_pool_income_3", sub);
				text::close_layout_box(contents, box);
			}
		}
		{
			for(auto n : state.world.in_nation) {
				auto rel = state.world.nation_get_overlord_as_subject(n);
				auto overlord = state.world.overlord_get_ruler(rel);

				if(overlord == state.local_player_nation) {
					bool temp = false;
					float est_private_const_spending = economy::estimate_private_construction_spendings(state, n);
					auto craved_constructions = economy::estimate_private_investment_construct(state, n, true, est_private_const_spending, temp);
					//auto upgrades = economy::estimate_private_investment_upgrade(state, n, est_private_const_spending);
					auto constructions = economy::estimate_private_investment_construct(state, n, false, est_private_const_spending, temp);
					auto province_constr = economy::estimate_private_investment_province(state, n, est_private_const_spending);

					if(economy::estimate_private_construction_spendings(state, n) < 1.0f /* && upgrades.size() == 0 */ && constructions.size() == 0 && province_constr.size() == 0) {
						auto amt = state.world.nation_get_private_investment(n) * state.defines.alice_privateinvestment_subject_transfer / 100.f;

						text::substitution_map sub{};
						text::add_to_substitution_map(sub, text::variable_type::x, text::fp_currency{ amt });
						text::add_to_substitution_map(sub, text::variable_type::country, n);
						auto box = text::open_layout_box(contents, 0);
						text::localised_format_box(state, contents, box, "investment_pool_income_2", sub);
						text::close_layout_box(contents, box);
					}
				}
			}
		}

		text::add_line_break_to_layout(state, contents);
		{
			text::substitution_map sub{};
			text::add_to_substitution_map(sub, text::variable_type::x, text::fp_currency{ economy::estimate_investment_pool_daily_loss(state, state.local_player_nation) });
			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, "investment_pool_spending_1", sub);
			text::close_layout_box(contents, box);
		}

		auto private_constr = economy::estimate_private_construction_spendings(state, state.local_player_nation);
		{
			text::substitution_map sub{};
			text::add_to_substitution_map(sub, text::variable_type::x, text::fp_currency{ private_constr });
			auto box = text::open_layout_box(contents, 15);
			text::localised_format_box(state, contents, box, "investment_pool_spending_2", sub);
			text::close_layout_box(contents, box);
		}
		{
			bool temp = false;
			float est_private_const_spending = economy::estimate_private_construction_spendings(state, state.local_player_nation);
			auto craved_constructions = economy::estimate_private_investment_construct(state, state.local_player_nation, true, est_private_const_spending, temp);
			//auto upgrades = economy::estimate_private_investment_upgrade(state, state.local_player_nation, est_private_const_spending);
			auto constructions = economy::estimate_private_investment_construct(state, state.local_player_nation, false, est_private_const_spending, temp);
			auto province_constr = economy::estimate_private_investment_province(state, state.local_player_nation, est_private_const_spending);

			if(private_constr < 1.f /* && upgrades.size() == 0 */ && constructions.size() == 0 && province_constr.size() == 0) {
				auto subjects = nations::nation_get_subjects(state, state.local_player_nation);
				auto amt = state.world.nation_get_private_investment(state.local_player_nation) * state.defines.alice_privateinvestment_subject_transfer / 100.f;

				if(subjects.size() > 0) {
					text::substitution_map sub{};
					text::add_to_substitution_map(sub, text::variable_type::x, text::fp_currency{ amt });
					auto box = text::open_layout_box(contents, 15);
					text::localised_format_box(state, contents, box, "investment_pool_spending_3", sub);
					text::close_layout_box(contents, box);
				}
			}
		}
	}

};

class topbar_nation_focus_allocation_text : public expanded_hitbox_text {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void on_create(sys::state& state) noexcept override {
		top_left_extension = ui::xy_pair{ 25, 3 };
		bottom_right_extension = ui::xy_pair{ 0, 2 };
		expanded_hitbox_text::on_create(state);
	}

	void on_update(sys::state& state) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		auto available = nations::max_national_focuses(state, nation_id);
		auto in_use = nations::national_focuses_in_use(state, nation_id);

		if(in_use < available) {
			set_text(state, "?R" + text::format_ratio(in_use, available));
		} else {
			set_text(state, text::format_ratio(in_use, available));
		}
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		auto box = text::open_layout_box(contents, 0);
		text::substitution_map sub1;
		float relevant_pop = state.world.nation_get_demographics(nation_id,
				demographics::to_key(state, state.world.nation_get_primary_culture(nation_id)));
		for(auto ac : state.world.in_culture) {
			if(state.world.nation_get_accepted_cultures(nation_id, ac)) {
				relevant_pop += state.world.nation_get_demographics(nation_id, demographics::to_key(state, ac));
			}
		}
		text::add_to_substitution_map(sub1, text::variable_type::num, text::pretty_integer{(int64_t)relevant_pop});
		auto fPoints = relevant_pop / state.defines.national_focus_divider; // NOTE: Occasionally inaccurate by a few 0.01, this
																																				// doesnt really matter so im leaving it -breizh
		text::add_to_substitution_map(sub1, text::variable_type::focus, text::fp_two_places{fPoints});
		text::localised_format_box(state, contents, box, std::string_view("tb_nationalfocus_culture"), sub1);
		text::add_line_break_to_layout_box(state, contents, box);
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
							text::add_to_layout_box(state, contents, box, natl_fat_id.get_name());
							text::add_line_break_to_layout_box(state, contents, box);
						}
					}
				}
			});
		} else {
			text::add_divider_to_layout_box(state, contents, box);
			text::localised_format_box(state, contents, box, std::string_view("tb_nationalfocus_none"), text::substitution_map{});
		}
		text::close_layout_box(contents, box);
		active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::max_national_focus, false);
	}
};

class topbar_nation_militancy_text : public expanded_hitbox_text {
public:
	void on_create(sys::state& state) noexcept override {
		top_left_extension = ui::xy_pair{ 25, 3 };
		bottom_right_extension = ui::xy_pair{ 0, 2 };
		expanded_hitbox_text::on_create(state);
	}

	void on_update(sys::state& state) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		auto militancy = state.world.nation_get_demographics(nation_id, demographics::militancy);
		auto total_pop = state.world.nation_get_demographics(nation_id, demographics::total);
		set_text(state, text::format_float(total_pop == 0.f ? 0.f : militancy / total_pop));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		auto box = text::open_layout_box(contents, 0);
		text::substitution_map sub;
		auto mil_change = demographics::get_estimated_mil_change(state, nation_id);
		auto total = state.world.nation_get_demographics(nation_id, demographics::total);
		text::add_to_substitution_map(sub, text::variable_type::avg,
				text::fp_two_places{total != 0.f ? state.world.nation_get_demographics(nation_id, demographics::militancy) / total : 0.f});
		text::add_to_substitution_map(sub, text::variable_type::val,
				text::fp_four_places{mil_change});
		text::localised_format_box(state, contents, box, std::string_view("topbar_avg_mil"), sub);
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("topbar_avg_change"), sub);
		text::close_layout_box(contents, box);

		active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::global_pop_militancy_modifier, true);
		active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::core_pop_militancy_modifier, true);
		active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::non_accepted_pop_militancy_modifier, true);
	}
};

class topbar_nation_consciousness_text : public expanded_hitbox_text {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void on_create(sys::state& state) noexcept override {
		top_left_extension = ui::xy_pair{ 25, 3 };
		bottom_right_extension = ui::xy_pair{ 0, 2 };
		expanded_hitbox_text::on_create(state);
	}

	void on_update(sys::state& state) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		auto militancy = state.world.nation_get_demographics(nation_id, demographics::consciousness);
		auto total_pop = state.world.nation_get_demographics(nation_id, demographics::total);
		set_text(state, text::format_float(total_pop == 0.f ? 0.f : militancy / total_pop));
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);

		auto box = text::open_layout_box(contents, 0);
		text::substitution_map sub;
		auto con_change = demographics::get_estimated_con_change(state, nation_id);
		auto total = state.world.nation_get_demographics(nation_id, demographics::total);
		text::add_to_substitution_map(sub, text::variable_type::avg,
				text::fp_two_places{ total != 0.f ? (state.world.nation_get_demographics(nation_id, demographics::consciousness) / total) : 0.f });
		text::add_to_substitution_map(sub, text::variable_type::val, text::fp_four_places{ con_change });
		text::localised_format_box(state, contents, box, std::string_view("topbar_avg_con"), sub);
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("topbar_avg_change"), sub);
		text::close_layout_box(contents, box);

		active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::global_pop_consciousness_modifier, true);
		active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::core_pop_consciousness_modifier, true);
		active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::non_accepted_pop_consciousness_modifier, true);
	}
};

class topbar_nation_diplomatic_points_text : public expanded_hitbox_text {
public:
	void on_create(sys::state& state) noexcept override {
		top_left_extension = ui::xy_pair{ 25, 3 };
		bottom_right_extension = ui::xy_pair{ 0, 2 };
		expanded_hitbox_text::on_create(state);
	}

	void on_update(sys::state& state) noexcept override {
		auto points = nations::diplomatic_points(state, retrieve<dcon::nation_id>(state, parent));
		set_text(state, text::format_float(points, 1));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);

		auto box = text::open_layout_box(contents, 0);
		text::substitution_map sub;
		text::add_to_substitution_map(sub, text::variable_type::curr,
			text::fp_one_place{ nations::diplomatic_points(state, nation_id) });

		// Monthly gain
		text::add_to_substitution_map(sub, text::variable_type::value,
			text::fp_one_place{ nations::monthly_diplomatic_points(state, nation_id) });

		text::substitution_map sub_base;
		// Base gain
		text::add_to_substitution_map(sub_base, text::variable_type::value,
			text::fp_one_place{ state.defines.base_monthly_diplopoints });

		text::localised_format_box(state, contents, box, std::string_view("topbar_diplopoints"), sub);
		text::add_divider_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("topbar_diplopoints_gain"), sub);
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("topbar_diplopoints_basegain"), sub_base);
		text::close_layout_box(contents, box);

		active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::diplomatic_points_modifier, false);
	}
};

class topbar_nation_brigade_allocation_text : public nation_brigade_allocation_text {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		auto box = text::open_layout_box(contents, 0);
		text::substitution_map sub;
		text::add_to_substitution_map(sub, text::variable_type::curr, state.world.nation_get_active_regiments(nation_id));
		text::add_to_substitution_map(sub, text::variable_type::max, state.world.nation_get_recruitable_regiments(nation_id));
		text::localised_format_box(state, contents, box, std::string_view("topbar_army_tooltip"), sub);
		text::close_layout_box(contents, box);
	}
};

class topbar_nation_navy_allocation_text : public nation_navy_allocation_text {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		{
			auto box = text::open_layout_box(contents, 0);
			text::substitution_map sub;
			int32_t num_ships = 0;
			for(auto nv : state.world.nation_get_navy_control(n)) {
				num_ships += int32_t(nv.get_navy().get_navy_membership().end() - nv.get_navy().get_navy_membership().begin());
			}
			text::add_to_substitution_map(sub, text::variable_type::curr, num_ships);
			text::add_to_substitution_map(sub, text::variable_type::tot, text::pretty_integer{ military::naval_supply_points(state, n) });
			text::add_to_substitution_map(sub, text::variable_type::req, text::pretty_integer{ military::naval_supply_points_used(state, n) });
			text::localised_format_box(state, contents, box, std::string_view("alice_navy_allocation_tt"), sub);
			text::close_layout_box(contents, box);
		}
		for(const auto nv : state.world.nation_get_navy_control_as_controller(n)) {
			int32_t total = 0;
			for(const auto memb : nv.get_navy().get_navy_membership()) {
				total += state.military_definitions.unit_base_definitions[memb.get_ship().get_type()].supply_consumption_score;
			}
			auto box = text::open_layout_box(contents, 0);
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::name, state.to_string_view(nv.get_navy().get_name()));
			text::add_to_substitution_map(sub, text::variable_type::value, total);
			text::add_to_substitution_map(sub, text::variable_type::perc, text::fp_percentage{ float(total) / military::naval_supply_points(state, n) });
			text::localised_format_box(state, contents, box, std::string_view("alice_navy_allocation_2"), sub);
			text::close_layout_box(contents, box);
		}
		active_modifiers_description(state, contents, n, 0, sys::national_mod_offsets::supply_range, true);
	}
};

class topbar_nation_mobilization_size_text : public nation_mobilization_size_text {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);

		auto issue_id = politics::get_issue_by_name(state, std::string_view("war_policy"));
		int32_t possible_sum = 0;
		int32_t raised_sum = 0;
		auto fat_id = dcon::fatten(state.world, n);
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
		text::add_to_substitution_map(sub2, text::variable_type::impact, text::fp_percentage{ (state.world.nation_get_modifier_values(n, sys::national_mod_offsets::mobilization_impact))});
		text::add_to_substitution_map(sub2, text::variable_type::policy, fat_id.get_issues(issue_id).get_name());
		text::add_to_substitution_map(sub2, text::variable_type::units, possible_sum);
		text::localised_single_sub_box(state, contents, box, std::string_view("topbar_mobilize_tooltip"), text::variable_type::curr, possible_sum);
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("mobilization_impact_limit_desc"), sub2);
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("mobilization_impact_limit_desc2"), sub2);
		text::close_layout_box(contents, box);

		active_modifiers_description(state, contents, n, 0, sys::national_mod_offsets::mobilization_impact, true);
		active_modifiers_description(state, contents, n, 0, sys::national_mod_offsets::mobilization_size, true);
	}
};

class topbar_nation_leadership_points_text : public nation_leadership_points_text {
private:
	float getResearchPointsFromPop(sys::state& state, dcon::pop_type_id pop, dcon::nation_id n) {
		return military::calculate_monthly_leadership_points(state, n);
	}

public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);

		auto box = text::open_layout_box(contents, 0);
		text::substitution_map sub;
		text::add_to_substitution_map(sub, text::variable_type::poptype,
			state.world.pop_type_get_name(state.culture_definitions.officers));
		text::add_to_substitution_map(sub, text::variable_type::value,
			text::fp_two_places{getResearchPointsFromPop(state, state.culture_definitions.officers, n)});
		text::add_to_substitution_map(sub, text::variable_type::fraction,
			text::fp_percentage{
				state.world.nation_get_demographics(n, demographics::to_key(state, state.culture_definitions.officers)) /
				state.world.nation_get_demographics(n, demographics::total) });
		text::add_to_substitution_map(sub, text::variable_type::optimal,
			text::fp_two_places{(state.world.pop_type_get_research_optimum(state.culture_definitions.officers) * 100)});
		text::localised_format_box(state, contents, box, std::string_view("alice_daily_leadership_tt"), sub);
		text::close_layout_box(contents, box);

		active_modifiers_description(state, contents, n, 0, sys::national_mod_offsets::leadership, true);
		active_modifiers_description(state, contents, n, 0, sys::national_mod_offsets::leadership_modifier, true);
	}
};

class background_image : public opaque_element_base {
public:
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		base_data.size.x = int16_t(ui_width(state));
		base_data.size.y = int16_t(ui_height(state));
		opaque_element_base::render(state, x, y);
		//Put it far away!
		if(base_data.position.x >= 256 || base_data.position.y >= 256) {
			base_data.position.x = int16_t(8192 * 2);
			base_data.position.y = int16_t(8192 * 2);
		}
	}
};

class topbar_tab_button : public checkbox_button {
public:
	void button_action(sys::state& state) noexcept override {
		auto const override_and_show_tab = [&]() {
			topbar_subwindow->set_visible(state, true);
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

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "alice_topbar_tab_1");
	}

	element_base* topbar_subwindow = nullptr;
};

class topbar_budget_tab_button : public topbar_tab_button {
public:
	sound::audio_instance& get_click_sound(sys::state& state) noexcept override {
		return sound::get_tab_budget_sound(state);
	}
};
class topbar_trade_tab_button : public topbar_tab_button {
public:
	sound::audio_instance& get_click_sound(sys::state& state) noexcept override {
		// for now we reuse the budget tab sound
		return sound::get_tab_budget_sound(state);
	}
};
class topbar_politics_tab_button : public topbar_tab_button {
public:
	sound::audio_instance& get_click_sound(sys::state& state) noexcept override {
		return sound::get_tab_politics_sound(state);
	}
};
class topbar_diplomacy_tab_button : public topbar_tab_button {
public:
	sound::audio_instance& get_click_sound(sys::state& state) noexcept override {
		return sound::get_tab_diplomacy_sound(state);
	}
};
class topbar_military_tab_button : public topbar_tab_button {
public:
	sound::audio_instance& get_click_sound(sys::state& state) noexcept override {
		return sound::get_tab_military_sound(state);
	}
};
class topbar_population_tab_button : public topbar_tab_button {
public:
	sound::audio_instance& get_click_sound(sys::state& state) noexcept override {
		return sound::get_tab_population_sound(state);
	}
};
class topbar_production_tab_button : public topbar_tab_button {
public:
	sound::audio_instance& get_click_sound(sys::state& state) noexcept override {
		return sound::get_tab_production_sound(state);
	}
};
class topbar_technology_tab_button : public topbar_tab_button {
public:
	sound::audio_instance& get_click_sound(sys::state& state) noexcept override {
		return sound::get_tab_technology_sound(state);
	}
};

class topbar_population_view_button : public topbar_tab_button {
public:
	sound::audio_instance& get_click_sound(sys::state& state) noexcept override {
		return sound::get_tab_population_sound(state);
	}

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

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "alice_topbar_tab_2");
	}
};

class topbar_date_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::date_to_string(state, state.current_date));
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(state.network_mode == sys::network_mode_type::host) {
			for(auto& pl : state.network_state.clients) {
				if(!pl.is_active()) {
					continue;
				}
				text::substitution_map sub{};

				text::add_to_substitution_map(sub, text::variable_type::name, pl.hshake_buffer.nickname.to_string_view());
				text::add_to_substitution_map(sub, text::variable_type::country, state.world.mp_player_get_nation_from_player_nation(pl.player_id));
				text::add_to_substitution_map(sub, text::variable_type::date, pl.last_seen);

				auto box = text::open_layout_box(contents);
				text::localised_format_box(state, contents, box, "alice_player_date_sync", sub);
				text::close_layout_box(contents, box);
			}
		}
	}
};

class topbar_pause_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		base_data.data.button.shortcut = sys::virtual_key::SPACE;
	}

	sound::audio_instance& get_click_sound(sys::state& state) noexcept override {
		if(state.actual_game_speed <= 0) {
			return sound::get_unpause_sound(state);
		}
		return sound::get_pause_sound(state);
	}

	void button_action(sys::state& state) noexcept override {
		if(state.actual_game_speed <= 0) {
			state.actual_game_speed = state.ui_state.held_game_speed;
		} else {
			state.ui_state.held_game_speed = state.actual_game_speed.load();
			state.actual_game_speed = 0;
			if(state.network_mode == sys::network_mode_type::host) {
				command::notify_pause_game(state, state.local_player_nation);
			}
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(state.network_mode == sys::network_mode_type::client) {
			disabled = true;
		} else {
			disabled = state.internally_paused || state.ui_pause.load(std::memory_order::acquire);
			disabled = disabled || ui::events_pause_test(state);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(state.network_mode == sys::network_mode_type::client) {
			text::add_line(state, contents, "alice_only_host_speed");
		} else {
			text::add_line(state, contents, "topbar_pause_speed");
		}

		auto ymd = state.current_date.to_ymd(state.start_date);
		if(sys::is_leap_year(ymd.year)) {
			text::add_line(state, contents, "date_is_leap");
		} else {
			text::add_line(state, contents, "date_is_not_leap");
		}

		float nh_temp = 15.f;
		std::string nh_season;
		if(ymd.month == 12 || ymd.month <= 2) {
			nh_season = text::produce_simple_string(state, "winter");
		} else if(ymd.month >= 3 && ymd.month <= 5) {
			nh_season = text::produce_simple_string(state, "spring");
		} else if(ymd.month >= 6 && ymd.month <= 8) {
			nh_season = text::produce_simple_string(state, "summer");
		} else if(ymd.month >= 9 && ymd.month <= 11) {
			nh_season = text::produce_simple_string(state, "autumn");
		}
		text::add_line(state, contents, "topbar_date_season_nh", text::variable_type::x, std::string_view(nh_season));

		std::string sh_season;
		if(ymd.month >= 6 && ymd.month <= 8) {
			sh_season = text::produce_simple_string(state, "winter");
		} else if(ymd.month >= 9 && ymd.month <= 11) {
			sh_season = text::produce_simple_string(state, "spring");
		} else if(ymd.month == 12 || ymd.month <= 2) {
			sh_season = text::produce_simple_string(state, "summer");
		} else if(ymd.month >= 3 && ymd.month <= 5) {
			sh_season = text::produce_simple_string(state, "autumn");
		}
		text::add_line(state, contents, "topbar_date_season_sh", text::variable_type::x, std::string_view(sh_season));

		//auto r = ((float(rng::reduce(state.game_seed, 4096)) / 4096.f) * 8.f) - 4.f;
		//float avg_temp = (nh_temp + sh_temp + r) / 2.f;
		//text::add_line(state, contents, "topbar_date_temperature", text::variable_type::x, text::fp_two_places{ avg_temp }, text::variable_type::y, text::fp_two_places{ (avg_temp * (9.f / 5.f)) + 32.f });
		//topbar_date_temperature;Average temperature: §Y$x$°C§! / §Y$y$°F§!
	}
};

class topbar_speedup_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		base_data.data.button.shortcut = sys::virtual_key::ADD;
		disabled = state.network_mode == sys::network_mode_type::client;
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
		if(state.network_mode == sys::network_mode_type::client) {
			text::localised_format_box(state, contents, box, std::string_view("alice_only_host_speed"));
		} else {
			text::localised_format_box(state, contents, box, std::string_view("topbar_inc_speed"));
		}
		text::close_layout_box(contents, box);
	}
};

class topbar_speeddown_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		base_data.data.button.shortcut = sys::virtual_key::SUBTRACT;
		disabled = state.network_mode == sys::network_mode_type::client;
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
		if(state.network_mode == sys::network_mode_type::client) {
			text::localised_format_box(state, contents, box, std::string_view("alice_only_host_speed"));
		} else {
			text::localised_format_box(state, contents, box, std::string_view("topbar_dec_speed"));
		}
		text::close_layout_box(contents, box);
	}
};

class topbar_speed_indicator : public topbar_pause_button {
public:
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(state.network_mode == sys::network_mode_type::single_player || state.network_mode == sys::network_mode_type::host) {
			if(state.internally_paused || state.ui_pause.load(std::memory_order::acquire)) {
				frame = 0;
			} else {
				frame = state.actual_game_speed;
			}
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
		if(!nations::is_great_power(state, state.local_player_nation)) {
			text::add_line(state, contents, "countryalert_no_gpstatus");
		} else if(state.world.nation_get_rank(state.local_player_nation) > uint16_t(state.defines.great_nations_count)) {
			text::add_line(state, contents, "alice_lose_gp");
			auto box = text::open_layout_box(contents);
			text::substitution_map sub{};
			text::add_to_substitution_map(sub, text::variable_type::x, int32_t(state.defines.great_nations_count));
			for(const auto gp : state.great_nations) {
				if(gp.nation == state.local_player_nation) {
					text::add_to_substitution_map(sub, text::variable_type::date, gp.last_greatness + int32_t(state.defines.greatness_days));
					break;
				}
			}
			text::localised_format_box(state, contents, box, "alice_gp_status_regain_expiration", sub);
			text::close_layout_box(contents, box);
		} else if(state.world.nation_get_rank(state.local_player_nation) <= uint16_t(state.defines.great_nations_count)) {
			text::add_line(state, contents, "countryalert_no_loosinggpstatus");
		}
	}
};

class topbar_at_peace_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
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
				text::localised_format_box(state, contents, box, "countryalert_isbuildingfactories", text::substitution_map{});
				auto nation_fat_id = dcon::fatten(state.world, nation_id);
				nation_fat_id.for_each_factory_construction([&](dcon::factory_construction_id building_slim_id) {
					auto building_fat_id = dcon::fatten(state.world, building_slim_id);
					auto stateName = building_fat_id.get_province().get_name();
					auto factoryType = building_fat_id.get_type().get_name();

					text::add_line_break_to_layout_box(state, contents, box);
					text::add_to_layout_box(state, contents, box, stateName);
					text::add_space_to_layout_box(state, contents, box);
					text::add_to_layout_box(state, contents, box, factoryType);
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
				text::add_line_break_to_layout_box(state, contents, box);
				auto nation_fat = dcon::fatten(state.world, nation_id);
				for(auto staat_owner : nation_fat.get_state_ownership()) {
					auto staat = staat_owner.get_state().get_definition();
					bool new_staat = true;
					for(auto abstract : staat.get_abstract_state_membership()) {
						auto prov = abstract.get_province();
						for(auto factloc : prov.get_factory_location()) {
							auto scale = economy::factory_total_desired_employment_score(state, factloc.get_factory());
							if(scale < economy::factory_closed_threshold) {
								if(new_staat) {
									text::add_to_layout_box(state, contents, box, staat.get_name(), text::text_color::yellow);
									// text::add_divider_to_layout_box(state, contents, box);		// TODO - Parity needed!
									text::add_line_break_to_layout_box(state, contents, box);
									new_staat = false;
								}
								text::add_to_layout_box(state, contents, box, std::string_view(" - "));
								text::add_to_layout_box(state, contents, box, factloc.get_factory().get_building_type().get_name(),
										text::text_color::yellow);
								text::add_line_break_to_layout_box(state, contents, box);
							}
						}
					}
				}
			}
			text::close_layout_box(contents, box);
		}
	}
};

class topbar_budget_warning : public image_element_base {
public:
	bool visible = false;
	void on_update(sys::state& state) noexcept override {
		visible = (state.world.nation_get_spending_level(state.local_player_nation) < 1.0f);
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(visible) {
			text::add_line(state, contents, "topbar_budget_warning");
		}
	}
};

class topbar_unemployment_icon : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		bool has_unemployed = false;
		// Only care about factory workers for displaying the red alert
		std::array<dcon::pop_type_id, 2> factory_workers{
			state.culture_definitions.primary_factory_worker,
			state.culture_definitions.secondary_factory_worker
		};
		for(auto pt : factory_workers) {
			for(auto si : state.world.nation_get_state_ownership(nation_id)) {
				auto state_instance = si.get_state();
				auto total = state_instance.get_demographics(demographics::to_key(state, pt));
				auto unemployed = total - state_instance.get_demographics(demographics::to_employment_key(state, pt));
				if(unemployed >= 1.0f) {
					has_unemployed = true;
					break;
				}
			}
			if(has_unemployed)
				break;
		}
		frame = has_unemployed ? 0 : 1;
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		bool header = false;
		std::array<dcon::pop_type_id, 2> factory_workers{
			state.culture_definitions.primary_factory_worker,
			state.culture_definitions.secondary_factory_worker
		};
		for(auto pt : factory_workers) {
			for(auto si : state.world.nation_get_state_ownership(nation_id)) {
				auto state_instance = si.get_state();
				auto total = state_instance.get_demographics(demographics::to_key(state, pt));
				auto unemployed = total - state_instance.get_demographics(demographics::to_employment_key(state, pt));
				if(unemployed >= 1.f) {
					if(!header) {
						text::add_line(state, contents, "remove_countryalert_hasunemployedworkers");
						header = true;
					}
					text::substitution_map sub;
					text::add_to_substitution_map(sub, text::variable_type::num, int64_t(unemployed));
					text::add_to_substitution_map(sub, text::variable_type::type, state.world.pop_type_get_name(pt));
					auto state_name = text::get_dynamic_state_name(state, state_instance);
					text::add_to_substitution_map(sub, text::variable_type::state, std::string_view{ state_name });
					text::add_to_substitution_map(sub, text::variable_type::perc, text::fp_two_places{ (unemployed / total) * 100.f });
					auto box = text::open_layout_box(contents);
					text::localised_format_box(state, contents, box, "topbar_unemployed", sub);
					text::close_layout_box(contents, box);
				}
			}
		}
		std::array<dcon::pop_type_id, 2> rgo_workers{
			state.culture_definitions.farmers,
			state.culture_definitions.laborers
		};
		for(auto pt : rgo_workers) {
			for(auto si : state.world.nation_get_state_ownership(nation_id)) {
				auto state_instance = si.get_state();
				auto total = state_instance.get_demographics(demographics::to_key(state, pt));
				auto unemployed = total - state_instance.get_demographics(demographics::to_employment_key(state, pt));
				if(unemployed >= 1.f) {
					if(!header) {
						text::add_line(state, contents, "alice_rgo_unemployment_country_alert");
						header = true;
					}
					text::substitution_map sub;
					text::add_to_substitution_map(sub, text::variable_type::num, int64_t(unemployed));
					text::add_to_substitution_map(sub, text::variable_type::type, state.world.pop_type_get_name(pt));
					auto state_name = text::get_dynamic_state_name(state, state_instance);
					text::add_to_substitution_map(sub, text::variable_type::state, std::string_view{ state_name });
					text::add_to_substitution_map(sub, text::variable_type::perc, text::fp_two_places{ (unemployed / total) * 100.f });
					auto box = text::open_layout_box(contents);
					text::localised_format_box(state, contents, box, "topbar_unemployed", sub);
					text::close_layout_box(contents, box);
				}
			}
		}
		if(!header) {
			text::add_line(state, contents, "countryalert_no_hasunemployedworkers");
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
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
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
					for(auto o : state.world.issue_get_options(i)) {
						if(o && politics::can_enact_political_reform(state, nation_id, o)) {
							auto fat_id = dcon::fatten(state.world, o);
							text::add_to_layout_box(state, contents, box, fat_id.get_parent_issue().get_name());
							text::add_to_layout_box(state, contents, box, std::string_view(": "));
							text::add_to_layout_box(state, contents, box, fat_id.get_name());
							text::add_line_break_to_layout_box(state, contents, box);
						}
					}
				}

				for(auto i : state.culture_definitions.social_issues) {
					for(auto o : state.world.issue_get_options(i)) {
						if(o && politics::can_enact_social_reform(state, nation_id, o)) {
							auto fat_id = dcon::fatten(state.world, o);
							text::add_to_layout_box(state, contents, box, fat_id.get_parent_issue().get_name());
							text::add_to_layout_box(state, contents, box, std::string_view(": "));
							text::add_to_layout_box(state, contents, box, fat_id.get_name());
							text::add_line_break_to_layout_box(state, contents, box);
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
							text::add_to_layout_box(state, contents, box, fat_id.get_parent_reform().get_name());
							text::add_to_layout_box(state, contents, box, std::string_view(": "));
							text::add_to_layout_box(state, contents, box, fat_id.get_name());
							text::add_line_break_to_layout_box(state, contents, box);
						}
					}
				}

				for(auto i : state.culture_definitions.economic_issues) {
					for(auto o : state.world.reform_get_options(i)) {
						if(o && politics::can_enact_economic_reform(state, nation_id, o)) {
							auto fat_id = dcon::fatten(state.world, o);
							text::add_to_layout_box(state, contents, box, fat_id.get_parent_reform().get_name());
							text::add_to_layout_box(state, contents, box, std::string_view(": "));
							text::add_to_layout_box(state, contents, box, fat_id.get_name());
							text::add_line_break_to_layout_box(state, contents, box);
						}
					}
				}
			}
		}
		text::close_layout_box(contents, box);
	}

	void button_action(sys::state& state) noexcept override {
		auto const override_and_show_tab = [&]() {
			state.ui_state.politics_subwindow->set_visible(state, true);
			state.ui_state.politics_subwindow->impl_on_update(state);

			Cyto::Any defs = Cyto::make_any<politics_window_tab>(politics_window_tab::reforms);
			state.ui_state.politics_subwindow->impl_get(state, defs);

			state.ui_state.root->move_child_to_front(state.ui_state.politics_subwindow);
			state.ui_state.topbar_subwindow = state.ui_state.politics_subwindow;
		};

		if(state.ui_state.topbar_subwindow->is_visible()) {
			state.ui_state.topbar_subwindow->set_visible(state, false);
			if(state.ui_state.topbar_subwindow != state.ui_state.politics_subwindow)
				override_and_show_tab();
		} else {
			override_and_show_tab();
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
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		if(!nations::has_decision_available(state, nation_id)) {
			text::add_line(state, contents, "countryalert_no_candodecisions");
		} else {
			text::add_line(state, contents, "countryalert_candodecisions");

			text::substitution_map m;
			produce_decision_substitutions(state, m, state.local_player_nation);

			state.world.for_each_decision([&](dcon::decision_id di) {
				if(nation_id != state.local_player_nation || !state.world.decision_get_hide_notification(di)) {
					auto lim = state.world.decision_get_potential(di);
					if(!lim || trigger::evaluate(state, lim, trigger::to_generic(nation_id), trigger::to_generic(nation_id), 0)) {
						auto allow = state.world.decision_get_allow(di);
						if(!allow || trigger::evaluate(state, allow, trigger::to_generic(nation_id), trigger::to_generic(nation_id), 0)) {
							auto fat_id = dcon::fatten(state.world, di);
							auto box = text::open_layout_box(contents);
							text::add_to_layout_box(state, contents, box, fat_id.get_name(), m);
							text::close_layout_box(contents, box);
						}
					}
				}
			});
		}
	}

	void button_action(sys::state& state) noexcept override {
		auto const override_and_show_tab = [&]() {
			state.ui_state.politics_subwindow->set_visible(state, true);
			state.ui_state.politics_subwindow->impl_on_update(state);

			Cyto::Any defs = Cyto::make_any<politics_window_tab>(politics_window_tab::decisions);
			state.ui_state.politics_subwindow->impl_get(state, defs);

			state.ui_state.root->move_child_to_front(state.ui_state.politics_subwindow);
			state.ui_state.topbar_subwindow = state.ui_state.politics_subwindow;
		};

		if(state.ui_state.topbar_subwindow->is_visible()) {
			state.ui_state.topbar_subwindow->set_visible(state, false);
			if(state.ui_state.topbar_subwindow != state.ui_state.politics_subwindow)
				override_and_show_tab();
		} else {
			override_and_show_tab();
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
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
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
};

class topbar_rebels_icon : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		for(auto rf : state.world.nation_get_rebellion_within(state.local_player_nation)) {
			auto org = rf.get_rebels().get_organization();
			if(org >= 0.01f) {
				frame = 0;
				return;
			}
		}
		frame = 1;
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		bool showed_title = false;

		for(auto rf : state.world.nation_get_rebellion_within(state.local_player_nation)) {
			auto org = rf.get_rebels().get_organization();
			if(org >= 0.01f) {
				if(!showed_title) {
					text::add_line(state, contents, "countryalert_haverebels");
					text::add_line_break_to_layout(state, contents);
					showed_title = true;
				}
				auto rebelname = rebel::rebel_name(state, rf.get_rebels());
					auto rebelsize = rf.get_rebels().get_possible_regiments();

				text::add_line(state, contents, "topbar_faction",
					text::variable_type::name, std::string_view{ rebelname },
					text::variable_type::strength, text::pretty_integer{ rebelsize },
					text::variable_type::org, text::fp_percentage{org});
			}
		}
	}

	void button_action(sys::state& state) noexcept override {
		auto const override_and_show_tab = [&]() {
			state.ui_state.politics_subwindow->set_visible(state, true);
			state.ui_state.politics_subwindow->impl_on_update(state);

			Cyto::Any defs = Cyto::make_any<politics_window_tab>(politics_window_tab::movements);
			state.ui_state.politics_subwindow->impl_get(state, defs);

			state.ui_state.root->move_child_to_front(state.ui_state.politics_subwindow);
			state.ui_state.topbar_subwindow = state.ui_state.politics_subwindow;
		};

		if(state.ui_state.topbar_subwindow->is_visible()) {
			state.ui_state.topbar_subwindow->set_visible(state, false);
			if(state.ui_state.topbar_subwindow != state.ui_state.politics_subwindow)
				override_and_show_tab();
		} else {
			override_and_show_tab();
		}
	}

};

class topbar_colony_icon : public standard_nation_button {
	uint32_t index = 0;

	dcon::province_id get_state_def_province(sys::state& state, dcon::state_definition_id sdef) noexcept {
		for(auto const p : state.world.state_definition_get_abstract_state_membership(sdef)) {
			if(!p.get_province().get_nation_from_province_ownership()) {
				return p.get_province();
			}
		}
		return dcon::province_id{};
	}
public:
	void button_action(sys::state& state) noexcept override {
		std::vector<dcon::province_id> provinces;
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		auto nation_fat_id = dcon::fatten(state.world, nation_id);
		for(auto si : state.world.nation_get_state_ownership(nation_id)) {
			if(province::can_integrate_colony(state, si.get_state())) {
				provinces.push_back(si.get_state().get_capital());
			}
		}

		state.world.for_each_state_definition([&](dcon::state_definition_id sdef) {
			if(province::can_start_colony(state, nation_id, sdef)) {
				dcon::province_id province;
				for(auto p : state.world.state_definition_get_abstract_state_membership(sdef)) {
					if(!p.get_province().get_nation_from_province_ownership()) {
						province = p.get_province().id;
						break;
					}
				}
				if(province) {
					provinces.push_back(province);
				}
			}
		});

		nation_fat_id.for_each_colonization([&](dcon::colonization_id colony) {
			auto sdef = state.world.colonization_get_state(colony);
			if(state.world.state_definition_get_colonization_stage(sdef) == 3) { //make protectorate
				provinces.push_back(get_state_def_province(state, sdef));
			} else if(province::can_invest_in_colony(state, nation_id, sdef)) { //invest
				provinces.push_back(get_state_def_province(state, sdef));
			} else { //losing rase
				auto lvl = state.world.colonization_get_level(colony);
				for(auto cols : state.world.state_definition_get_colonization(sdef)) {
					if(lvl < cols.get_level()) {
						provinces.push_back(get_state_def_province(state, sdef));
						break;
					}
				}
			}
		});
		if(!provinces.empty()) {
			index++;
			if(index >= uint32_t(provinces.size())) {
				index = 0;
			}
			if(auto prov = provinces[index]; prov && prov.value < state.province_definitions.first_sea_province.value) {
				sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
				state.set_selected_province(prov);
				static_cast<ui::province_view_window*>(state.ui_state.province_window)->set_active_province(state, prov);
				if(state.map_state.get_zoom() < map::zoom_very_close)
					state.map_state.zoom = map::zoom_very_close;
				state.map_state.center_map_on_province(state, prov);
			}
		}
	}

	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override {
		bool any_integratable = false;
		for(auto si : state.world.nation_get_state_ownership(nation_id)) {
			if(province::can_integrate_colony(state, si.get_state())) {
				any_integratable = true;
				break;
			}
		}
		state.world.for_each_state_definition([&](dcon::state_definition_id sdef) {
			if(province::can_start_colony(state, nation_id, sdef)) {
				any_integratable = true;
			}
		});
		if(nations::can_expand_colony(state, nation_id) || any_integratable) {
			return 0;
		} else if(nations::is_losing_colonial_race(state, nation_id)) {
			return 1;
		} else {
			return 2;
		}
	}
	// TODO - when the player clicks on the colony icon and there are colonies to expand then we want to teleport their camera to the
	// colony's position & open the prov window

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		auto nation_fat_id = dcon::fatten(state.world, nation_id);

		bool is_empty = true;

		for(auto si : state.world.nation_get_state_ownership(nation_id)) {
			if(province::can_integrate_colony(state, si.get_state())) {
				text::add_line(state, contents, "countryalert_colonialgood_state", text::variable_type::region, si.get_state().id);
				is_empty = false;
			}
		}

		state.world.for_each_state_definition([&](dcon::state_definition_id sdef) {
			if(province::can_start_colony(state, nation_id, sdef)) {
				text::add_line(state, contents, "alice_countryalert_colonialgood_start", text::variable_type::region, sdef);
				is_empty = false;
			}
		});

		nation_fat_id.for_each_colonization([&](dcon::colonization_id colony) {
			auto sdef = state.world.colonization_get_state(colony);
			if(state.world.state_definition_get_colonization_stage(sdef) == 3) {
				text::add_line(state, contents, "countryalert_colonialgood_colony", text::variable_type::region, sdef);
				is_empty = false;
			} else if(province::can_invest_in_colony(state, nation_id, sdef)) {
				text::add_line(state, contents, "countryalert_colonialgood_invest", text::variable_type::region, sdef);
				is_empty = false;
			}
			auto lvl = state.world.colonization_get_level(colony);
			for(auto cols : state.world.state_definition_get_colonization(sdef)) {
				if(lvl < cols.get_level()) {
					text::add_line(state, contents, "countryalert_colonialbad_influence", text::variable_type::region, sdef);
					is_empty = false;
				}
			}
		});

		if(is_empty) {
			text::add_line(state, contents, "countryalert_no_colonial");
		}
	}
};

class topbar_crisis_icon : public standard_nation_icon {
public:
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override {
		if(state.current_crisis_state == sys::crisis_state::inactive) {
			return 2;
		} else if(state.crisis_temperature > 80.f) {
			return 1;
		} else {
			return 0;
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::substitution_map sub;
		text::add_to_substitution_map(sub, text::variable_type::temperature, text::fp_two_places{state.crisis_temperature});
		text::add_to_substitution_map(sub, text::variable_type::attacker, state.primary_crisis_attacker);
		text::add_to_substitution_map(sub, text::variable_type::defender, state.primary_crisis_defender);
		text::add_to_substitution_map(sub, text::variable_type::date, state.last_crisis_end_date);
		text::add_to_substitution_map(sub, text::variable_type::time, int32_t(state.defines.crisis_cooldown_months));
		if(state.current_crisis_state == sys::crisis_state::inactive) {
			auto box = text::open_layout_box(contents);
			text::localised_format_box(state, contents, box, std::string_view("alice_crisis_inactive"), sub);
			text::close_layout_box(contents, box);
		} else if(state.current_crisis_state == sys::crisis_state::finding_attacker) {
			auto box = text::open_layout_box(contents);
			text::localised_format_box(state, contents, box, std::string_view("alice_crisis_finding_attacker"), sub);
			text::close_layout_box(contents, box);
		} else if(state.current_crisis_state == sys::crisis_state::finding_defender) {
			auto box = text::open_layout_box(contents);
			text::localised_format_box(state, contents, box, std::string_view("alice_crisis_finding_defender"), sub);
			text::close_layout_box(contents, box);
		} else if(state.current_crisis_state == sys::crisis_state::heating_up) {
			auto box = text::open_layout_box(contents);
			text::localised_format_box(state, contents, box, std::string_view("alice_crisis_heating_up"), sub);
			text::close_layout_box(contents, box);
			//atackers
			text::add_line(state, contents, std::string_view("alice_crisis_par_1"));
			for(const auto par : state.crisis_participants) {
				if(!par.merely_interested && par.supports_attacker) {
					text::add_line(state, contents, text::get_name(state, par.id));
				}
			}
			//defenders
			text::add_line(state, contents, std::string_view("alice_crisis_par_2"));
			for(const auto par : state.crisis_participants) {
				if(!par.merely_interested && !par.supports_attacker) {
					text::add_line(state, contents, text::get_name(state, par.id));
				}
			}
			//merely interested
			text::add_line(state, contents, std::string_view("alice_crisis_par_3"));
			for(const auto par : state.crisis_participants) {
				if(par.merely_interested) {
					text::add_line(state, contents, text::get_name(state, par.id));
				}
			}
		}
		if(state.last_crisis_end_date) {
			auto box = text::open_layout_box(contents);
			text::localised_format_box(state, contents, box, std::string_view("alice_last_crisis"), sub);
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
		auto n = retrieve<dcon::nation_id>(state, parent);

		if(!nations::is_great_power(state, n)) {
			text::add_line(state, contents, std::string_view("countryalert_no_gpstatus"));
		} else {
			bool added_increase_header = false;

			for(auto it : state.world.nation_get_gp_relationship_as_great_power(n)) {
				if((it.get_status() & nations::influence::is_banned) == 0) {
					if(it.get_influence() >= state.defines.increaseopinion_influence_cost
						&& (nations::influence::level_mask & it.get_status()) != nations::influence::level_in_sphere
						&& (nations::influence::level_mask & it.get_status()) != nations::influence::level_friendly) {

						if(!added_increase_header)
							text::add_line(state, contents, std::string_view("countryalert_canincreaseopinion"));
						added_increase_header = true;
						text::nation_name_and_flag(state, it.get_influence_target(), contents, 15);
					} else if(!(it.get_influence_target().get_in_sphere_of()) &&
										it.get_influence() >= state.defines.addtosphere_influence_cost) {
						if(!added_increase_header)
							text::add_line(state, contents, std::string_view("countryalert_canincreaseopinion"));
						added_increase_header = true;
						text::nation_name_and_flag(state, it.get_influence_target(), contents, 15);
					} else if(it.get_influence_target().get_in_sphere_of()
						&& (nations::influence::level_mask & it.get_status()) == nations::influence::level_friendly &&
										it.get_influence() >= state.defines.removefromsphere_influence_cost) {
						if(!added_increase_header)
							text::add_line(state, contents, std::string_view("countryalert_canincreaseopinion"));
						added_increase_header = true;
						text::nation_name_and_flag(state, it.get_influence_target(), contents, 15);
					}
				}
			}
			bool added_reb_header = false;
			for(auto m : state.world.in_nation) {
				if(state.world.nation_get_in_sphere_of(m) == n) {
					[&]() {
						for(auto fac : state.world.nation_get_rebellion_within(m)) {
							if(rebel::get_faction_brigades_active(state, fac.get_rebels()) > 0) {
								if(!added_reb_header)
									text::add_line(state, contents, std::string_view("a_alert_reb"));
								added_reb_header = true;
								text::nation_name_and_flag(state, m, contents, 15);
								return;
							}
						}
						}();
				}
			}

			if(!added_increase_header && !added_reb_header)
				text::add_line(state, contents, std::string_view("alice_ca_cant_influence"));
		}
	}
};

class topbar_nation_daily_research_points_text : public expanded_hitbox_text {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void on_create(sys::state& state) noexcept override {
		top_left_extension = ui::xy_pair{ 25, 3 };
		bottom_right_extension = ui::xy_pair{ 0, 2 };
		expanded_hitbox_text::on_create(state);
	}

	void on_update(sys::state& state) noexcept override {
		auto points = nations::daily_research_points(state, retrieve<dcon::nation_id>(state, parent));
		set_text(state, text::format_float(points, 2));
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		auto fat = dcon::fatten(state.world, nation_id);

		auto tech_id = nations::current_research(state, nation_id);

		auto total_pop = state.world.nation_get_demographics(nation_id, demographics::total);
		for(auto pt : state.world.in_pop_type) {
			auto rp = state.world.pop_type_get_research_points(pt);
			if(rp > 0 && state.world.nation_get_demographics(nation_id, demographics::to_key(state, pt)) > 0.0f) {
				auto amount = rp * std::min(1.0f, state.world.nation_get_demographics(nation_id, demographics::to_key(state, pt)) / (total_pop * state.world.pop_type_get_research_optimum(pt)));

				text::substitution_map sub1;
				text::add_to_substitution_map(sub1, text::variable_type::poptype, state.world.pop_type_get_name(pt));
				text::add_to_substitution_map(sub1, text::variable_type::value, text::fp_two_places{ amount });
				text::add_to_substitution_map(sub1, text::variable_type::fraction,
						text::fp_two_places{ (state.world.nation_get_demographics(nation_id, demographics::to_key(state, pt)) / total_pop) * 100 });
				text::add_to_substitution_map(sub1, text::variable_type::optimal,
					text::fp_two_places{ (state.world.pop_type_get_research_optimum(pt) * 100) });

				auto box = text::open_layout_box(contents, 0);
				text::localised_format_box(state, contents, box, std::string_view("tech_daily_researchpoints_tooltip"), sub1);
				text::close_layout_box(contents, box);
			}
		}
		active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::research_points, true);
		text::add_line_break_to_layout(state, contents);

		active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::research_points_modifier, true);
		text::add_line_break_to_layout(state, contents);

		if(!bool(tech_id)) {
			auto box2 = text::open_layout_box(contents, 0);
			text::localised_single_sub_box(state, contents, box2, std::string_view("rp_accumulated"), text::variable_type::val,
					text::fp_one_place{fat.get_research_points()});

			text::close_layout_box(contents, box2);
		}
	}
};

class topbar_commodity_xport_icon : public image_element_base {
public:
	uint8_t slot = 0;
	economy::trade_volume_data_detailed amount{};

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {

		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(
			state,
			contents,
			box,
			text::produce_simple_string(state, state.world.commodity_get_name(amount.commodity)),
			text::text_color::white
		);
		text::add_to_layout_box(state, contents, box, std::string_view(":"), text::text_color::white);
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::format_float(amount.volume, 2), text::text_color::white);

		text::add_line_break_to_layout_box(state, contents, box);

		for(auto i = 0; i < 5; i++) {
			if(amount.targets[i].nation) {
				text::add_line_break_to_layout_box(state, contents, box);
				auto ident = state.world.nation_get_identity_from_identity_holder(amount.targets[i].nation);
				add_to_layout_box(state, contents, box, text::embedded_flag{ ident ? ident : state.national_definitions.rebel_id });
				add_space_to_layout_box(state, contents, box);
				add_to_layout_box(state, contents, box, text::get_name(state, amount.targets[i].nation));
				text::add_to_layout_box(state, contents, box, std::string_view(":"), text::text_color::white);
				text::add_space_to_layout_box(state, contents, box);
				text::add_to_layout_box(state, contents, box, text::format_float(amount.targets[i].value, 2), text::text_color::white);
			}
		}

		text::close_layout_box(contents, box);
	}
};

class topbar_commodity_amount_icon : public image_element_base {
public:
	uint8_t slot = 0;
	dcon::commodity_id commodity_id{};
	float amount{};

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(state, contents, box,
				text::produce_simple_string(state, state.world.commodity_get_name(commodity_id)), text::text_color::white);
		text::add_to_layout_box(state, contents, box, std::string_view(":"), text::text_color::white);
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::format_float(amount, 2), text::text_color::white);
		text::close_layout_box(contents, box);
	}
};

class topbar_window : public window_element_base {
private:
	dcon::nation_id current_nation{};
	std::vector<topbar_commodity_xport_icon*> import_icons;
	std::vector<topbar_commodity_xport_icon*> export_icons;
	std::vector<topbar_commodity_amount_icon*> produced_icons;
	simple_text_element_base* atpeacetext = nullptr;

	army_management_window* army_mgmt_win = nullptr;

public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		base_data.position.y += 1;

		auto bg_pic = make_element_by_type<background_image>(state, "bg_main_menus");
		background_pic = bg_pic.get();
		// Some mods might remove the background image
		if(background_pic) {
			background_pic->base_data.position.y -= 1;
			add_child_to_back(std::move(bg_pic));
		}

		auto dpi_win = make_element_by_type<ui::diplomatic_message_topbar_listbox>(state, "alice_diplomessageicons_window");
		state.ui_state.request_topbar_listbox = dpi_win.get();
		add_child_to_front(std::move(dpi_win));

		state.ui_state.topbar_window = this;

		auto new_win = make_element_by_type<army_management_window>(state,
				state.ui_state.defs_by_name.find(state.lookup_key("army_management_window"))->second.definition);
		army_mgmt_win = new_win.get();
		army_mgmt_win->set_visible(state, false);
		add_child_to_front(std::move(new_win));

		on_update(state);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "topbar_bg") {
			return partially_transparent_image::make_element_by_type_alias(state, id);
		} else if(name == "topbar_paper") {
			return partially_transparent_image::make_element_by_type_alias(state, id);
		} else if(name == "topbarbutton_production") {
			auto btn = make_element_by_type<topbar_production_tab_button>(state, id);

			auto tab = make_element_by_type<production_window>(state, "country_production");
			state.ui_state.production_subwindow = state.ui_state.topbar_subwindow = btn->topbar_subwindow = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "topbarbutton_budget") {
			auto btn = make_element_by_type<topbar_budget_tab_button>(state, id);

			//auto tab = make_element_by_type<budget_window>(state, "country_budget");
			auto tab = alice_ui::make_budgetwindow_main(state);
			tab->set_visible(state, false);
			btn->topbar_subwindow = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "topbarbutton_tech") {
			auto btn = make_element_by_type<topbar_technology_tab_button>(state, id);

			auto tab = make_element_by_type<technology_window>(state, "country_technology");
			btn->topbar_subwindow = tab.get();

			state.ui_state.technology_subwindow = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "topbarbutton_politics") {
			auto btn = make_element_by_type<topbar_politics_tab_button>(state, id);
			auto tab = make_element_by_type<politics_window>(state, "country_politics");
			btn->topbar_subwindow = tab.get();

			state.ui_state.politics_subwindow = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "topbarbutton_pops") {
			auto btn = make_element_by_type<topbar_population_view_button>(state, id);
			//auto tab = make_element_by_type<population_window>(state, "country_pop");
			auto tab = alice_ui::make_demographicswindow_main(state);
			tab->set_visible(state, false);
			btn->topbar_subwindow = tab.get();

			state.ui_state.population_subwindow = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "topbarbutton_trade") {
			auto btn = make_element_by_type<topbar_trade_tab_button>(state, id);

			//auto tab = make_element_by_type<trade_window>(state, "alice_country_trade");
			auto tab = alice_ui::make_trade_dashboard_main(state);
			tab->set_visible(state, false);
			btn->topbar_subwindow = tab.get();

			state.ui_state.trade_subwindow = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "topbarbutton_diplomacy") {
			auto btn = make_element_by_type<topbar_diplomacy_tab_button>(state, id);

			auto tab = make_element_by_type<diplomacy_window>(state, "country_diplomacy");
			btn->topbar_subwindow = tab.get();
			state.ui_state.root->add_child_to_back(std::move(tab));
			return btn;
		} else if(name == "topbarbutton_military") {
			auto btn = make_element_by_type<topbar_military_tab_button>(state, id);

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
			return make_element_by_type<flag_button>(state, id);
		} else if(name == "country_prestige") {
			return make_element_by_type<topbar_nation_prestige_text>(state, id);
		} else if(name == "country_economic") {
			return make_element_by_type<topbar_nation_industry_score_text>(state, id);
		} else if(name == "country_military") {
			return make_element_by_type<topbar_nation_military_score_text>(state, id);
		} else if(name == "country_total") {
			return make_element_by_type<nation_total_score_text>(state, id);
		} else if(name == "country_colonial_power") {
			return make_element_by_type<topbar_nation_colonial_power_text>(state, id);
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
		} else if(name == "alice_budget_warning") {
			return make_element_by_type<topbar_budget_warning>(state, id);
		} else if(name == "budget_funds") {
			return make_element_by_type<topbar_treasury_text>(state, id);
		} else if(name == "topbar_tech_progress") {
			return make_element_by_type<nation_technology_research_progress>(state, id);
		} else if(name == "tech_current_research") {
			return make_element_by_type<nation_current_research_text>(state, id);
		} else if(name == "topbar_researchpoints_value") {
			return make_element_by_type<topbar_nation_daily_research_points_text>(state, id);
		} else if(name == "tech_literacy_value") {
			return make_element_by_type<topbar_nation_literacy_text>(state, id);
		} else if(name == "politics_party_icon") {
			return make_element_by_type<nation_ruling_party_ideology_plupp>(state, id);
		} else if(name == "politics_ruling_party") {
			return make_element_by_type<nation_ruling_party_text>(state, id);
		} else if(name == "politics_supressionpoints_value") {
			return make_element_by_type<nation_suppression_points_text>(state, id);
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
			return make_element_by_type<topbar_nation_population_text>(state, id);
		} else if(name == "topbar_focus_value") {
			return make_element_by_type<topbar_nation_focus_allocation_text>(state, id);
		} else if(name == "population_avg_mil_value") {
			return make_element_by_type<topbar_nation_militancy_text>(state, id);
		} else if(name == "population_avg_con_value") {
			return make_element_by_type<topbar_nation_consciousness_text>(state, id);
		} else if(name == "diplomacy_status") {
			auto ptr = make_element_by_type<topbar_at_peace_text>(state, id);
			atpeacetext = ptr.get();
			return ptr;
		} else if(name == "diplomacy_at_war") {
			auto ptr = make_element_by_type<overlapping_enemy_flags>(state, id);
			ptr->base_data.position.y -= ptr->base_data.position.y / 4;
			ptr->base_data.size.x /= 2;
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
		} else if(name == "topbar_outlinerbutton" || name == "topbar_outlinerbutton_bg") {
			// Fake button isn't used - we create it manually instead...
			return make_element_by_type<invisible_element>(state, id);
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
			auto ptr = make_element_by_type<topbar_commodity_amount_icon>(state, id);
			std::string var = std::string{name.substr(15)};
			var.empty() ? ptr->slot = uint8_t(0) : ptr->slot = uint8_t(std::stoi(var));
			produced_icons.push_back(ptr.get());
			return ptr;
		} else if (name == "selected_military_icon") {
			return make_element_by_type<military_score_icon>(state, id);
		}  else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		atpeacetext->set_visible(state, !state.world.nation_get_is_at_war(state.local_player_nation));
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
			static std::vector<economy::trade_volume_data_detailed> exported;
			static std::vector<economy::trade_volume_data_detailed> imported;

			exported.clear();
			imported.clear();

			for(dcon::commodity_id cid : state.world.in_commodity) {
				if(sys::commodity_group(state.world.commodity_get_commodity_group(cid)) != sys::commodity_group::military_goods &&
						sys::commodity_group(state.world.commodity_get_commodity_group(cid)) != sys::commodity_group::raw_material_goods &&
						sys::commodity_group(state.world.commodity_get_commodity_group(cid)) != sys::commodity_group::industrial_goods &&
						sys::commodity_group(state.world.commodity_get_commodity_group(cid)) != sys::commodity_group::consumer_goods &&
						sys::commodity_group(state.world.commodity_get_commodity_group(cid)) != sys::commodity_group::industrial_and_consumer_goods)
					return;
				exported.push_back(economy::export_volume_detailed(state, state.local_player_nation, cid));
				imported.push_back(economy::import_volume_detailed(state, state.local_player_nation, cid));
			}

			sys::merge_sort(exported.begin(), exported.end(), [&](auto& a, auto& b) {
				return a.volume > b.volume;
			});
			sys::merge_sort(imported.begin(), imported.end(), [&](auto& a, auto& b) {
				return a.volume > b.volume;
			});

			uint8_t slot = 0;
			for(size_t it = 0; it < exported.size(); it++) {
				for(auto const& e : export_icons)
					if(e->slot == slot) {
						e->frame = state.world.commodity_get_icon(exported[it].commodity);
						e->amount = exported[it];
						e->set_visible(state, true);
					}
				++slot;
			}
			slot = 0;
			for(size_t it = 0; it < imported.size(); it++) {
				for(auto const& e : import_icons)
					if(e->slot == slot) {
						e->frame = state.world.commodity_get_icon(imported[it].commodity);
						e->amount = imported[it];
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
						sys::commodity_group(state.world.commodity_get_commodity_group(cid)) != sys::commodity_group::consumer_goods &&
						sys::commodity_group(state.world.commodity_get_commodity_group(cid)) != sys::commodity_group::industrial_and_consumer_goods)
					return;
				v.insert({economy::supply(state, state.local_player_nation, cid) - economy::import_volume(state, state.local_player_nation, cid), cid.index() });
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
		if(background_pic) {
			if(state.ui_state.topbar_subwindow->is_visible()) {
				background_pic->set_visible(state, true);
			} else {
				background_pic->set_visible(state, false);
			}
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
