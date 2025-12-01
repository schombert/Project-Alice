#pragma once

#include "container_types_dcon.hpp"
#include "dcon_generated_ids.hpp"
#include "system_state_forward.hpp"
#include "constants_dcon.hpp"

namespace economy {

enum commodity_production_type {
	primary,
	derivative,
	both
};


enum class worker_effect : uint8_t { none = 0, input, output, throughput };

template<typename T>
auto desired_needs_spending([[maybe_unused]] sys::state const& state, [[maybe_unused]] T pop_indices) {
	// TODO: gather pop types, extract cached needs sum, etc etc
	return 0.0f;
}



inline constexpr float production_scale_delta = 0.1f;
inline constexpr float factory_closed_threshold = 0.0001f;
inline constexpr uint32_t price_history_length = 256;
inline constexpr uint32_t gdp_history_length = 128;

void presimulate(sys::state& state);
void sanity_check(sys::state& state);

float subsistence_max_pseudoemployment(sys::state& state, dcon::province_id p);
float factory_total_employment_score(sys::state const& state, dcon::factory_id f);

bool has_building(sys::state const& state, dcon::state_instance_id si, dcon::factory_type_id fac);
bool is_bankrupt_debtor_to(sys::state& state, dcon::nation_id debt_holder, dcon::nation_id debtor);

// misc
int32_t factory_priority(sys::state const& state, dcon::factory_id f);
void set_factory_priority(sys::state& state, dcon::factory_id f, int32_t priority);
bool factory_is_profitable(sys::state const& state, dcon::factory_id f);

bool nation_is_constructing_factories(sys::state& state, dcon::nation_id n);
bool nation_has_closed_factories(sys::state& state, dcon::nation_id n);

dcon::unilateral_relationship_id nation_gives_free_trade_rights(sys::state& state, dcon::nation_id source, dcon::nation_id target);
dcon::unilateral_relationship_id nation_gives_direct_free_trade_rights(sys::state& state, dcon::nation_id source, dcon::nation_id target);

void initialize(sys::state& state);
void regenerate_unsaved_values(sys::state& state);

float pop_min_wage_factor(sys::state& state, dcon::nation_id n);
float farmer_min_wage(sys::state& state, dcon::market_id m, float min_wage_factor);
float laborer_min_wage(sys::state& state, dcon::market_id m, float min_wage_factor);

void daily_update(sys::state& state, bool presimulation, float presimulation_stage);
void resolve_constructions(sys::state& state);

std::vector<dcon::factory_type_id> commodity_get_factory_types_as_output(sys::state const& state, dcon::commodity_id output_good);

float government_consumption(sys::state& state, dcon::nation_id n, dcon::commodity_id c);
float nation_pop_consumption(sys::state& state, dcon::nation_id n, dcon::commodity_id c);
float nation_total_imports(sys::state& state, dcon::nation_id n);

float estimate_gold_income(sys::state& state, dcon::nation_id n);
float estimate_tariff_import_income(sys::state& state, dcon::nation_id n);
float estimate_tariff_export_income(sys::state& state, dcon::nation_id n);
float estimate_education_spending(sys::state& state, dcon::nation_id n);
float estimate_subsidy_spending(sys::state& state, dcon::nation_id n);
float estimate_diplomatic_balance(sys::state& state, dcon::nation_id n);
float estimate_diplomatic_income(sys::state& state, dcon::nation_id n);
float estimate_diplomatic_expenses(sys::state& state, dcon::nation_id n);
float estimate_max_domestic_investment(sys::state& state, dcon::nation_id n);
float estimate_current_domestic_investment(sys::state& state, dcon::nation_id n);

float estimate_land_spending(sys::state& state, dcon::nation_id n);
float estimate_naval_spending(sys::state& state, dcon::nation_id n);
float estimate_war_subsidies_spending(sys::state& state, dcon::nation_id n);
float estimate_reparations_spending(sys::state& state, dcon::nation_id n);
float estimate_war_subsidies_income(sys::state& state, dcon::nation_id n);
float estimate_reparations_income(sys::state& state, dcon::nation_id n);
float estimate_overseas_penalty_spending(sys::state& state, dcon::nation_id n);
float estimate_stockpile_filling_spending(sys::state& state, dcon::nation_id n);

struct full_construction_factory {
	float cost = 0.0f;
	dcon::nation_id nation;
	dcon::province_id province;
	bool is_pop_project = false;
	bool is_upgrade = false;
	dcon::factory_type_id type;
};

struct full_construction_province {
	float cost = 0.0f;
	dcon::nation_id nation;
	dcon::province_id province;
	bool is_pop_project = false;
	province_building_type type = province_building_type::railroad;
};

std::vector<full_construction_factory> estimate_private_investment_upgrade(sys::state& state, dcon::nation_id nid, float est_private_const_spending);
std::vector<full_construction_factory> estimate_private_investment_construct(sys::state& state, dcon::nation_id nid, bool craved, float est_private_const_spending, bool& potential_target_exists);
std::vector<full_construction_province> estimate_private_investment_province(sys::state& state, dcon::nation_id nid, float est_private_const_spending);

// NOTE: used to estimate how much you will pay if you were to subsidize a particular nation,
// *not* how much you are paying at the moment
float estimate_war_subsidies(sys::state& state, dcon::nation_id target, dcon::nation_id source);

float estimate_subject_payments_paid(sys::state& state, dcon::nation_id n);
float estimate_subject_payments_received(sys::state& state, dcon::nation_id o);

float estimate_daily_income_ai(sys::state& state, dcon::nation_id n);
float estimate_daily_income(sys::state& state, dcon::nation_id n);

construction_status province_building_construction(sys::state& state, dcon::province_id, province_building_type t);
construction_status factory_upgrade(sys::state& state, dcon::factory_id f);



// Represents one type of employment of a building
struct employment_record {
	int32_t employment_type;
	float target_employment;
	float satisfaction;
	float actual_employment;
};

float unit_construction_progress(sys::state& state, dcon::province_land_construction_id c);
float unit_construction_progress(sys::state& state, dcon::province_naval_construction_id c);
void try_add_factory_to_state(sys::state& state, dcon::state_instance_id s, dcon::factory_type_id t);
void bound_budget_settings(sys::state& state, dcon::nation_id n);

int32_t most_recent_price_record_index(sys::state& state);
int32_t previous_price_record_index(sys::state& state);
int32_t most_recent_gdp_record_index(sys::state& state);
int32_t previous_gdp_record_index(sys::state& state);

void prune_factories(sys::state& state); // get rid of closed factories in full states
void go_bankrupt(sys::state& state, dcon::nation_id n);
dcon::modifier_id get_province_selector_modifier(sys::state& state);
dcon::modifier_id get_province_immigrator_modifier(sys::state& state);

bool can_take_loans(sys::state& state, dcon::nation_id n);
float interest_payment(sys::state& state, dcon::nation_id n);
float max_loan(sys::state& state, dcon::nation_id n);

float estimate_investment_pool_daily_loss(sys::state& state, dcon::nation_id n);

bool get_commodity_uses_potentials(sys::state& state, dcon::commodity_id c);
float calculate_province_factory_limit(sys::state& state, dcon::province_id pid, dcon::commodity_id c);
float calculate_state_factory_limit(sys::state& state, dcon::state_instance_id sid, dcon::commodity_id c);
float calculate_nation_factory_limit(sys::state& state, dcon::nation_id nid, dcon::commodity_id c);

bool do_resource_potentials_allow_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::factory_type_id type);
bool do_resource_potentials_allow_upgrade(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::factory_type_id type);
bool do_resource_potentials_allow_refit(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::factory_type_id from, dcon::factory_type_id refit_target);

} // namespace economy
