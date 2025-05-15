#pragma once

#include "container_types.hpp"
#include "dcon_generated.hpp"
#include "commands.hpp"

namespace economy {

enum commodity_production_type {
	primary,
	derivative,
	both
};

struct building_information {
	economy::commodity_set cost;
	int32_t naval_capacity = 1;
	int32_t colonial_points[8] = { 30, 50, 70, 90, 110, 130, 150, 170 };
	int32_t colonial_range = 50;
	int32_t max_level = 6;
	int32_t time = 1080;
	float infrastructure = 0.16f;
	dcon::text_key name;
	dcon::modifier_id province_modifier;
	uint16_t padding2 = 0;
	bool defined = false;
	uint8_t padding[3] = { 0 };
};

static_assert(sizeof(building_information) == 104);
static_assert(sizeof(building_information::cost) == 40);
static_assert(sizeof(building_information::colonial_points) == 32);
static_assert(sizeof(building_information::province_modifier) == 2);
static_assert(sizeof(building_information::name) == 4);
static_assert(sizeof(building_information::cost)
	+ sizeof(building_information::naval_capacity)
	+ sizeof(building_information::colonial_range)
	+ sizeof(building_information::colonial_points)
	+ sizeof(building_information::max_level)
	+ sizeof(building_information::time)
	+ sizeof(building_information::infrastructure)
	== 92);
static_assert(sizeof(building_information) ==
	sizeof(building_information::cost)
	+ sizeof(building_information::naval_capacity)
	+ sizeof(building_information::colonial_range)
	+ sizeof(building_information::colonial_points)
	+ sizeof(building_information::max_level)
	+ sizeof(building_information::time)
	+ sizeof(building_information::infrastructure)
	+ sizeof(building_information::province_modifier)
	+ sizeof(building_information::name)
	+ sizeof(building_information::defined)
	+ sizeof(building_information::padding)
	+ sizeof(building_information::padding2));

inline std::string_view province_building_type_get_name(economy::province_building_type v) {
	switch(v) {
	case economy::province_building_type::railroad:
		return "railroad";
	case economy::province_building_type::fort:
		return "fort";
	case economy::province_building_type::naval_base:
		return "naval_base";
	case economy::province_building_type::bank:
		return "bank";
	case economy::province_building_type::university:
		return "university";
	default:
		return "???";
	}
}
inline std::string_view province_building_type_get_level_text(economy::province_building_type v) {
	switch(v) {
	case economy::province_building_type::railroad:
		return "railroad_level";
	case economy::province_building_type::fort:
		return "fort_level";
	case economy::province_building_type::naval_base:
		return "naval_base_level";
	case economy::province_building_type::bank:
		return "bank_level";
	case economy::province_building_type::university:
		return "university_level";
	default:
		return "???";
	}
}

// base subsistence
inline constexpr float subsistence_factor = 5.0f;
inline constexpr float subsistence_score_life = 30.0f;
//inline constexpr float subsistence_score_everyday = 30.0f;
inline constexpr float subsistence_score_total = subsistence_score_life;// +subsistence_score_everyday;

//local merchants take a cut from most local monetary operations
inline constexpr float local_market_cut_baseline = 0.15f;

struct global_economy_state {
	building_information building_definitions[max_building_types];
	float craftsmen_fraction = 0.8f;
	dcon::modifier_id selector_modifier{};
	dcon::modifier_id immigrator_modifier{};
};
static_assert(sizeof(global_economy_state) ==
	sizeof(global_economy_state::building_definitions)
	+ sizeof(global_economy_state::selector_modifier)
	+ sizeof(global_economy_state::immigrator_modifier)
	+ sizeof(global_economy_state::craftsmen_fraction));

enum class worker_effect : uint8_t { none = 0, input, output, throughput };

template<typename T>
auto desired_needs_spending(sys::state const& state, T pop_indices) {
	// TODO: gather pop types, extract cached needs sum, etc etc
	return 0.0f;
}

constexpr inline dcon::commodity_id money(0);

// move to defines later
inline constexpr float payouts_spending_multiplier = 10.f;

inline constexpr float production_scale_delta = 0.1f;
inline constexpr float factory_closed_threshold = 0.0001f;
inline constexpr uint32_t price_history_length = 256;
inline constexpr uint32_t gdp_history_length = 128;
inline constexpr float price_speed_mod = 0.001f;
inline constexpr float price_rigging = 0.02f;
inline constexpr float production_throughput_multiplier = 2.5f; // for the sake of machine tools

// stockpile related things:
inline constexpr float stockpile_to_supply = 0.1f;
inline constexpr float stockpile_spoilage = 0.01f;
inline constexpr float stockpile_expected_spending_per_commodity = 1'000.f;
inline constexpr float market_savings_target = 1'000'000.f;
inline constexpr float trade_transaction_soft_limit = 10'000.f;

// trade related
inline constexpr float merchant_cut_foreign = 0.05f;
inline constexpr float merchant_cut_domestic = 0.001f;
inline constexpr float effect_of_transportation_scale = 0.0005f;
inline constexpr float trade_distance_covered_by_pair_of_workers_per_unit_of_good = 100.f;

// greed drives incomes of corresponding pops up
// while making life worse on average
// profit cuts change distribution of incomes
inline constexpr float aristocrats_greed = 0.1f;
inline constexpr float artisans_greed = 0.001f;
inline constexpr float labor_greed_life = 2.1f;
inline constexpr float labor_greed_everyday = 0.001f;
// inline constexpr float capitalists_greed = 1.f; // for future use

void presimulate(sys::state& state);
void sanity_check(sys::state& state);

float commodity_daily_production_amount(sys::state& state, dcon::commodity_id c);

float effective_tariff_import_rate(sys::state& state, dcon::nation_id n, dcon::market_id m);
float effective_tariff_export_rate(sys::state& state, dcon::nation_id n, dcon::market_id m);

float subsistence_max_pseudoemployment(sys::state& state, dcon::nation_id n, dcon::province_id p);
float factory_total_employment_score(sys::state const& state, dcon::factory_id f);

bool has_building(sys::state const& state, dcon::state_instance_id si, dcon::factory_type_id fac);
bool is_bankrupt_debtor_to(sys::state& state, dcon::nation_id debt_holder, dcon::nation_id debtor);

//factories

// abstract modifiers
float factory_total_desired_employment_score(sys::state const& state, dcon::factory_id f);
float factory_total_employment(sys::state const& state, dcon::factory_id f);
float factory_unqualified_employment(sys::state const& state, dcon::factory_id f);
float factory_primary_employment(sys::state const& state, dcon::factory_id f);
float factory_secondary_employment(sys::state const& state, dcon::factory_id f);

// misc

int32_t factory_priority(sys::state const& state, dcon::factory_id f);
void set_factory_priority(sys::state& state, dcon::factory_id f, int32_t priority);
bool factory_is_profitable(sys::state const& state, dcon::factory_id f);

bool nation_is_constructing_factories(sys::state& state, dcon::nation_id n);
bool nation_has_closed_factories(sys::state& state, dcon::nation_id n);

dcon::unilateral_relationship_id nation_gives_free_trade_rights(sys::state& state, dcon::nation_id source, dcon::nation_id target);

void initialize(sys::state& state);
void regenerate_unsaved_values(sys::state& state);

float pop_min_wage_factor(sys::state& state, dcon::nation_id n);
float farmer_min_wage(sys::state& state, dcon::market_id m, float min_wage_factor);
float laborer_min_wage(sys::state& state, dcon::market_id m, float min_wage_factor);

void daily_update(sys::state& state, bool presimulation, float presimulation_stage);
void resolve_constructions(sys::state& state);

std::vector<dcon::factory_type_id> commodity_get_factory_types_as_output(sys::state const& state, dcon::commodity_id output_good);

float stockpile_commodity_daily_increase(sys::state& state, dcon::commodity_id c, dcon::nation_id n);
float global_market_commodity_daily_increase(sys::state& state, dcon::commodity_id c);
float government_consumption(sys::state& state, dcon::nation_id n, dcon::commodity_id c);
float nation_pop_consumption(sys::state& state, dcon::nation_id n, dcon::commodity_id c);
float nation_total_imports(sys::state& state, dcon::nation_id n);

struct tariff_data {
	std::array<bool, 2> applies_tariff;
	std::array<float, 2> export_tariff;
	std::array<float, 2> import_tariff;

	float distance;
	float loss;
	float base_distance_cost;
	float workers_satisfaction;
};

struct trade_and_tariff {
	dcon::market_id origin;
	dcon::market_id target;

	dcon::nation_id origin_nation;
	dcon::nation_id target_nation;

	float amount_origin;
	float amount_target;

	float tariff_origin;
	float tariff_target;

	float tariff_rate_origin;
	float tariff_rate_target;

	float price_origin;
	float price_target;

	float transport_cost;
	float transportaion_loss;
	float distance;

	float payment_per_unit;
	float payment_received_per_unit;
};

tariff_data explain_trade_route(sys::state& state, dcon::trade_route_id trade_route);
trade_and_tariff explain_trade_route_commodity(sys::state& state, dcon::trade_route_id trade_route, tariff_data& additional_data, dcon::commodity_id cid);
trade_and_tariff explain_trade_route_commodity(sys::state& state, dcon::trade_route_id trade_route, dcon::commodity_id cid);
struct trade_breakdown_item {
	dcon::nation_id trade_partner;
	dcon::commodity_id commodity;
	float traded_amount;
	float tariff;
};
std::vector<trade_breakdown_item> explain_national_tariff(sys::state& state, dcon::nation_id n, bool import_flag, bool export_flag);

float estimate_gold_income(sys::state& state, dcon::nation_id n);
float estimate_tariff_import_income(sys::state& state, dcon::nation_id n);
float estimate_tariff_export_income(sys::state& state, dcon::nation_id n);
float estimate_social_spending(sys::state& state, dcon::nation_id n);
float estimate_pop_payouts_by_income_type(sys::state& state, dcon::nation_id n, culture::income_type in);
float estimate_subsidy_spending(sys::state& state, dcon::nation_id n);
float estimate_diplomatic_balance(sys::state& state, dcon::nation_id n);
float estimate_diplomatic_income(sys::state& state, dcon::nation_id n);
float estimate_diplomatic_expenses(sys::state& state, dcon::nation_id n);
float estimate_max_domestic_investment(sys::state& state, dcon::nation_id n);
float estimate_current_domestic_investment(sys::state& state, dcon::nation_id n);

float estimate_land_spending(sys::state& state, dcon::nation_id n);
float estimate_naval_spending(sys::state& state, dcon::nation_id n);
float estimate_private_construction_spendings(sys::state& state, dcon::nation_id nid);
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
float estimate_war_subsidies(sys::state& state, dcon::nation_fat_id target, dcon::nation_fat_id source);

float estimate_subject_payments_paid(sys::state& state, dcon::nation_id n);
float estimate_subject_payments_received(sys::state& state, dcon::nation_id o);

float estimate_daily_income(sys::state& state, dcon::nation_id n);

struct construction_status {
	float progress = 0.0f; // in range [0,1)
	bool is_under_construction = false;
};

construction_status province_building_construction(sys::state& state, dcon::province_id, province_building_type t);
construction_status factory_upgrade(sys::state& state, dcon::factory_id f);

struct new_factory {
	float progress = 0.0f;
	dcon::factory_type_id type;
};

struct upgraded_factory {
	float progress = 0.0f;
	dcon::factory_type_id type;
	dcon::factory_type_id target_type;
};

float unit_construction_progress(sys::state& state, dcon::province_land_construction_id c);
float unit_construction_progress(sys::state& state, dcon::province_naval_construction_id c);
void try_add_factory_to_state(sys::state& state, dcon::state_instance_id s, dcon::factory_type_id t);
void bound_budget_settings(sys::state& state, dcon::nation_id n);

int32_t most_recent_price_record_index(sys::state& state);
int32_t previous_price_record_index(sys::state& state);
int32_t most_recent_gdp_record_index(sys::state& state);
int32_t previous_gdp_record_index(sys::state& state);

float gdp(sys::state& state, dcon::nation_id n);
float gdp(sys::state& state, dcon::market_id n);

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

command::budget_settings_data budget_minimums(sys::state& state, dcon::nation_id n);
command::budget_settings_data budget_maximums(sys::state& state, dcon::nation_id n);
} // namespace economy
