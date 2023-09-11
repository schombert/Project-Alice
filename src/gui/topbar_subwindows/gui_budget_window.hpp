#pragma once

#include "commands.hpp"
#include "culture.hpp"
#include "dcon_generated.hpp"
#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"
#include "nations.hpp"
#include "system_state.hpp"
#include "text.hpp"
#include <cstdint>
#include <string_view>
#include <vector>

namespace dcon {
class pop_satisfaction_wrapper_id {
public:
	using value_base_t = uint8_t;
	value_base_t value = 0;
	pop_satisfaction_wrapper_id() { }
	pop_satisfaction_wrapper_id(uint8_t v) : value(v) { }
	value_base_t index() {
		return value;
	}
};
class pop_satisfaction_wrapper_fat {
	static text_sequence_id names[5];

public:
	uint8_t value = 0;
	void set_name(text_sequence_id text) noexcept {
		names[value] = text;
	}
	text_sequence_id get_name() noexcept {
		switch(value) {
		case 0: // No needs fulfilled
		case 1: // Some life needs
		case 2: // All life needs, some everyday
		case 3: // All everyday, some luxury
		case 4: // All luxury
			return names[value];
		}
		return text_sequence_id{0};
	}
};
pop_satisfaction_wrapper_fat fatten(data_container const& c, pop_satisfaction_wrapper_id id) noexcept {
	return pop_satisfaction_wrapper_fat{id.value};
}
} // namespace dcon
namespace ogl {
template<>
uint32_t get_ui_color(sys::state& state, dcon::pop_satisfaction_wrapper_id id) {
	switch(id.value) {
	case 0: // red
		return sys::pack_color(1.0f, 0.1f, 0.1f);
	case 1: // yellow
		return sys::pack_color(1.0f, 1.0f, 0.1f);
	case 2: // green
		return sys::pack_color(0.1f, 1.0f, 0.1f);
	case 3: // blue
		return sys::pack_color(0.1f, 0.1f, 1.0f);
	case 4: // light blue
		return sys::pack_color(0.1f, 1.0f, 1.0f);
	}
	return 0;
}
} // namespace ogl

namespace ui {


class nation_actual_stockpile_spending_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::format_money(economy::estimate_stockpile_filling_spending(state, state.local_player_nation)));
	}
};

class nation_gold_income_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::format_money(economy::estimate_gold_income(state, state.local_player_nation)));
	}
};

class nation_loan_spending_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::format_money(economy::estimate_loan_payments(state, state.local_player_nation)));
	}
};

class nation_diplomatic_balance_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::format_money(economy::estimate_diplomatic_balance(state, state.local_player_nation)));
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);

		float w_subsidies_amount =
			economy::estimate_war_subsidies_income(state, n) - economy::estimate_war_subsidies_spending(state, n);
		float reparations_amount = economy::estimate_reparations_income(state, n) - economy::estimate_reparations_spending(state, n);

		if(w_subsidies_amount > 0.0f) {
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::val, text::fp_one_place{ w_subsidies_amount });
			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, "warsubsidies_income", m);
			text::close_layout_box(contents, box);
		} else if(w_subsidies_amount < 0.0f) {
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::val, text::fp_one_place{ w_subsidies_amount });
			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, "warsubsidies_expense", m);
			text::close_layout_box(contents, box);
		}

		if(reparations_amount > 0.0f) {
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::val, text::fp_one_place{ w_subsidies_amount });
			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, "warindemnities_income", m);
			text::close_layout_box(contents, box);
		} else if(reparations_amount < 0.0f) {
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::val, text::fp_one_place{ w_subsidies_amount });
			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, "warindemnities_expense", m);
			text::close_layout_box(contents, box);
		}
	}
};

class nation_subsidy_spending_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::format_money(economy::estimate_subsidy_spending(state, state.local_player_nation)));
	}
};

class nation_administrative_efficiency_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::format_percentage(state.world.nation_get_administrative_efficiency(state.local_player_nation)));
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto n = state.local_player_nation;

		{
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::val,
					text::fp_percentage{ 1.0f + state.world.nation_get_modifier_values(n, sys::national_mod_offsets::administrative_efficiency_modifier) });
			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, "admin_explain_1", m);
			text::close_layout_box(contents, box);
		}
		active_modifiers_description(state, contents, n, 15, sys::national_mod_offsets::administrative_efficiency_modifier,
				false);
		{
			auto non_colonial = state.world.nation_get_non_colonial_population(n);
			auto total = non_colonial > 0.0f ? state.world.nation_get_non_colonial_bureaucrats(n) / non_colonial : 0.0f;

			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::val, text::fp_two_places{ total * 100.0f });
			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, "admin_explain_2", m);
			text::close_layout_box(contents, box);
		}
		{
			float issue_sum = 0.0f;
			for(auto i : state.culture_definitions.social_issues) {
				issue_sum = issue_sum + state.world.issue_option_get_administrative_multiplier(state.world.nation_get_issues(n, i));
			}
			auto from_issues = issue_sum * state.defines.bureaucracy_percentage_increment;

			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::val,
					text::fp_two_places{ (from_issues + state.defines.max_bureaucracy_percentage) * 100.0f });
			text::add_to_substitution_map(m, text::variable_type::x,
					text::fp_two_places{ state.defines.max_bureaucracy_percentage * 100.0f });
			text::add_to_substitution_map(m, text::variable_type::y, text::fp_two_places{ from_issues * 100.0f });
			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, "admin_explain_3", m);
			text::close_layout_box(contents, box);
		}
	}
};

template<culture::pop_strata Strata>
class pop_satisfaction_piechart : public piechart<dcon::pop_satisfaction_wrapper_id> {
protected:
	std::unordered_map<dcon::pop_satisfaction_wrapper_id::value_base_t, float> get_distribution(
			sys::state& state) noexcept override {
		std::unordered_map<dcon::pop_satisfaction_wrapper_id::value_base_t, float> distrib = {};
		
		enabled = true;
		if(parent == nullptr)
			return distrib;

		auto total = 0.f;
		std::array<float, 5> sat_pool = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		for(auto prov : state.world.nation_get_province_ownership(state.local_player_nation)) {

			for(auto pop_loc : prov.get_province().get_pop_location()) {
				auto pop_id = pop_loc.get_pop();
				auto pop_strata = state.world.pop_type_get_strata(state.world.pop_get_poptype(pop_id));
				auto pop_size = pop_strata == uint8_t(Strata) ? state.world.pop_get_size(pop_id) : 0.f;
					// All luxury needs
					// OR All everyday needs
					// OR All life needs
					// OR Some life needs
					// OR No needs fulfilled...
				sat_pool[(pop_id.get_luxury_needs_satisfaction() > 0.95f)             ? 4
								 : (pop_id.get_everyday_needs_satisfaction() > 0.95f) ? 3
								 : (pop_id.get_life_needs_satisfaction() > 0.95f)     ? 2
								 : (pop_id.get_life_needs_satisfaction() > 0.01f)     ? 1
								 : 0] += pop_size;
				total += pop_size;
			}
		}
		if(total <= 0.f) {
			enabled = false;
			return distrib;
		}

		for(size_t i = 0; i < sat_pool.size(); i++)
			distrib[dcon::pop_satisfaction_wrapper_id::value_base_t(i)] = sat_pool[i] / total;
		
		return distrib;
	}

public:
	void on_create(sys::state& state) noexcept override {
		// Fill-in static information...
		static bool has_run = false;
		if(!has_run) {
			dcon::fatten(state.world, dcon::pop_satisfaction_wrapper_id{0})
					.set_name(text::find_or_add_key(state, "BUDGET_STRATA_NO_NEED"));
			dcon::fatten(state.world, dcon::pop_satisfaction_wrapper_id{1})
					.set_name(text::find_or_add_key(state, "BUDGET_STRATA_NEED"));
			dcon::fatten(state.world, dcon::pop_satisfaction_wrapper_id{2})
					.set_name(text::find_or_add_key(state, "BUDGET_STRATA_NEED"));
			dcon::fatten(state.world, dcon::pop_satisfaction_wrapper_id{3})
					.set_name(text::find_or_add_key(state, "BUDGET_STRATA_NEED"));
			dcon::fatten(state.world, dcon::pop_satisfaction_wrapper_id{4})
					.set_name(text::find_or_add_key(state, "BUDGET_STRATA_NEED"));
			has_run = true;
		}
		piechart::on_create(state);
	}

	void populate_tooltip(sys::state& state, dcon::pop_satisfaction_wrapper_id psw, float percentage,
			text::columnar_layout& contents) noexcept override {
		static const std::string needs_types[5] = {"no_need", "some_life_needs", "life_needs", "everyday_needs", "luxury_needs"};
		auto fat_psw = dcon::fatten(state.world, psw);
		auto box = text::open_layout_box(contents, 0);
		auto sub = text::substitution_map{};
		auto needs_type = text::produce_simple_string(state, needs_types[psw.value]);
		text::add_to_substitution_map(sub, text::variable_type::val, text::fp_one_place{percentage * 100.f});
		text::add_to_substitution_map(sub, text::variable_type::type, std::string_view(needs_type));
		text::add_to_layout_box(state, contents, box, fat_psw.get_name(), sub);
		text::close_layout_box(contents, box);
	}
};

enum class budget_slider_target : uint8_t {
	poor_tax,
	middle_tax,
	rich_tax,
	army_stock,
	navy_stock,
	construction_stock,
	education,
	admin,
	social,
	military,
	tariffs,
	raw,
	target_count
};

struct budget_slider_signal {
	budget_slider_target target;
	float amount;
};

template<budget_slider_target SliderTarget>
class budget_slider : public scrollbar {
public:
	void on_value_change(sys::state& state, int32_t v) noexcept final {
		if(parent) {
			float amount = float(v) / 100.0f;
			Cyto::Any payload = budget_slider_signal{SliderTarget, amount};
			parent->impl_set(state, payload);
		}
		if(state.ui_state.drag_target == nullptr) {
			commit_changes(state);
		}
	}

	void on_update(sys::state& state) noexcept final {
		switch(SliderTarget) {
		case budget_slider_target::poor_tax: {
			auto min_tax =
					int32_t(100.0f * state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::min_tax));
			auto max_tax =
					int32_t(100.0f * state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::max_tax));
			if(max_tax <= 0)
				max_tax = 100;
			max_tax = std::max(min_tax, max_tax);

			mutable_scrollbar_settings new_settings;
			new_settings.lower_value = 0;
			new_settings.upper_value = 100;
			new_settings.using_limits = true;
			new_settings.lower_limit = std::clamp(min_tax, 0, 100);
			new_settings.upper_limit = std::clamp(max_tax, 0, 100);
			change_settings(state, new_settings);
		} break;
		case budget_slider_target::middle_tax: {
			auto min_tax =
					int32_t(100.0f * state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::min_tax));
			auto max_tax =
					int32_t(100.0f * state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::max_tax));
			if(max_tax <= 0)
				max_tax = 100;
			max_tax = std::max(min_tax, max_tax);

			mutable_scrollbar_settings new_settings;
			new_settings.lower_value = 0;
			new_settings.upper_value = 100;
			new_settings.using_limits = true;
			new_settings.lower_limit = std::clamp(min_tax, 0, 100);
			new_settings.upper_limit = std::clamp(max_tax, 0, 100);
			change_settings(state, new_settings);
		} break;
		case budget_slider_target::rich_tax: {
			auto min_tax =
					int32_t(100.0f * state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::min_tax));
			auto max_tax =
					int32_t(100.0f * state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::max_tax));
			if(max_tax <= 0)
				max_tax = 100;
			max_tax = std::max(min_tax, max_tax);

			mutable_scrollbar_settings new_settings;
			new_settings.lower_value = 0;
			new_settings.upper_value = 100;
			new_settings.using_limits = true;
			new_settings.lower_limit = std::clamp(min_tax, 0, 100);
			new_settings.upper_limit = std::clamp(max_tax, 0, 100);
			change_settings(state, new_settings);
		} break;
		case budget_slider_target::social: {
			auto min_spend = int32_t(100.0f * state.world.nation_get_modifier_values(state.local_player_nation,
																						sys::national_mod_offsets::min_social_spending));
			auto max_spend = int32_t(100.0f * state.world.nation_get_modifier_values(state.local_player_nation,
																						sys::national_mod_offsets::max_social_spending));
			if(max_spend <= 0)
				max_spend = 100;
			max_spend = std::max(min_spend, max_spend);

			mutable_scrollbar_settings new_settings;
			new_settings.lower_value = 0;
			new_settings.upper_value = 100;
			new_settings.using_limits = true;
			new_settings.lower_limit = std::clamp(min_spend, 0, 100);
			new_settings.upper_limit = std::clamp(max_spend, 0, 100);
			change_settings(state, new_settings);
		} break;
		case budget_slider_target::military: {
			auto min_spend = int32_t(100.0f * state.world.nation_get_modifier_values(state.local_player_nation,
																						sys::national_mod_offsets::min_military_spending));
			auto max_spend = int32_t(100.0f * state.world.nation_get_modifier_values(state.local_player_nation,
																						sys::national_mod_offsets::max_military_spending));
			if(max_spend <= 0)
				max_spend = 100;
			max_spend = std::max(min_spend, max_spend);

			mutable_scrollbar_settings new_settings;
			new_settings.lower_value = 0;
			new_settings.upper_value = 100;
			new_settings.using_limits = true;
			new_settings.lower_limit = std::clamp(min_spend, 0, 100);
			new_settings.upper_limit = std::clamp(max_spend, 0, 100);
			change_settings(state, new_settings);
		} break;
		case budget_slider_target::tariffs: {
			auto min_tariff = int32_t(
					100.0f * state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::min_tariff));
			auto max_tariff = int32_t(
					100.0f * state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::max_tariff));
			max_tariff = std::max(min_tariff, max_tariff);

			mutable_scrollbar_settings new_settings;
			new_settings.lower_value = -100;
			new_settings.upper_value = 100;
			new_settings.using_limits = true;
			new_settings.lower_limit = std::clamp(min_tariff, -100, 100);
			new_settings.upper_limit = std::clamp(max_tariff, -100, 100);
			change_settings(state, new_settings);
		} break;
		default:
			break;
		}

		int32_t v = 0;
		if(state.ui_state.drag_target == slider) {
			v = int32_t(scaled_value());
		} else {
			v = get_true_value(state);
			update_raw_value(state, v);
		}

		if(parent) {
			float amount = float(v) / 100.f;
			Cyto::Any payload = budget_slider_signal{SliderTarget, amount};
			parent->impl_set(state, payload);
		}
	}

	virtual int32_t get_true_value(sys::state& state) noexcept {
		return 0;
	}

	void on_drag_finish(sys::state& state) noexcept override {
		commit_changes(state);
	}

private:
	void commit_changes(sys::state& state) noexcept {
		auto budget_settings = command::make_empty_budget_settings();
		update_budget_settings(budget_settings);
		command::change_budget_settings(state, state.local_player_nation, budget_settings);
	}

	void update_budget_settings(command::budget_settings_data& budget_settings) noexcept {
		auto new_val = int8_t(scaled_value());
		switch(SliderTarget) {
		case budget_slider_target::poor_tax:
			budget_settings.poor_tax = new_val;
			break;
		case budget_slider_target::middle_tax:
			budget_settings.middle_tax = new_val;
			break;
		case budget_slider_target::rich_tax:
			budget_settings.rich_tax = new_val;
			break;
		case budget_slider_target::army_stock:
			budget_settings.land_spending = new_val;
			break;
		case budget_slider_target::navy_stock:
			budget_settings.naval_spending = new_val;
			break;
		case budget_slider_target::construction_stock:
			budget_settings.construction_spending = new_val;
			break;
		case budget_slider_target::education:
			budget_settings.education_spending = new_val;
			break;
		case budget_slider_target::admin:
			budget_settings.administrative_spending = new_val;
			break;
		case budget_slider_target::social:
			budget_settings.social_spending = new_val;
			break;
		case budget_slider_target::military:
			budget_settings.military_spending = new_val;
			break;
		case budget_slider_target::tariffs:
			budget_settings.tariffs = new_val;
			break;
		default:
			break;
		}
	}
};

class budget_poor_tax_slider : public budget_slider<budget_slider_target::poor_tax> {
	int32_t get_true_value(sys::state& state) noexcept override {
		return int32_t(state.world.nation_get_poor_tax(state.local_player_nation));
	}
};

class budget_middle_tax_slider : public budget_slider<budget_slider_target::middle_tax> {
	int32_t get_true_value(sys::state& state) noexcept override {
		return int32_t(state.world.nation_get_middle_tax(state.local_player_nation));
	}
};

class budget_rich_tax_slider : public budget_slider<budget_slider_target::rich_tax> {
	int32_t get_true_value(sys::state& state) noexcept override {
		return int32_t(state.world.nation_get_rich_tax(state.local_player_nation));
	}
};

class budget_army_stockpile_slider : public budget_slider<budget_slider_target::army_stock> {
	int32_t get_true_value(sys::state& state) noexcept override {
		return int32_t(state.world.nation_get_land_spending(state.local_player_nation));
	}
};

class budget_navy_stockpile_slider : public budget_slider<budget_slider_target::navy_stock> {
	int32_t get_true_value(sys::state& state) noexcept override {
		return int32_t(state.world.nation_get_naval_spending(state.local_player_nation));
	}
};

class budget_construction_stockpile_slider : public budget_slider<budget_slider_target::construction_stock> {
	int32_t get_true_value(sys::state& state) noexcept override {
		return int32_t(state.world.nation_get_construction_spending(state.local_player_nation));
	}
};

class budget_education_slider : public budget_slider<budget_slider_target::education> {
	int32_t get_true_value(sys::state& state) noexcept override {
		return int32_t(state.world.nation_get_education_spending(state.local_player_nation));
	}
};

class budget_administration_slider : public budget_slider<budget_slider_target::admin> {
	int32_t get_true_value(sys::state& state) noexcept override {
		return int32_t(state.world.nation_get_administrative_spending(state.local_player_nation));
	}
};

class budget_social_spending_slider : public budget_slider<budget_slider_target::social> {
	int32_t get_true_value(sys::state& state) noexcept override {
		return int32_t(state.world.nation_get_social_spending(state.local_player_nation));
	}
};

class budget_military_spending_slider : public budget_slider<budget_slider_target::military> {
	int32_t get_true_value(sys::state& state) noexcept override {
		return int32_t(state.world.nation_get_military_spending(state.local_player_nation));
	}
};

class budget_tariff_slider : public budget_slider<budget_slider_target::tariffs> {
	int32_t get_true_value(sys::state& state) noexcept override {
		return int32_t(state.world.nation_get_tariffs(state.local_player_nation));
	}
};

class budget_scaled_monetary_value_text : public color_text_element {
private:
	std::array<float, size_t(budget_slider_target::target_count)> values;
	std::array<float, size_t(budget_slider_target::target_count)> multipliers;

public:
	virtual void put_values(sys::state& state, std::array<float, size_t(budget_slider_target::target_count)>& vals) noexcept { }

	bool color_result = false;

	void on_create(sys::state& state) noexcept override {
		color_text_element::on_create(state);
		color = text::text_color::black;
		for(uint8_t i = 0; i < uint8_t(budget_slider_target::target_count); ++i) {
			values[i] = 0.f;
			multipliers[i] = 1.f;
		}
	}

	void apply_multipliers(sys::state& state) noexcept {
		auto total = 0.f;
		for(uint8_t i = 0; i < uint8_t(budget_slider_target::target_count); ++i)
			total += values[i] * multipliers[i];

		if(color_result) {
			if(total < 0.0f) {
				color = text::text_color::dark_red;
				set_text(state, text::format_money(total));
			} else if(total > 0.0f) {
				color = text::text_color::dark_green;
				set_text(state, std::string("+") + text::format_money(total));
			} else {
				color = text::text_color::black;
				set_text(state, text::format_money(total));
			}
		} else {
			set_text(state, text::format_money(total));
		}

		
	}

	void on_update(sys::state& state) noexcept override {
		put_values(state, values);
		apply_multipliers(state);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<budget_slider_signal>()) {
			auto sig = any_cast<budget_slider_signal>(payload);
			multipliers[uint8_t(sig.target)] = sig.amount;
			if(values[uint8_t(sig.target)] != 0.f)
				apply_multipliers(state);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class budget_estimated_stockpile_spending_text : public budget_scaled_monetary_value_text {
public:
	void put_values(sys::state& state, std::array<float, size_t(budget_slider_target::target_count)>& vals) noexcept override {
		vals[uint8_t(budget_slider_target::construction_stock)] =
				economy::estimate_construction_spending(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::army_stock)] = economy::estimate_land_spending(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::navy_stock)] = economy::estimate_naval_spending(state, state.local_player_nation);
	}
};

class budget_military_spending_text : public budget_scaled_monetary_value_text {
public:
	void put_values(sys::state& state, std::array<float, size_t(budget_slider_target::target_count)>& vals) noexcept override {
		vals[uint8_t(budget_slider_target::army_stock)] = economy::estimate_land_spending(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::navy_stock)] = economy::estimate_naval_spending(state, state.local_player_nation);
	}
};

class budget_overseas_spending_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::format_money(economy::estimate_overseas_penalty_spending(state, state.local_player_nation)));
	}
};

class budget_tariff_income_text : public budget_scaled_monetary_value_text {
public:
	void put_values(sys::state& state, std::array<float, size_t(budget_slider_target::target_count)>& vals) noexcept override {
		vals[uint8_t(budget_slider_target::tariffs)] = economy::estimate_tariff_income(state, state.local_player_nation);
	}
};

template<culture::pop_strata Strata, budget_slider_target BudgetTarget>
class budget_stratified_tax_income_text : public budget_scaled_monetary_value_text {
	void put_values(sys::state& state, std::array<float, size_t(budget_slider_target::target_count)>& vals) noexcept override {
		vals[uint8_t(BudgetTarget)] = economy::estimate_tax_income_by_strata(state, state.local_player_nation, Strata);
	}
};

template<culture::income_type IncomeType, budget_slider_target BudgetTarget>
class budget_expenditure_text : public budget_scaled_monetary_value_text {
public:
	void put_values(sys::state& state, std::array<float, size_t(budget_slider_target::target_count)>& vals) noexcept override {
		vals[uint8_t(BudgetTarget)] = economy::estimate_pop_payouts_by_income_type(state, state.local_player_nation, IncomeType);
	}
};

class budget_social_spending_text : public budget_scaled_monetary_value_text {
public:
	void put_values(sys::state& state, std::array<float, size_t(budget_slider_target::target_count)>& vals) noexcept override {
		vals[uint8_t(budget_slider_target::social)] = economy::estimate_social_spending(state, state.local_player_nation);
	}
};

class budget_income_projection_text : public budget_scaled_monetary_value_text {
public:
	void put_values(sys::state& state, std::array<float, size_t(budget_slider_target::target_count)>& vals) noexcept override {
		vals[uint8_t(budget_slider_target::poor_tax)] =
				economy::estimate_tax_income_by_strata(state, state.local_player_nation, culture::pop_strata::poor);
		vals[uint8_t(budget_slider_target::middle_tax)] =
				economy::estimate_tax_income_by_strata(state, state.local_player_nation, culture::pop_strata::middle);
		vals[uint8_t(budget_slider_target::rich_tax)] =
				economy::estimate_tax_income_by_strata(state, state.local_player_nation, culture::pop_strata::rich);
		vals[uint8_t(budget_slider_target::raw)] = economy::estimate_gold_income(state, state.local_player_nation);
	}
};

class budget_expenditure_projection_text : public budget_scaled_monetary_value_text {
public:
	void put_values(sys::state& state, std::array<float, size_t(budget_slider_target::target_count)>& vals) noexcept override {
		vals[uint8_t(budget_slider_target::construction_stock)] =
				economy::estimate_construction_spending(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::army_stock)] = economy::estimate_land_spending(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::navy_stock)] = economy::estimate_naval_spending(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::social)] = economy::estimate_social_spending(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::education)] =
				economy::estimate_pop_payouts_by_income_type(state, state.local_player_nation, culture::income_type::education);
		vals[uint8_t(budget_slider_target::admin)] =
				economy::estimate_pop_payouts_by_income_type(state, state.local_player_nation, culture::income_type::administration);
		vals[uint8_t(budget_slider_target::military)] =
				economy::estimate_pop_payouts_by_income_type(state, state.local_player_nation, culture::income_type::military);
		vals[uint8_t(budget_slider_target::raw)] = economy::estimate_loan_payments(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::raw)] += economy::estimate_subsidy_spending(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::raw)] += economy::estimate_overseas_penalty_spending(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::raw)] += economy::estimate_stockpile_filling_spending(state, state.local_player_nation);
	}
};

class budget_balance_projection_text : public budget_scaled_monetary_value_text {
public:
	void put_values(sys::state& state, std::array<float, size_t(budget_slider_target::target_count)>& vals) noexcept override {
		// income
		vals[uint8_t(budget_slider_target::poor_tax)] =
				economy::estimate_tax_income_by_strata(state, state.local_player_nation, culture::pop_strata::poor);
		vals[uint8_t(budget_slider_target::middle_tax)] =
				economy::estimate_tax_income_by_strata(state, state.local_player_nation, culture::pop_strata::middle);
		vals[uint8_t(budget_slider_target::rich_tax)] =
				economy::estimate_tax_income_by_strata(state, state.local_player_nation, culture::pop_strata::rich);
		vals[uint8_t(budget_slider_target::raw)] = economy::estimate_gold_income(state, state.local_player_nation);

		// spend
		vals[uint8_t(budget_slider_target::construction_stock)] =
				-economy::estimate_construction_spending(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::army_stock)] = -economy::estimate_land_spending(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::navy_stock)] = -economy::estimate_naval_spending(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::social)] = -economy::estimate_social_spending(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::education)] =
				-economy::estimate_pop_payouts_by_income_type(state, state.local_player_nation, culture::income_type::education);
		vals[uint8_t(budget_slider_target::admin)] =
				-economy::estimate_pop_payouts_by_income_type(state, state.local_player_nation, culture::income_type::administration);
		vals[uint8_t(budget_slider_target::military)] =
				-economy::estimate_pop_payouts_by_income_type(state, state.local_player_nation, culture::income_type::military);
		vals[uint8_t(budget_slider_target::raw)] += -economy::estimate_loan_payments(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::raw)] += -economy::estimate_subsidy_spending(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::raw)] += -economy::estimate_overseas_penalty_spending(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::raw)] += -economy::estimate_stockpile_filling_spending(state, state.local_player_nation);
		// balance
		vals[uint8_t(budget_slider_target::raw)] += economy::estimate_diplomatic_balance(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::tariffs)] = economy::estimate_tariff_income(state, state.local_player_nation);
	}
};

class budget_take_loan_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<bool>{bool{true}};
			parent->impl_get(state, payload);
		}
	}
};

// NOTE for simplicity sake we use a payload with bool{true} for taking loan window and a payload with bool{false} for repaying
// loan window

class budget_repay_loan_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<bool>{bool{false}};
			parent->impl_get(state, payload);
		}
	}
};

class budget_take_loan_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "bg") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "take_loan_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "ok") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "cancel") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "money_value") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} /*else if(name == "money_slider") {
			return nullptr;
		}*/
		else {
			return nullptr;
		}
	}
};

class budget_repay_loan_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "bg") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "repay_loan_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "ok") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "cancel") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "money_value") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} /*else if(name == "money_slider") {
			return nullptr;
		}*/
		else {
			return nullptr;
		}
	}
};

class tax_list_pop_type_icon : public opaque_element_base {
public:
	dcon::pop_type_id type{};

	void set_type(sys::state& state, dcon::pop_type_id t) {
		type = t;
		frame = int32_t(state.world.pop_type_get_sprite(t) - 1);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void on_update(sys::state& state) noexcept override {
		auto total_pop = state.world.nation_get_demographics(state.local_player_nation, demographics::to_key(state, type));
		disabled = total_pop < 1.0f;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto total = 0.f;
		std::array<float, 5> sat_pool = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
		for(auto prov : state.world.nation_get_province_ownership(state.local_player_nation)) {
			for(auto pop_loc : prov.get_province().get_pop_location()) {
				auto pop_id = pop_loc.get_pop();
				if(pop_id.get_poptype() == type) {
					auto pop_size = state.world.pop_get_size(pop_id);
					sat_pool[(pop_id.get_luxury_needs_satisfaction() > 0.95f)             ? 4
									 : (pop_id.get_everyday_needs_satisfaction() > 0.95f) ? 3
									 : (pop_id.get_life_needs_satisfaction() > 0.95f)     ? 2
									 : (pop_id.get_life_needs_satisfaction() > 0.01f)     ? 1
									 : 0] += pop_size;
					total += pop_size;
				}
			}
		}

	
			if(total > 0.0f) {
				auto type_strata = state.world.pop_type_get_strata(type);
				float total_pop = 0.0f;
				if(culture::pop_strata(type_strata) == culture::pop_strata::poor) {
					total_pop = state.world.nation_get_demographics(state.local_player_nation, demographics::poor_total);
				} else if(culture::pop_strata(type_strata) == culture::pop_strata::middle) {
					total_pop = state.world.nation_get_demographics(state.local_player_nation, demographics::middle_total);
				} else {
					total_pop = state.world.nation_get_demographics(state.local_player_nation, demographics::rich_total);
				}

				{
					auto box = text::open_layout_box(contents, 0);
					text::add_to_layout_box(state, contents, box, state.world.pop_type_get_name(type));
					text::close_layout_box(contents, box);
				}
				{
					auto box = text::open_layout_box(contents, 0);
					text::localised_single_sub_box(state, contents, box, std::string_view("percent_of_pop_strata"),
							text::variable_type::val, text::fp_percentage{total / total_pop});
					text::close_layout_box(contents, box);
					//percent_of_pop_strata
				}
				text::add_line_break_to_layout(state, contents);
				static const std::string needs_types[5] = {"no_need", "some_life_needs", "life_needs", "everyday_needs", "luxury_needs"};
				{
					auto box = text::open_layout_box(contents);
					auto sub = text::substitution_map{};
					auto needs_type = text::produce_simple_string(state, "no_need");
					text::add_to_substitution_map(sub, text::variable_type::val, text::fp_one_place{sat_pool[0] * 100.0f / total});
					text::add_to_substitution_map(sub, text::variable_type::type, std::string_view(needs_type));
					text::localised_format_box(state, contents, box, "budget_strata_no_need", sub);
					text::close_layout_box(contents, box);
				}
				{
					auto box = text::open_layout_box(contents);
					auto sub = text::substitution_map{};
					auto needs_type = text::produce_simple_string(state, "some_life_needs");
					text::add_to_substitution_map(sub, text::variable_type::val, text::fp_one_place{sat_pool[1] * 100.0f / total});
					text::add_to_substitution_map(sub, text::variable_type::type, std::string_view(needs_type));
					text::localised_format_box(state, contents, box, "budget_strata_need", sub);
					text::close_layout_box(contents, box);
				}
				{
					auto box = text::open_layout_box(contents);
					auto sub = text::substitution_map{};
					auto needs_type = text::produce_simple_string(state, "life_needs");
					text::add_to_substitution_map(sub, text::variable_type::val, text::fp_one_place{sat_pool[2] * 100.0f / total});
					text::add_to_substitution_map(sub, text::variable_type::type, std::string_view(needs_type));
					text::localised_format_box(state, contents, box, "budget_strata_need", sub);
					text::close_layout_box(contents, box);
				}
				{
					auto box = text::open_layout_box(contents);
					auto sub = text::substitution_map{};
					auto needs_type = text::produce_simple_string(state, "everyday_needs");
					text::add_to_substitution_map(sub, text::variable_type::val, text::fp_one_place{sat_pool[3] * 100.0f / total});
					text::add_to_substitution_map(sub, text::variable_type::type, std::string_view(needs_type));
					text::localised_format_box(state, contents, box, "budget_strata_need", sub);
					text::close_layout_box(contents, box);
				}
				{
					auto box = text::open_layout_box(contents);
					auto sub = text::substitution_map{};
					auto needs_type = text::produce_simple_string(state, "luxury_needs");
					text::add_to_substitution_map(sub, text::variable_type::val, text::fp_one_place{sat_pool[4] * 100.0f / total});
					text::add_to_substitution_map(sub, text::variable_type::type, std::string_view(needs_type));
					text::localised_format_box(state, contents, box, "budget_strata_need", sub);
					text::close_layout_box(contents, box);
				}
			} else {
				{
					auto box = text::open_layout_box(contents, 0);
					text::add_to_layout_box(state, contents, box, state.world.pop_type_get_name(type));
					text::close_layout_box(contents, box);
				}
				text::add_line_break_to_layout(state, contents);
				{
					auto box = text::open_layout_box(contents, 0);
					text::localised_format_box(state, contents, box, std::string_view("no_pops_of_type"));
					text::close_layout_box(contents, box);
				}
			}
		
	}
};

class budget_pop_list_item : public window_element_base {
private:
	tax_list_pop_type_icon* pop_type_icon = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "pop") {
			auto ptr = make_element_by_type<tax_list_pop_type_icon>(state, id);
			pop_type_icon = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<wrapped_listbox_row_content<dcon::pop_type_id>>()) {
			auto pop_type_id = any_cast<wrapped_listbox_row_content<dcon::pop_type_id>>(payload).content;
			pop_type_icon->set_type(state, pop_type_id);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

template<culture::pop_strata Strata>
class budget_pop_tax_list : public overlapping_listbox_element_base<budget_pop_list_item, dcon::pop_type_id> {
protected:
	std::string_view get_row_element_name() override {
		return "pop_listitem";
	}

public:
	void on_create(sys::state& state) noexcept override {
		overlapping_listbox_element_base<budget_pop_list_item, dcon::pop_type_id>::on_create(state);
		state.world.for_each_pop_type([&](dcon::pop_type_id pt) {
			if(state.world.pop_type_get_strata(pt) == uint8_t(Strata)) {
				row_contents.push_back(pt);
			}
		});
		update(state);
	}
};

template<culture::income_type Income>
class budget_pop_income_list : public overlapping_listbox_element_base<budget_pop_list_item, dcon::pop_type_id> {
protected:
	std::string_view get_row_element_name() override {
		return "pop_listitem";
	}

public:
	void on_create(sys::state& state) noexcept override {
		overlapping_listbox_element_base<budget_pop_list_item, dcon::pop_type_id>::on_create(state);
		state.world.for_each_pop_type([&](dcon::pop_type_id pt) {
			if(state.world.pop_type_get_life_needs_income_type(pt) == uint8_t(Income) ||
					state.world.pop_type_get_everyday_needs_income_type(pt) == uint8_t(Income) ||
					state.world.pop_type_get_luxury_needs_income_type(pt) == uint8_t(Income)) {
				row_contents.push_back(pt);
			}
		});
		update(state);
	}
};

template<culture::income_type Income>
class budget_small_pop_income_list : public budget_pop_income_list<Income> {
protected:
	std::string_view get_row_element_name() override {
		return "pop_listitem_small";
	}
};

class budget_tariff_percentage_text : public simple_text_element_base {
public:
	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<budget_slider_signal>()) {
			auto sig = any_cast<budget_slider_signal>(payload);
			if(sig.target == budget_slider_target::tariffs) {
				set_text(state, text::format_percentage(sig.amount));
			}
			return message_result::consumed;
		}
		return message_result::unseen;
	}

	void on_update(sys::state& state) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		set_text(state, text::format_percentage(float(state.world.nation_get_tariffs(nation_id)) / 100.0f));
	}
};

class budget_window : public window_element_base {
private:
	budget_take_loan_window* budget_take_loan_win = nullptr;
	budget_repay_loan_window* budget_repay_loan_win = nullptr;

public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		auto win1337 = make_element_by_type<budget_take_loan_window>(state,
				state.ui_state.defs_by_name.find("take_loan_window")->second.definition);
		budget_take_loan_win = win1337.get();
		win1337->base_data.position.y -= 66; // Nudge >w<
		win1337->set_visible(state, false);
		add_child_to_front(std::move(win1337));

		auto win101 = make_element_by_type<budget_repay_loan_window>(state,
				state.ui_state.defs_by_name.find("repay_loan_window")->second.definition);
		budget_repay_loan_win = win101.get();
		win101->base_data.position.y -= 66; // Nudge >w<
		win101->set_visible(state, false);
		add_child_to_front(std::move(win101));

		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "tariffs_percent") {
			return make_element_by_type<budget_tariff_percentage_text>(state, id);
		} else if(name == "total_funds_val") {
			return make_element_by_type<nation_budget_funds_text>(state, id);
		} else if(name == "national_bank_val") {
			return make_element_by_type<nation_budget_bank_text>(state, id);
		} else if(name == "debt_val") {
			return make_element_by_type<nation_budget_debt_text>(state, id);
		} else if(name == "chart_0") {
			return make_element_by_type<pop_satisfaction_piechart<culture::pop_strata::poor>>(state, id);
		} else if(name == "chart_1") {
			return make_element_by_type<pop_satisfaction_piechart<culture::pop_strata::middle>>(state, id);
		} else if(name == "chart_2") {
			return make_element_by_type<pop_satisfaction_piechart<culture::pop_strata::rich>>(state, id);
		} else if(name == "nat_stock_val") {
			return make_element_by_type<nation_actual_stockpile_spending_text>(state, id);
		} else if(name == "nat_stock_est") {
			return make_element_by_type<budget_estimated_stockpile_spending_text>(state, id);
		} else if(name == "mil_cost_val") {
			return make_element_by_type<budget_military_spending_text>(state, id);
		} else if(name == "overseas_cost_val") {
			return make_element_by_type<budget_overseas_spending_text>(state, id);
		} else if(name == "tariff_val") {
			return make_element_by_type<budget_tariff_income_text>(state, id);
		} else if(name == "gold_inc") {
			return make_element_by_type<nation_gold_income_text>(state, id);
		} else if(name == "tax_0_inc") {
			return make_element_by_type< budget_stratified_tax_income_text<culture::pop_strata::poor, budget_slider_target::poor_tax>>(
					state, id);
		} else if(name == "tax_1_inc") {
			return make_element_by_type<
					budget_stratified_tax_income_text<culture::pop_strata::middle, budget_slider_target::middle_tax>>(state, id);
		} else if(name == "tax_2_inc") {
			return make_element_by_type< budget_stratified_tax_income_text<culture::pop_strata::rich, budget_slider_target::rich_tax>>(
					state, id);
		} else if(name == "exp_val_0") {
			return make_element_by_type< budget_expenditure_text<culture::income_type::education, budget_slider_target::education>>(
					state, id);
		} else if(name == "exp_val_1") {
			return make_element_by_type< budget_expenditure_text<culture::income_type::administration, budget_slider_target::admin>>(
					state, id);
		} else if(name == "exp_val_2") {
			return make_element_by_type<budget_social_spending_text>(state, id);
		} else if(name == "exp_val_3") {
			return make_element_by_type<budget_expenditure_text<culture::income_type::military, budget_slider_target::military>>(state,
					id);
		} else if(name == "admin_efficiency") {
			return make_element_by_type<nation_administrative_efficiency_text>(state, id);
		} else if(name == "interest_val") {
			return make_element_by_type<nation_loan_spending_text>(state, id);
		} else if(name == "ind_sub_val") {
			return make_element_by_type<nation_subsidy_spending_text>(state, id);
		} else if(name == "diplomatic_balance") {
			return make_element_by_type<nation_diplomatic_balance_text>(state, id);
		} else if(name == "total_inc") {
			return make_element_by_type<budget_income_projection_text>(state, id);
		} else if(name == "total_exp") {
			return make_element_by_type<budget_expenditure_projection_text>(state, id);
		} else if(name == "balance") {
			auto ptr = make_element_by_type<budget_balance_projection_text>(state, id);
			ptr->color_result = true;
			return ptr;
		} else if(name == "tax_0_slider") {
			return make_element_by_type<budget_poor_tax_slider>(state, id);
		} else if(name == "tax_1_slider") {
			return make_element_by_type<budget_middle_tax_slider>(state, id);
		} else if(name == "tax_2_slider") {
			return make_element_by_type<budget_rich_tax_slider>(state, id);
		} else if(name == "land_stockpile_slider") {
			return make_element_by_type<budget_army_stockpile_slider>(state, id);
		} else if(name == "naval_stockpile_slider") {
			return make_element_by_type<budget_navy_stockpile_slider>(state, id);
		} else if(name == "projects_stockpile_slider") {
			return make_element_by_type<budget_construction_stockpile_slider>(state, id);
		} else if(name == "exp_0_slider") {
			return make_element_by_type<budget_education_slider>(state, id);
		} else if(name == "exp_1_slider") {
			return make_element_by_type<budget_administration_slider>(state, id);
		} else if(name == "exp_2_slider") {
			return make_element_by_type<budget_social_spending_slider>(state, id);
		} else if(name == "exp_3_slider") {
			return make_element_by_type<budget_military_spending_slider>(state, id);
		} else if(name == "tariff_slider") {
			return make_element_by_type<budget_tariff_slider>(state, id);
		} else if(name == "take_loan") {
			return make_element_by_type<budget_take_loan_button>(state, id);
		} else if(name == "repay_loan") {
			return make_element_by_type<budget_repay_loan_button>(state, id);
		} else if(name == "tax_0_pops") {
			return make_element_by_type<budget_pop_tax_list<culture::pop_strata::poor>>(state, id);
		} else if(name == "tax_1_pops") {
			return make_element_by_type<budget_pop_tax_list<culture::pop_strata::middle>>(state, id);
		} else if(name == "tax_2_pops") {
			return make_element_by_type<budget_pop_tax_list<culture::pop_strata::rich>>(state, id);
		} else if(name == "exp_0_pops") {
			return make_element_by_type<budget_small_pop_income_list<culture::income_type::education>>(state, id);
		} else if(name == "exp_1_pops") {
			return make_element_by_type<budget_small_pop_income_list<culture::income_type::administration>>(state, id);
			// } else if(name == "exp_2_pops") {   // intentionally unused
			// 	return make_element_by_type<budget_pop_income_list<culture::income_type::reforms>>(state, id);
		} else if(name == "exp_3_pops") {
			return make_element_by_type<budget_pop_income_list<culture::income_type::military>>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(state.local_player_nation);
			return message_result::consumed;
		}
		//=====================================================================
		else if(payload.holds_type<element_selection_wrapper<bool>>()) {
			bool type = any_cast<element_selection_wrapper<bool>>(payload).data;
			if(type) { // Take Loan Win.
				budget_take_loan_win->set_visible(state, true);
				move_child_to_front(budget_take_loan_win);
			} else { // Repay Loan Win.
				budget_repay_loan_win->set_visible(state, true);
				move_child_to_front(budget_repay_loan_win);
			}
			return message_result::consumed;
		}

		return message_result::unseen;
	}
};

} // namespace ui
