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
#include "gui_chat_window.hpp"
#include "gui_common_elements.hpp"
#include "nations.hpp"
#include "politics.hpp"
#include "rebels.hpp"
#include "system_state.hpp"
#include "text.hpp"
#include <vector>

namespace ui {

class topbar_nation_name : public generic_name_text<dcon::nation_id> {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, std::string_view("Nation ID: "));
		text::add_to_layout_box(state, contents, box, std::to_string(state.local_player_nation.value));
		text::close_layout_box(contents, box);
	}
};

class topbar_flag_button : public flag_button {
public:
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

class topbar_nation_total_score_text : public nation_total_score_text {
public:
};

class topbar_nation_colonial_power_text : public nation_colonial_power_text {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);

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
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("from_technology"), sub);
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::format_float(points, 0), text::text_color::green);

		text::close_layout_box(contents, box);
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
			datapoints[i] = state.player_data_cache.treasury_record[(state.ui_date.value + 1 + i) % 32] -
				state.player_data_cache.treasury_record[(state.ui_date.value + 0 + i) % 32];
		datapoints[datapoints.size() - 1] = state.player_data_cache.treasury_record[(state.ui_date.value + 1 + 31) % 32] -
																				state.player_data_cache.treasury_record[(state.ui_date.value + 0 + 31) % 32];
		datapoints[0] = datapoints[1]; // otherwise you will store the difference between two non-consecutive days here

		set_data_points(state, datapoints);
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
		auto literacy = state.world.nation_get_demographics(n, demographics::literacy);
		auto total_pop = std::max(1.0f, state.world.nation_get_demographics(n, demographics::total));
		set_text(state, text::format_percentage(literacy / total_pop, 1));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);

		auto box = text::open_layout_box(contents, 0);
		text::substitution_map sub;
		auto literacy_change = (demographics::get_estimated_literacy_change(state, nation_id) / 30);
		text::add_to_substitution_map(sub, text::variable_type::val, text::fp_four_places{literacy_change}); // TODO - This needs to display the estimated literacy change -breizh
		auto avg_literacy = text::format_percentage((state.world.nation_get_demographics(nation_id, demographics::literacy) / state.world.nation_get_demographics(nation_id, demographics::total)), 1);
		text::add_to_substitution_map(sub, text::variable_type::avg, std::string_view(avg_literacy));
		text::localised_format_box(state, contents, box, std::string_view("topbar_avg_literacy"), sub);
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("topbar_avg_change"), sub);
		text::close_layout_box(contents, box);
	}
};

class topbar_nation_ruling_party_text : public nation_ruling_party_text {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		auto fat_id = dcon::fatten(state.world, nation_id);
		std::string ruling_party = text::get_name_as_string(state, fat_id.get_ruling_party());
		ruling_party = ruling_party + " (" +
										text::get_name_as_string(state,
												state.world.political_party_get_ideology(state.world.nation_get_ruling_party(nation_id))) +
										")";
		auto box = text::open_layout_box(contents, 0);
		text::localised_single_sub_box(state, contents, box, std::string_view("topbar_ruling_party"), text::variable_type::curr, std::string_view(ruling_party));
		text::add_divider_to_layout_box(state, contents, box);
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
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			auto fat_id = dcon::fatten(state.world, nation_id);
			text::localised_single_sub_box(state, contents, box, std::string_view("diplomacy_infamy"), text::variable_type::value,
					text::fp_two_places{fat_id.get_infamy()});
			text::add_divider_to_layout_box(state, contents, box);
			// text::add_to_layout_box(state, contents, box, text::format_ratio(fat_id.get_infamy(), ));
			text::add_to_layout_box(state, contents, box, text::fp_two_places{fat_id.get_infamy()});
			text::add_to_layout_box(state, contents, box, std::string_view("/"));
			text::add_to_layout_box(state, contents, box, text::fp_two_places{state.defines.badboy_limit});
			text::add_line_break_to_layout_box(state, contents, box);
			text::localised_format_box(state, contents, box, std::string_view("badboy_dro_1"));
			text::close_layout_box(contents, box);

			active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::badboy, false);
		}
	}
};

class topbar_nation_population_text : public multiline_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		auto total_pop = state.world.nation_get_demographics(n, demographics::total);

		auto pop_amount = state.player_data_cache.population_record[state.ui_date.value % 32];
		auto pop_change = state.ui_date.value <= 32
			? (state.ui_date.value <= 2 ? 0.0f : pop_amount - state.player_data_cache.population_record[2])
			: (pop_amount - state.player_data_cache.population_record[(state.ui_date.value - 30) % 32]);

		text::text_color color = pop_change < 0 ?  text::text_color::red : text::text_color::green;
		if(pop_change == 0)
			color = text::text_color::white;

		auto layout = text::create_endless_layout(internal_layout,
		text::layout_parameters{0, 0, int16_t(base_data.size.x), int16_t(base_data.size.y), base_data.data.text.font_handle, 0, text::alignment::left, text::text_color::black, false});
		auto box = text::open_layout_box(layout, 0);


		text::add_to_layout_box(state, layout, box, text::prettify(int32_t(total_pop)));
		text::add_to_layout_box(state, layout, box, std::string(" ("));
		if(pop_change > 0) {
			text::add_to_layout_box(state, layout, box, std::string("+"), text::text_color::green);
		}
		text::add_to_layout_box(state, layout, box, text::pretty_integer{int64_t(pop_change)}, color);
		text::add_to_layout_box(state, layout, box, std::string(")"));

		text::close_layout_box(layout, box);
	}
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		return type != mouse_probe_type::tooltip ? message_result::unseen : message_result::consumed;
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		
		auto nation_id = retrieve<dcon::nation_id>(state, parent);

		auto pop_amount = state.player_data_cache.population_record[state.ui_date.value % 32];
		auto pop_change = state.ui_date.value <= 30 ? 0.0f : (pop_amount - state.player_data_cache.population_record[(state.ui_date.value - 30) % 32]);

		text::add_line(state, contents, "topbar_population_visual", text::variable_type::curr, text::pretty_integer{ int64_t(state.world.nation_get_demographics(nation_id, demographics::total)) });
		text::add_line_break_to_layout(state, contents);
		text::add_line(state, contents, "pop_growth_topbar_2", text::variable_type::x, text::pretty_integer{ int64_t(pop_change) });
		text::add_line(state, contents, "pop_growth_topbar", text::variable_type::x, text::pretty_integer{ int64_t(nations::get_monthly_pop_increase_of_nation(state, nation_id)) });

		text::add_line_break_to_layout(state, contents);
			
		active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::pop_growth, true);
	}

};

class topbar_treasury_text : public multiline_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);

		auto layout = text::create_endless_layout(internal_layout,
		text::layout_parameters{ 0, 0, int16_t(base_data.size.x), int16_t(base_data.size.y), base_data.data.text.font_handle, 0, text::alignment::center, text::text_color::black, false });
		auto box = text::open_layout_box(layout, 0);

		auto current_day_record = state.player_data_cache.treasury_record[state.ui_date.value % 32];
		auto previous_day_record = state.player_data_cache.treasury_record[(state.ui_date.value + 31) % 32];
		auto change = current_day_record - previous_day_record;

		text::add_to_layout_box(state, layout, box, text::prettify_currency(nations::get_treasury(state, n)));
		text::add_to_layout_box(state, layout, box, std::string(" ("));
		if(change > 0) {
			text::add_to_layout_box(state, layout, box, std::string("+"), text::text_color::green);
			text::add_to_layout_box(state, layout, box, text::prettify_currency( change ), text::text_color::green);
		} else if(change == 0) {
			text::add_to_layout_box(state, layout, box, text::prettify_currency( change ), text::text_color::white);
		} else {
			text::add_to_layout_box(state, layout, box, text::prettify_currency(change), text::text_color::red);
		}
		text::add_to_layout_box(state, layout, box, std::string(")"));

		text::close_layout_box(layout, box);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
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
		total_expense += economy::estimate_loan_payments(state, nation_id);
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
						-economy::estimate_loan_payments(state, nation_id) }); // $VAL - presumably loan payments == interest (?)
		text::add_line(state, contents, std::string_view("budget_imports"), text::variable_type::val,
				text::fp_one_place{ -economy::nation_total_imports(state,
						nation_id) }); // $VAL - presumably nation_total_imports is for national stockpile (?)

		text::add_line_break_to_layout(state, contents);

		text::add_line(state, contents, std::string_view("topbar_projected_income"), text::variable_type::val,
				text::fp_two_places{ economy::estimate_daily_income(state, nation_id) });
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
		set_text(state, text::format_ratio(in_use, available));
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
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
		set_text(state, text::format_float(militancy / total_pop));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		auto box = text::open_layout_box(contents, 0);
		text::substitution_map sub;
		auto mil_change = (demographics::get_estimated_mil_change(state, nation_id) / 30);
		text::add_to_substitution_map(sub, text::variable_type::avg,
				text::fp_two_places{(state.world.nation_get_demographics(nation_id, demographics::militancy) /
															state.world.nation_get_demographics(nation_id, demographics::total))});
		text::add_to_substitution_map(sub, text::variable_type::val,
				text::fp_four_places{mil_change}); // TODO - This needs to display the estimated militancy change -breizh
		text::localised_format_box(state, contents, box, std::string_view("topbar_avg_mil"), sub);
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("topbar_avg_change"), sub);
		text::close_layout_box(contents, box);

			
		active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::global_pop_militancy_modifier,
					true);
		active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::core_pop_militancy_modifier, true);
		active_modifiers_description(state, contents, nation_id, 0,
					sys::national_mod_offsets::non_accepted_pop_militancy_modifier, true);
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
		set_text(state, text::format_float(militancy / total_pop));
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
			text::add_line_break_to_layout_box(state, contents, box);
			text::localised_format_box(state, contents, box, std::string_view("topbar_avg_change"), sub);
			text::close_layout_box(contents, box);

			
				active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::global_pop_consciousness_modifier,
						true);
				active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::core_pop_consciousness_modifier,
						true);
				active_modifiers_description(state, contents, nation_id, 0,
						sys::national_mod_offsets::non_accepted_pop_consciousness_modifier, true);
			
		}
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
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto nation_id = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::curr,
					text::fp_one_place{nations::diplomatic_points(state, nation_id)});

			// Monthly gain
			text::add_to_substitution_map(sub, text::variable_type::value,
					text::fp_one_place{nations::monthly_diplomatic_points(state, nation_id)});

			text::substitution_map sub_base;
			// Base gain
			text::add_to_substitution_map(sub_base, text::variable_type::value,
					text::fp_one_place{state.defines.base_monthly_diplopoints});

			text::localised_format_box(state, contents, box, std::string_view("topbar_diplopoints"), sub);
			text::add_divider_to_layout_box(state, contents, box);
			text::localised_format_box(state, contents, box, std::string_view("topbar_diplopoints_gain"), sub);
			text::add_line_break_to_layout_box(state, contents, box);
			text::localised_format_box(state, contents, box, std::string_view("topbar_diplopoints_basegain"), sub_base);
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
			text::add_to_substitution_map(sub, text::variable_type::max, state.world.nation_get_recruitable_regiments(nation_id));
			text::localised_format_box(state, contents, box, std::string_view("topbar_army_tooltip"), sub);
			text::close_layout_box(contents, box);
		}
	}
};

class topbar_nation_navy_allocation_text : public nation_navy_allocation_text {
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
			text::add_line_break_to_layout_box(state, contents, box);
			text::add_line_break_to_layout_box(state, contents, box);
			text::add_to_substitution_map(sub, text::variable_type::tot,
					text::pretty_integer{military::naval_supply_points(state, nation_id)});
			text::add_to_substitution_map(sub, text::variable_type::req,
					text::pretty_integer{military::naval_supply_points_used(state, nation_id)});
			text::localised_format_box(state, contents, box, std::string_view("supply_load_status_desc_basic"), sub);
			text::add_line_break_to_layout_box(state, contents, box);
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
			text::add_line_break_to_layout_box(state, contents, box);
			text::localised_format_box(state, contents, box, std::string_view("mobilization_impact_limit_desc"), sub2);
			text::add_line_break_to_layout_box(state, contents, box);
			text::localised_format_box(state, contents, box, std::string_view("mobilization_impact_limit_desc2"), sub2);

			text::close_layout_box(contents, box);
		}
	}
};

class topbar_nation_leadership_points_text : public nation_leadership_points_text {
private:
	float getResearchPointsFromPop(sys::state& state, dcon::pop_type_id pop, dcon::nation_id n) {
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
		if(state.network_mode == sys::network_mode_type::client) {
			disabled = true;
		} else {
			disabled = state.internally_paused || state.ui_pause.load(std::memory_order::acquire);
		}
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
		text::localised_format_box(state, contents, box, std::string_view("topbar_inc_speed"));
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
				nation_fat_id.for_each_state_building_construction([&](dcon::state_building_construction_id building_slim_id) {
					auto building_fat_id = dcon::fatten(state.world, building_slim_id);
					auto stateName = building_fat_id.get_state().get_definition().get_name();
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
							auto scale = factloc.get_factory().get_production_scale();
							if(scale < 0.05f) {
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

class topbar_unemployment_icon : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		bool has_unemployed = false;
		for(auto pt : state.world.in_pop_type) {
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
		for(auto pt : state.world.in_pop_type) {
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
					text::add_to_substitution_map(sub, text::variable_type::num, int64_t(total));
					text::add_to_substitution_map(sub, text::variable_type::type, state.world.pop_type_get_name(state.culture_definitions.primary_factory_worker));
					auto state_name = text::get_dynamic_state_name(state, state_instance);
					text::add_to_substitution_map(sub, text::variable_type::state, std::string_view{ state_name });
					text::add_to_substitution_map(sub, text::variable_type::perc, text::fp_percentage_one_place{ unemployed / total });
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
	// TODO - when the player clicks on the colony icon and there are colonies to expand then we want to teleport their camera to the
	// colony's position & open the prov window

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		auto nation_fat_id = dcon::fatten(state.world, nation_id);

		bool is_empty = true;
		nation_fat_id.for_each_colonization([&](dcon::colonization_id colony) {
			auto sdef = state.world.colonization_get_state(colony);
			if(province::can_invest_in_colony(state, nation_id, sdef) || state.world.state_definition_get_colonization_stage(sdef) == 3) {
				text::add_line(state, contents, "countryalert_colonialgood_state", text::variable_type::region, sdef);
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
		auto box = text::open_layout_box(contents, 0);
		text::substitution_map sub;
		text::add_to_substitution_map(sub, text::variable_type::temperature, text::fp_two_places{state.crisis_temperature});
		if(state.current_crisis == sys::crisis_type::none) {
			text::localised_format_box(state, contents, box, std::string_view("countryalert_no_crisis"), sub);
		} else if(state.crisis_temperature > 0.8f) {
			text::localised_format_box(state, contents, box, std::string_view("countryalert_crisis"), sub);
		} else {
#define STRINGIFY(x) #x
			text::add_to_layout_box(state, contents, box, std::string_view(__FILE__ ":" STRINGIFY(__LINE__)));
#undef STRINGIFY
		}
		text::close_layout_box(contents, box);
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
							auto control = fac.get_rebels().get_province_rebel_control();
							if(control.begin() != control.end()) {
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
				text::add_line(state, contents, std::string_view("remove_countryalert_no_canincreaseopinion"));
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
		active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::research_points, false);
		text::add_line_break_to_layout(state, contents);

		active_modifiers_description(state, contents, nation_id, 0, sys::national_mod_offsets::research_points_modifier, false);
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
	dcon::commodity_id commodity_id{};
	float amount = 0.f;

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

class topbar_map_legend_title : public simple_text_element_base {
	std::string_view get_title_from_mode(map_mode::mode v) {
		switch(v) {
		case map_mode::mode::admin:
			return "mapmode_8";
		case map_mode::mode::civilization_level:
			return "mapmode_19";
		case map_mode::mode::colonial:
			return "mapmode_7";
		case map_mode::mode::crisis:
			return "mapmode_21";
		case map_mode::mode::migration:
			return "mapmode_18";
		case map_mode::mode::naval:
			return "mapmode_22";
		case map_mode::mode::nationality:
			return "mapmode_13";
		case map_mode::mode::national_focus:
			return "mapmode_10";
		case map_mode::mode::party_loyalty:
			return "mapmode_16";
		case map_mode::mode::political:
			return "mapmode_2";
		case map_mode::mode::population:
			return "mapmode_12";
		case map_mode::mode::rank:
			return "mapmode_17";
		case map_mode::mode::recruitment:
			return "mapmode_9";
		case map_mode::mode::region:
			return "mapmode_5";
		case map_mode::mode::relation:
			return "mapmode_20";
		case map_mode::mode::revolt:
			return "mapmode_3";
		case map_mode::mode::rgo_output:
			return "mapmode_11";
		case map_mode::mode::supply:
			return "mapmode_15";
		case map_mode::mode::terrain:
			return "mapmode_1";
		default:
			return "???";
		}
	}
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::produce_simple_string(state, get_title_from_mode(state.map_state.active_map_mode)));
	}
};
class topbar_map_legend_icon : public image_element_base {
	int16_t get_frame_from_mode(map_mode::mode v) {
		switch(v) {
		case map_mode::mode::migration:
		case map_mode::mode::rank:
		case map_mode::mode::relation:
		case map_mode::mode::rgo_output:
		case map_mode::mode::supply:
		case map_mode::mode::admin:
		case map_mode::mode::civilization_level:
		case map_mode::mode::colonial:
			return 1;
		case map_mode::mode::population:
		case map_mode::mode::crisis:
		case map_mode::mode::revolt:
			return 3;
		default:
			return 0;
		}
	}
public:
	void on_update(sys::state& state) noexcept override {
		frame = get_frame_from_mode(state.map_state.active_map_mode);
	}
};
class topbar_map_legend_gradient_max : public simple_text_element_base {
	std::string_view get_gradient_max_from_mode(map_mode::mode v) {
		switch(v) {
		case map_mode::mode::admin:
			return "lg_max_mapmode_8";
		case map_mode::mode::civilization_level:
			return "lg_max_mapmode_19";
		case map_mode::mode::colonial:
			return "lg_max_mapmode_7";
		case map_mode::mode::crisis:
			return "lg_max_mapmode_21";
		case map_mode::mode::migration:
			return "lg_max_mapmode_18";
		case map_mode::mode::population:
			return "lg_max_mapmode_12";
		case map_mode::mode::rank:
			return "lg_max_mapmode_17";
		case map_mode::mode::relation:
			return "lg_max_mapmode_20";
		case map_mode::mode::revolt:
			return "lg_max_mapmode_3";
		case map_mode::mode::rgo_output:
			return "lg_max_mapmode_11";
		case map_mode::mode::supply:
			return "lg_max_mapmode_15";
		default:
			return "???";
		}
	}
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::produce_simple_string(state, get_gradient_max_from_mode(state.map_state.active_map_mode)));
	}
};
class topbar_map_legend_gradient_min : public simple_text_element_base {
	std::string_view get_gradient_min_from_mode(map_mode::mode v) {
		switch(v) {
		case map_mode::mode::admin:
			return "lg_min_mapmode_8";
		case map_mode::mode::civilization_level:
			return "lg_min_mapmode_19";
		case map_mode::mode::colonial:
			return "lg_min_mapmode_7";
		case map_mode::mode::crisis:
			return "lg_min_mapmode_21";
		case map_mode::mode::migration:
			return "lg_min_mapmode_18";
		case map_mode::mode::population:
			return "lg_min_mapmode_12";
		case map_mode::mode::rank:
			return "lg_min_mapmode_17";
		case map_mode::mode::relation:
			return "lg_min_mapmode_20";
		case map_mode::mode::revolt:
			return "lg_min_mapmode_3";
		case map_mode::mode::rgo_output:
			return "lg_min_mapmode_11";
		case map_mode::mode::supply:
			return "lg_min_mapmode_15";
		default:
			return "???";
		}
	}
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::produce_simple_string(state, get_gradient_min_from_mode(state.map_state.active_map_mode)));
	}
};

class topbar_map_legend : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "legend_title") {
			return make_element_by_type<topbar_map_legend_title>(state, id);
		} else if(name == "gradient_icon") {
			return make_element_by_type<topbar_map_legend_icon>(state, id);
		} else if(name == "gradient_min") {
			return make_element_by_type<topbar_map_legend_gradient_min>(state, id);
		} else if(name == "gradient_max") {
			return make_element_by_type<topbar_map_legend_gradient_max>(state, id);
		} else {
			return nullptr;
		}
	}
};

class topbar_window : public window_element_base {
private:
	dcon::nation_id current_nation{};
	std::vector<topbar_commodity_xport_icon*> import_icons;
	std::vector<topbar_commodity_xport_icon*> export_icons;
	std::vector<topbar_commodity_xport_icon*> produced_icons;
	simple_text_element_base* atpeacetext = nullptr;
	element_base* map_legend = nullptr;

	bool get_needs_legend_from_mode(map_mode::mode v) {
		switch(v) {
		case map_mode::mode::migration:
		case map_mode::mode::population:
		case map_mode::mode::rank:
		case map_mode::mode::relation:
		case map_mode::mode::revolt:
		case map_mode::mode::rgo_output:
		case map_mode::mode::supply:
		case map_mode::mode::admin:
		case map_mode::mode::civilization_level:
		case map_mode::mode::colonial:
		case map_mode::mode::crisis:
			return true;
		default:
			return false;
		}
	}

public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		auto bg_pic = make_element_by_type<background_image>(state, "bg_main_menus");
		background_pic = bg_pic.get();
		add_child_to_back(std::move(bg_pic));

		auto legend_win = make_element_by_type<topbar_map_legend>(state, "alice_map_legend_window");
		map_legend = legend_win.get();
		add_child_to_front(std::move(legend_win));

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
			return make_element_by_type<topbar_nation_ruling_party_text>(state, id);
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
		atpeacetext->set_visible(state, !state.world.nation_get_is_at_war(state.local_player_nation));
		if(state.local_player_nation != current_nation) {
			current_nation = state.local_player_nation;
			Cyto::Any payload = current_nation;
			impl_set(state, payload);
		}

		map_legend->set_visible(state, get_needs_legend_from_mode(state.map_state.active_map_mode));

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
						sys::commodity_group(state.world.commodity_get_commodity_group(cid)) != sys::commodity_group::consumer_goods &&
						sys::commodity_group(state.world.commodity_get_commodity_group(cid)) != sys::commodity_group::industrial_and_consumer_goods)
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
						sys::commodity_group(state.world.commodity_get_commodity_group(cid)) != sys::commodity_group::consumer_goods &&
						sys::commodity_group(state.world.commodity_get_commodity_group(cid)) != sys::commodity_group::industrial_and_consumer_goods)
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
