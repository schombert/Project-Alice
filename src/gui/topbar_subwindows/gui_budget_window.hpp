#pragma once

#include "commands.hpp"
#include "culture.hpp"
#include "dcon_generated.hpp"
#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"
#include "nations.hpp"
#include "system_state.hpp"
#include <cstdint>
#include <vector>

namespace dcon {
class pop_satisfaction_wrapper_id {
public:
	using value_base_t = uint8_t;
	uint8_t value = 0;
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
	uint32_t get_color() noexcept {
		switch(value) {
		case 0: // red
			return sys::pack_color(0.9f, 0.2f, 0.1f);
		case 1: // yellow
			return sys::pack_color(0.9f, 0.9f, 0.1f);
		case 2: // green
			return sys::pack_color(0.2f, 0.95f, 0.2f);
		case 3: // blue
			return sys::pack_color(0.5f, 0.5f, 1.0f);
		case 4: // light blue
			return sys::pack_color(0.2f, 0.2f, 0.8f);
		}
		return 0;
	}
	void set_name(text_sequence_id text) noexcept {
		names[value] = text;
	}
	text_sequence_id get_name() noexcept {
		switch(value) {
		case 0: // No needs fullfilled
		case 1: // Some life needs
		case 2: // All life needs
		case 3: // All everyday
		case 4: // All luxury
			return names[value];
		}
		return text_sequence_id{ 0 };
	}
};
pop_satisfaction_wrapper_fat fatten(data_container const& c, pop_satisfaction_wrapper_id id) noexcept {
	return pop_satisfaction_wrapper_fat{ id.value };
}
}
namespace ogl {
template<>
uint32_t get_ui_color(sys::state& state, dcon::pop_satisfaction_wrapper_id id){
	return ogl::color_from_hash(uint32_t(id.index()));
}
}

namespace ui {
template<culture::pop_strata Strata>
class pop_satisfaction_piechart : public piechart<dcon::pop_satisfaction_wrapper_id> {
protected:
	std::unordered_map<dcon::pop_satisfaction_wrapper_id::value_base_t, float> get_distribution(sys::state& state) noexcept override {
		std::unordered_map<dcon::pop_satisfaction_wrapper_id::value_base_t, float> distrib = {};
		Cyto::Any nat_id_payload = dcon::nation_id{};

		enabled = true;
		if(parent == nullptr)
			return distrib;

		parent->impl_get(state, nat_id_payload);
		if(nat_id_payload.holds_type<dcon::nation_id>()) {
			auto nat_id = any_cast<dcon::nation_id>(nat_id_payload);
			auto total = 0.f;
			auto sat_pool = std::vector<float>(5);
			state.world.for_each_province([&](dcon::province_id province) {
				if(nat_id != state.world.province_get_nation_from_province_ownership(province))
					return;

				for(auto pop_loc : state.world.province_get_pop_location(province)) {
					auto pop_id = pop_loc.get_pop();
					auto pop_strata = state.world.pop_type_get_strata(state.world.pop_get_poptype(pop_id));
					auto pop_size = pop_strata == uint8_t(Strata) ? state.world.pop_get_size(pop_id) : 0.f;
					// All luxury needs
					// OR All everyday needs
					// OR All life needs
					// OR Some life needs
					// OR No needs fullfilled...
					sat_pool[(pop_id.get_luxury_needs_satisfaction() > 0.f)
						? 4
						: (pop_id.get_everyday_needs_satisfaction() > 0.f)
							? 3
							: (pop_id.get_life_needs_satisfaction() >= 1.f)
								? 2
								: (pop_id.get_life_needs_satisfaction() > 0.f)
									? 1
									: 0] += pop_size;
					total += pop_size;
				}
			});
			if(total <= 0.f) {
				enabled = false;
				return distrib;
			}

			for(size_t i = 0; i < sat_pool.size(); i++)
				distrib[dcon::pop_satisfaction_wrapper_id::value_base_t(i)] = sat_pool[i] / total;
		}
		return distrib;
	}
public:
	void on_create(sys::state& state) noexcept override {
		// Fill-in static information...
		dcon::fatten(state.world, dcon::pop_satisfaction_wrapper_id{ 0 }).set_name(text::find_or_add_key(state, "BUDGET_STRATA_NO_NEED"));
		dcon::fatten(state.world, dcon::pop_satisfaction_wrapper_id{ 1 }).set_name(text::find_or_add_key(state, "BUDGET_STRATA_NEED"));
		dcon::fatten(state.world, dcon::pop_satisfaction_wrapper_id{ 2 }).set_name(text::find_or_add_key(state, "BUDGET_STRATA_NEED"));
		dcon::fatten(state.world, dcon::pop_satisfaction_wrapper_id{ 3 }).set_name(text::find_or_add_key(state, "BUDGET_STRATA_NEED"));
		dcon::fatten(state.world, dcon::pop_satisfaction_wrapper_id{ 4 }).set_name(text::find_or_add_key(state, "BUDGET_STRATA_NEED"));

		piechart::on_create(state);
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
	float amount = 0.0f;
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
		int32_t v = 0;
		if(state.ui_state.drag_target == slider) {
			v = int32_t(scaled_value());
		} else {
			v = get_true_value(state);
			update_raw_value(state, v);
		}
		if(parent) {
			float amount = float(v) / 100.f;
			Cyto::Any payload = budget_slider_signal{ SliderTarget, amount };
			parent->impl_set(state, payload);
		}
	}

	virtual int32_t get_true_value(sys::state& state) noexcept {
		return 0;
	}

	void on_drag_finish(sys::state & state) noexcept override {
		commit_changes(state);
	}

private:
	void commit_changes(sys::state& state) noexcept {
		auto budget_settings = command::make_empty_budget_settings();
		update_budget_settings(budget_settings);
		command::change_budget_settings(state, state.local_player_nation, budget_settings);
	}

	void update_budget_settings(command::budget_settings_data &budget_settings) noexcept {
		auto new_val = int8_t(scaled_value());
		switch (SliderTarget) {
			case budget_slider_target::poor_tax:
				budget_settings.poor_tax = new_val; break;
			case budget_slider_target::middle_tax:
				budget_settings.middle_tax = new_val; break;
			case budget_slider_target::rich_tax:
				budget_settings.rich_tax = new_val; break;
			case budget_slider_target::army_stock:
				budget_settings.land_spending = new_val; break;
			case budget_slider_target::navy_stock:
				budget_settings.naval_spending = new_val; break;
			case budget_slider_target::construction_stock:
				budget_settings.construction_spending = new_val; break;
			case budget_slider_target::education:
				budget_settings.education_spending = new_val; break;
			case budget_slider_target::admin:
				budget_settings.administrative_spending = new_val; break;
			case budget_slider_target::social:
				budget_settings.social_spending = new_val; break;
			case budget_slider_target::military:
				budget_settings.military_spending = new_val; break;
			case budget_slider_target::tariffs:
				budget_settings.tariffs = new_val; break;
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

class budget_scaled_monetary_value_text : public standard_nation_text {
private:
	std::array<float, size_t(budget_slider_target::target_count)> values;
	std::array<float, size_t(budget_slider_target::target_count)> multipliers;

public:
	virtual void put_values(sys::state& state, std::array<float, size_t(budget_slider_target::target_count)>& vals) noexcept { }

	void on_create(sys::state& state) noexcept override {
		standard_nation_text::on_create(state);
		for(uint8_t i = 0; i < uint8_t(budget_slider_target::target_count); ++i) {
			values[i] = 0.f;
			multipliers[i] = 1.f;
		}
	}

	void apply_multipliers(sys::state& state) noexcept {
		auto total = 0.f;
		for(uint8_t i = 0; i < uint8_t(budget_slider_target::target_count); ++i)
			total += values[i] * multipliers[i];
		set_text(state, text::format_money(total));
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
		} else {
			return standard_nation_text::set(state, payload);
		}
	}
};

class budget_estimated_stockpile_spending_text : public budget_scaled_monetary_value_text {
public:
	void put_values(sys::state& state, std::array<float, size_t(budget_slider_target::target_count)>& vals) noexcept override {
		vals[uint8_t(budget_slider_target::construction_stock)] = economy::estimate_construction_spending(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::army_stock)] = economy::estimate_land_spending(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::navy_stock)] = economy::estimate_naval_spending(state, state.local_player_nation);
	}
};

class budget_army_spending_text : public budget_scaled_monetary_value_text {
public:
	void put_values(sys::state& state, std::array<float, size_t(budget_slider_target::target_count)>& vals) noexcept override {
		vals[uint8_t(budget_slider_target::army_stock)] = economy::estimate_land_spending(state, state.local_player_nation);
	}
};

class budget_naval_spending_text : public budget_scaled_monetary_value_text {
public:
	void put_values(sys::state& state, std::array<float, size_t(budget_slider_target::target_count)>& vals) noexcept override {
		vals[uint8_t(budget_slider_target::navy_stock)] = economy::estimate_naval_spending(state, state.local_player_nation);
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
		vals[uint8_t(budget_slider_target::poor_tax)] = economy::estimate_tax_income_by_strata(state, state.local_player_nation, culture::pop_strata::poor);
		vals[uint8_t(budget_slider_target::middle_tax)] = economy::estimate_tax_income_by_strata(state, state.local_player_nation, culture::pop_strata::middle);
		vals[uint8_t(budget_slider_target::rich_tax)] = economy::estimate_tax_income_by_strata(state, state.local_player_nation, culture::pop_strata::rich);
		vals[uint8_t(budget_slider_target::raw)] = economy::estimate_gold_income(state, state.local_player_nation);
	}
};

class budget_expenditure_projection_text : public budget_scaled_monetary_value_text {
public:
	void put_values(sys::state& state, std::array<float, size_t(budget_slider_target::target_count)>& vals) noexcept override {
		vals[uint8_t(budget_slider_target::construction_stock)] = economy::estimate_construction_spending(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::army_stock)] = economy::estimate_land_spending(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::navy_stock)] = economy::estimate_naval_spending(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::social)] = economy::estimate_social_spending(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::education)] = economy::estimate_pop_payouts_by_income_type(state, state.local_player_nation, culture::income_type::education);
		vals[uint8_t(budget_slider_target::admin)] = economy::estimate_pop_payouts_by_income_type(state, state.local_player_nation, culture::income_type::administration);
		vals[uint8_t(budget_slider_target::military)] = economy::estimate_pop_payouts_by_income_type(state, state.local_player_nation, culture::income_type::military);
		vals[uint8_t(budget_slider_target::raw)] = economy::estimate_loan_payments(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::raw)] += economy::estimate_subsidy_spending(state, state.local_player_nation);
	}
};

class budget_balance_projection_text : public budget_scaled_monetary_value_text {
public:
	void put_values(sys::state& state, std::array<float, size_t(budget_slider_target::target_count)>& vals) noexcept override {
		// income
		vals[uint8_t(budget_slider_target::poor_tax)] = economy::estimate_tax_income_by_strata(state, state.local_player_nation, culture::pop_strata::poor);
		vals[uint8_t(budget_slider_target::middle_tax)] = economy::estimate_tax_income_by_strata(state, state.local_player_nation, culture::pop_strata::middle);
		vals[uint8_t(budget_slider_target::rich_tax)] = economy::estimate_tax_income_by_strata(state, state.local_player_nation, culture::pop_strata::rich);
		vals[uint8_t(budget_slider_target::raw)] = economy::estimate_gold_income(state, state.local_player_nation);

		// spend
		vals[uint8_t(budget_slider_target::construction_stock)] = -economy::estimate_construction_spending(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::army_stock)] = -economy::estimate_land_spending(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::navy_stock)] = -economy::estimate_naval_spending(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::social)] = -economy::estimate_social_spending(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::education)] = -economy::estimate_pop_payouts_by_income_type(state, state.local_player_nation, culture::income_type::education);
		vals[uint8_t(budget_slider_target::admin)] = -economy::estimate_pop_payouts_by_income_type(state, state.local_player_nation, culture::income_type::administration);
		vals[uint8_t(budget_slider_target::military)] = -economy::estimate_pop_payouts_by_income_type(state, state.local_player_nation, culture::income_type::military);
		vals[uint8_t(budget_slider_target::raw)] += -economy::estimate_loan_payments(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::raw)] += -economy::estimate_subsidy_spending(state, state.local_player_nation);

		// balance
		vals[uint8_t(budget_slider_target::raw)] += economy::estimate_diplomatic_balance(state, state.local_player_nation);
		vals[uint8_t(budget_slider_target::tariffs)] = economy::estimate_tariff_income(state, state.local_player_nation);
	}
};

class budget_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "tariffs_percent") {
			return make_element_by_type<nation_tariff_percentage_text>(state, id);
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
			return make_element_by_type<budget_army_spending_text>(state, id);
		} else if(name == "overseas_cost_val") {
			return make_element_by_type<budget_naval_spending_text>(state, id);
		} else if(name == "tariff_val") {
			return make_element_by_type<budget_tariff_income_text>(state, id);
		} else if(name == "gold_inc") {
			return make_element_by_type<nation_gold_income_text>(state, id);
		} else if(name == "tax_0_inc") {
			return make_element_by_type<budget_stratified_tax_income_text<culture::pop_strata::poor, budget_slider_target::poor_tax>>(state, id);
		} else if(name == "tax_1_inc") {
			return make_element_by_type<budget_stratified_tax_income_text<culture::pop_strata::middle, budget_slider_target::middle_tax>>(state, id);
		} else if(name == "tax_2_inc") {
			return make_element_by_type<budget_stratified_tax_income_text<culture::pop_strata::rich, budget_slider_target::rich_tax>>(state, id);
		} else if(name == "exp_val_0") {
			return make_element_by_type<budget_expenditure_text<culture::income_type::education, budget_slider_target::education>>(state, id);
		} else if(name == "exp_val_1") {
			return make_element_by_type<budget_expenditure_text<culture::income_type::administration, budget_slider_target::admin>>(state, id);
		} else if(name == "exp_val_2") {
			return make_element_by_type<budget_social_spending_text>(state, id);
		} else if(name == "exp_val_3") {
			return make_element_by_type<budget_expenditure_text<culture::income_type::military, budget_slider_target::military>>(state, id);
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
			return make_element_by_type<budget_balance_projection_text>(state, id);
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

}
