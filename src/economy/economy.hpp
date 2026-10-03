#pragma once

#include "container_types_dcon.hpp"
#include "dcon_generated_ids.hpp"
#include "system_state_forward.hpp"
#include "constants_dcon.hpp"
#include "economy_constants.hpp"
#include "economy_common_api_containers.hpp"
#include "concept_declarations.hpp"

namespace economy {

template<typename T>
auto desired_needs_spending([[maybe_unused]] sys::state const& state, [[maybe_unused]] T pop_indices) {
	// TODO: gather pop types, extract cached needs sum, etc etc
	return 0.0f;
}

struct total_stockpile_spendings {
	float army_stockpile_spendings = 0.0f;
	float navy_stockpile_spendings = 0.0f;
	float construction_spendings = 0.0f;
	float stockpile_filling_spendings = 0.0f;
};
struct total_stockpile_spendings_by_commodity {
	tagged_vector<float, dcon::unit_supply_and_build_commodity_id> army_stockpile_spendings{ };
	tagged_vector<float, dcon::unit_supply_and_build_commodity_id> navy_stockpile_spendings{ };
	tagged_vector<float, dcon::commodity_id> construction_spendings{ };
	tagged_vector<float, dcon::commodity_id> stockpile_filling_spendings{ };
};


dcon::province_id construction_get_location(const sys::state& state, dcon::province_land_construction_id con);
dcon::province_id construction_get_location(const sys::state& state, dcon::province_naval_construction_id con);
dcon::province_id construction_get_location(const sys::state& state, dcon::province_building_construction_id con);
dcon::province_id construction_get_location(const sys::state& state, dcon::factory_construction_id con);

dcon::unit_type_id construction_get_type(const sys::state& state, dcon::province_land_construction_id con);
dcon::unit_type_id construction_get_type(const sys::state& state, dcon::province_naval_construction_id con);
uint8_t construction_get_type(const sys::state& state, dcon::province_building_construction_id con);
dcon::factory_type_id construction_get_type(const sys::state& state, dcon::factory_construction_id con);


dcon::nation_id construction_get_controller(const sys::state& state, dcon::province_land_construction_id con);
dcon::nation_id construction_get_controller(const sys::state& state, dcon::province_naval_construction_id con);
dcon::nation_id construction_get_controller(const sys::state& state, dcon::province_building_construction_id con);
dcon::nation_id construction_get_controller(const sys::state& state, dcon::factory_construction_id con);

dcon::commodity_id unit_commodity_get_base_commodity(const sys::state& state, dcon::unit_supply_commodity_id com_id);
dcon::commodity_id unit_commodity_get_base_commodity(const sys::state& state, dcon::unit_build_commodity_id com_id);
dcon::commodity_id unit_commodity_get_base_commodity(const sys::state& state, dcon::unit_supply_and_build_commodity_id com_id);


void presimulate(sys::state& state);
void sanity_check(sys::state& state);

float subsistence_max_pseudoemployment(sys::state& state, dcon::province_id p);

bool has_building(sys::state const& state, dcon::state_instance_id si, dcon::factory_type_id fac);
bool is_bankrupt_debtor_to(sys::state& state, dcon::nation_id debt_holder, dcon::nation_id debtor);

// Adds the given amount ONLY to the total_stockpiles cache
void add_total_govt_stockpile(sys::state& state, dcon::nation_id controller, dcon::commodity_id commodity, float amount);
// Subtracts the given amount ONLY to the total_stockpiles cache
void subtract_total_govt_stockpile(sys::state& state, dcon::nation_id controller, dcon::commodity_id commodity, float amount);

// Adds the given amount to the government stockpile in the market controlled by the controller
void add_government_stockpile(sys::state& state, dcon::nation_id controller, dcon::market_id market, dcon::commodity_id commodity, float amount);
// Subtracts the given amount to the government stockpile in the market controlled by the controller (expects a positive number)
void subtract_government_stockpile(sys::state& state, dcon::nation_id controller, dcon::market_id market, dcon::commodity_id commodity, float amount);


// Works just like add_government_stockpile, but assumes the market is controlled by rebels
void add_rebel_stockpile(sys::state& state, dcon::market_id market, dcon::commodity_id commodity, float amount);
// Works just like subtract_government_stockpile, but assumes the market is controlled by rebels
void subtract_rebel_stockpile(sys::state& state, dcon::market_id market, dcon::commodity_id commodity, float amount);

template<price_estimation price_est, concepts::any_commodity_type com_type>
float get_estimated_stockpile_total_purchase_price(const sys::state& state, dcon::nation_id for_nation, const tagged_vector<float, com_type>& goods);

template<price_estimation price_est, concepts::any_commodity_type com_type>
tagged_vector<float, com_type> get_estimated_stockpile_purchase_price_by_commodity(const sys::state& state, dcon::nation_id for_nation, const tagged_vector<float, com_type>& goods);

total_stockpile_spendings estimate_total_stockpile_spendings(const sys::state& state, dcon::nation_id nation_as, float military_construction_budget, float stockpile_filling_budget, float army_budget, float navy_budget);
total_stockpile_spendings_by_commodity estimate_total_stockpile_spendings_by_commodity(const sys::state& state, dcon::nation_id nation_as, float military_construction_budget, float stockpile_filling_budget, float army_supplies_budget, float navy_supplies_budget);

tagged_vector<float, dcon::unit_supply_and_build_commodity_id> estimate_nation_army_consumption(const sys::state& state, dcon::nation_id nation);
tagged_vector<float, dcon::unit_supply_and_build_commodity_id> estimate_nation_navy_consumption(const sys::state& state, dcon::nation_id nation);
tagged_vector<float, dcon::unit_supply_and_build_commodity_id> estimate_nation_army_and_navy_consumption(const sys::state& state, dcon::nation_id nation);

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

float estimate_today_land_spending(sys::state& state, dcon::nation_id n);
float estimate_today_naval_spending(sys::state& state, dcon::nation_id n);
float estimate_war_subsidies_spending(sys::state& state, dcon::nation_id n);
float estimate_reparations_spending(sys::state& state, dcon::nation_id n);
float estimate_war_subsidies_income(sys::state& state, dcon::nation_id n);
float estimate_reparations_income(sys::state& state, dcon::nation_id n);
float estimate_overseas_penalty_spending(sys::state& state, dcon::nation_id n);

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

struct consume_stockpile_result {
	float amount_consumed;
	float satisfaction;
};

std::vector<full_construction_factory> estimate_private_investment_upgrade(sys::state& state, dcon::nation_id nid, float est_private_const_spending);
std::vector<full_construction_factory> estimate_private_investment_construct(sys::state& state, dcon::nation_id nid, bool craved, float est_private_const_spending, bool& potential_target_exists);
std::vector<full_construction_province> estimate_private_investment_province(sys::state& state, dcon::nation_id nid, float est_private_const_spending);

void change_factory_type_in_province(sys::state& state, dcon::province_id p, dcon::factory_type_id t, dcon::factory_type_id refit_target);
void add_factory_level_to_province(sys::state& state, dcon::province_id p, dcon::factory_type_id t);

// NOTE: used to estimate how much you will pay if you were to subsidize a particular nation,
// *not* how much you are paying at the moment
float estimate_war_subsidies(sys::state& state, dcon::nation_id target, dcon::nation_id source);

float estimate_subject_payments_paid(sys::state& state, dcon::nation_id n);
float estimate_subject_payments_received(sys::state& state, dcon::nation_id o);

float estimate_daily_income_ai(sys::state& state, dcon::nation_id n);
float estimate_daily_income(sys::state& state, dcon::nation_id n);

construction_status province_building_construction(sys::state& state, dcon::province_id, province_building_type t);
construction_status factory_upgrade(sys::state& state, dcon::factory_id f);

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

// Returns the closest available market states from the location as the nation as a sorted vector
void get_closest_available_market_states(sys::state& state, std::vector<dcon::state_instance_id>& out_buffer, dcon::nation_id nation_as, dcon::province_id location_from);
// Returns the closest available market states from the location as the nation as a sorted dcon vector
void get_closest_available_market_states(sys::state& state, dcon::dcon_vv_fat_id<dcon::state_instance_id> out_buffer, dcon::nation_id nation_as, dcon::province_id location_from);

// Consumes goods from any of the provided government stockpiles with the given supply-to province and nation.
// The "to_consume" commodity set will be decremented over time and after the call will only hold the commoditiy quantities which it was unable to fufll.
// Make SURE that commodity set passed in is ok to be modified
float consume_from_government_stockpiles(sys::state& state, economy::commodity_set& to_consume, std::span<const dcon::state_instance_id> stockpile_states, dcon::province_id location_from, dcon::nation_id nation_as);


float estimate_investment_pool_daily_loss(sys::state& state, dcon::nation_id n);

bool get_commodity_uses_potentials(sys::state& state, dcon::commodity_id c);
float calculate_province_factory_limit(sys::state& state, dcon::province_id pid, dcon::commodity_id c);
float calculate_state_factory_limit(sys::state& state, dcon::state_instance_id sid, dcon::commodity_id c);
float calculate_nation_factory_limit(sys::state& state, dcon::nation_id nid, dcon::commodity_id c);

void recreate_total_government_stockpiles(sys::state& state);

bool do_resource_potentials_allow_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::factory_type_id type);
bool do_resource_potentials_allow_upgrade(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::factory_type_id type);
bool do_resource_potentials_allow_refit(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::factory_type_id from, dcon::factory_type_id refit_target);

} // namespace economy
