#pragma once

#include "container_types.hpp"
#include "dcon_generated.hpp"

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
	dcon::text_sequence_id name;
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

float get_artisan_distribution_slow(sys::state& state, dcon::nation_id n, dcon::commodity_id c);

// base subsistence
inline constexpr float subsistence_factor = 10.0f;
inline constexpr float subsistence_score_life = 20.0f;
inline constexpr float subsistence_score_everyday = 30.0f;
inline constexpr float subsistence_score_luxury = 40.0f;
inline constexpr float subsistence_score_total = subsistence_score_life + subsistence_score_everyday + subsistence_score_luxury;

float local_subsistence_factor(sys::state const& state, dcon::province_id p);

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

inline constexpr float production_scale_delta = 0.1f;
inline constexpr float factory_closed_threshold = 0.0001f;
inline constexpr uint32_t price_history_length = 256;
inline constexpr float rgo_owners_cut = 0.75f;

void presimulate(sys::state& state);

float commodity_daily_production_amount(sys::state& state, dcon::commodity_id c);

float rgo_effective_size(sys::state const& state, dcon::nation_id n, dcon::province_id p);
float rgo_full_production_quantity(sys::state const& state, dcon::nation_id n, dcon::province_id p);
float rgo_max_employment(sys::state const& state, dcon::nation_id n, dcon::province_id p);

float factory_max_employment(sys::state const& state, dcon::factory_id f);

bool has_factory(sys::state const& state, dcon::state_instance_id si);
bool has_building(sys::state const& state, dcon::state_instance_id si, dcon::factory_type_id fac);
bool is_bankrupt_debtor_to(sys::state& state, dcon::nation_id debt_holder, dcon::nation_id debtor);

void populate_effective_prices(sys::state& state, dcon::nation_id n);
float factory_min_input_available(sys::state& state, dcon::nation_id n, dcon::factory_type_fat_id fac_type);
float factory_input_total_cost(sys::state& state, dcon::nation_id n, dcon::factory_type_fat_id fac_type);
float factory_min_e_input_available(sys::state& state, dcon::nation_id n, dcon::factory_type_fat_id fac_type);
float factory_e_input_total_cost(sys::state& state, dcon::nation_id n, dcon::factory_type_fat_id fac_type);
float factory_input_multiplier(sys::state& state, dcon::factory_fat_id fac, dcon::nation_id n, dcon::province_id p, dcon::state_instance_id s);
float factory_throughput_multiplier(sys::state& state, dcon::factory_type_fat_id fac_type, dcon::nation_id n, dcon::province_id p, dcon::state_instance_id s);
float factory_output_multiplier(sys::state& state, dcon::factory_fat_id fac, dcon::nation_id n, dcon::province_id p);
float factory_max_production_scale(sys::state& state, dcon::factory_fat_id fac, float mobilization_impact, bool occupied, bool overseas);
float factory_desired_raw_profit(dcon::factory_fat_id fac, float spendings);
float factory_total_employment(sys::state const& state, dcon::factory_id f);
float factory_primary_employment(sys::state const& state, dcon::factory_id f);
float factory_secondary_employment(sys::state const& state, dcon::factory_id f);
int32_t factory_priority(sys::state const& state, dcon::factory_id f);
void set_factory_priority(sys::state& state, dcon::factory_id f, int32_t priority);
bool factory_is_profitable(sys::state const& state, dcon::factory_id f);

bool nation_is_constructing_factories(sys::state& state, dcon::nation_id n);
bool nation_has_closed_factories(sys::state& state, dcon::nation_id n);

void initialize(sys::state& state);
void regenerate_unsaved_values(sys::state& state);

float pop_min_wage_factor(sys::state& state, dcon::nation_id n);
float pop_farmer_min_wage(sys::state& state, dcon::nation_id n, float min_wage_factor);
float pop_laborer_min_wage(sys::state& state, dcon::nation_id n, float min_wage_factor);
float pop_factory_min_wage(sys::state& state, dcon::nation_id n, float min_wage_factor);

struct rgo_workers_breakdown {
	float paid_workers;
	float slaves;
	float total;
};

rgo_workers_breakdown rgo_relevant_population(sys::state& state, dcon::province_id p, dcon::nation_id n);

float rgo_overhire_modifier(sys::state& state, dcon::province_id p, dcon::nation_id n);
float rgo_desired_worker_norm_profit(sys::state& state, dcon::province_id p, dcon::nation_id n, float min_wage, float total_relevant_population);
float rgo_expected_worker_norm_profit(sys::state& state, dcon::province_id p, dcon::nation_id n);


void update_rgo_employment(sys::state& state);
void update_factory_employment(sys::state& state);
void daily_update(sys::state& state, bool initiate_building);
void resolve_constructions(sys::state& state);

float base_artisan_profit(sys::state& state, dcon::nation_id n, dcon::commodity_id c);
float artisan_scale_limit(sys::state& state, dcon::nation_id n, dcon::commodity_id c);

float stockpile_commodity_daily_increase(sys::state& state, dcon::commodity_id c, dcon::nation_id n);
float global_market_commodity_daily_increase(sys::state& state, dcon::commodity_id c);
float government_consumption(sys::state& state, dcon::nation_id n, dcon::commodity_id c);
float nation_factory_consumption(sys::state& state, dcon::nation_id n, dcon::commodity_id c);
float nation_pop_consumption(sys::state& state, dcon::nation_id n, dcon::commodity_id c);
float nation_total_imports(sys::state& state, dcon::nation_id n);
float pop_income(sys::state& state, dcon::pop_id p);

float estimate_gold_income(sys::state& state, dcon::nation_id n);
float estimate_tariff_income(sys::state& state, dcon::nation_id n);
float estimate_social_spending(sys::state& state, dcon::nation_id n);
float estimate_pop_payouts_by_income_type(sys::state& state, dcon::nation_id n, culture::income_type in);
float estimate_tax_income_by_strata(sys::state& state, dcon::nation_id n, culture::pop_strata ps);
float estimate_subsidy_spending(sys::state& state, dcon::nation_id n);
float estimate_diplomatic_balance(sys::state& state, dcon::nation_id n);
float estimate_domestic_investment(sys::state& state, dcon::nation_id n);

float estimate_land_spending(sys::state& state, dcon::nation_id n);
float estimate_naval_spending(sys::state& state, dcon::nation_id n);
float estimate_construction_spending(sys::state& state, dcon::nation_id n);
float estimate_war_subsidies_spending(sys::state& state, dcon::nation_id n);
float estimate_reparations_spending(sys::state& state, dcon::nation_id n);
float estimate_war_subsidies_income(sys::state& state, dcon::nation_id n);
float estimate_reparations_income(sys::state& state, dcon::nation_id n);
float estimate_overseas_penalty_spending(sys::state& state, dcon::nation_id n);
float estimate_stockpile_filling_spending(sys::state& state, dcon::nation_id n);

// NOTE: used to estimate how much you will pay if you were to subsidize a particular nation,
// *not* how much you are paying at the moment
float estimate_war_subsidies(sys::state& state, dcon::nation_id n);

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
};

bool state_contains_constructed_factory(sys::state& state, dcon::state_instance_id si, dcon::factory_type_id ft);
bool state_contains_factory(sys::state& state, dcon::state_instance_id s, dcon::factory_type_id ft);
int32_t state_factory_count(sys::state& state, dcon::state_instance_id sid, dcon::nation_id n);
float unit_construction_progress(sys::state& state, dcon::province_land_construction_id c);
float unit_construction_progress(sys::state& state, dcon::province_naval_construction_id c);
void try_add_factory_to_state(sys::state& state, dcon::state_instance_id s, dcon::factory_type_id t);
void bound_budget_settings(sys::state& state, dcon::nation_id n);

int32_t most_recent_price_record_index(sys::state& state);
int32_t previous_price_record_index(sys::state& state);

void prune_factories(sys::state& state); // get rid of closed factories in full states
void go_bankrupt(sys::state& state, dcon::nation_id n);
dcon::modifier_id get_province_selector_modifier(sys::state& state);
dcon::modifier_id get_province_immigrator_modifier(sys::state& state);

bool can_take_loans(sys::state& state, dcon::nation_id n);
float interest_payment(sys::state& state, dcon::nation_id n);
float max_loan(sys::state& state, dcon::nation_id n);

commodity_production_type get_commodity_production_type(sys::state& state, dcon::commodity_id c);

} // namespace economy
