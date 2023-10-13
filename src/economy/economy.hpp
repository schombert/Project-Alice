#pragma once

#include "container_types.hpp"
#include "dcon_generated.hpp"

namespace economy {

struct building_information {
	economy::commodity_set cost;
	float infrastructure = 0.16f;
	int32_t naval_capacity = 1;
	int32_t colonial_range = 50;
	int32_t colonial_points[8] = {30, 50, 70, 90, 110, 130, 150, 170};
	int32_t max_level = 6;
	int32_t time = 1080;
	dcon::modifier_id province_modifier;
	dcon::text_sequence_id name;
	bool defined = false;
};

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

struct global_economy_state {
	building_information building_definitions[max_building_types];
	dcon::modifier_id selector_modifier{};
	dcon::modifier_id immigrator_modifier{};
	float craftsmen_fraction = 0.8f;
};

enum class worker_effect : uint8_t { none = 0, input, output, throughput };

template<typename T>
auto desired_needs_spending(sys::state const& state, T pop_indices) {
	// TODO: gather pop types, extract cached needs sum, etc etc
	return 0.0f;
}

constexpr inline dcon::commodity_id money(0);

// scales the needs values so that they are needs per this many pops
// this value was arrived at by looking at farmers: 40'000 farmers produces enough grain to satisfy about 2/3
// of the nominal life needs value for themselves. If we assume that there is supposed to be enough grain in the world
// to feed everyone, and *every* the rgos was grain (and everyone farmed), the scaling factor would have to be about 60'000
// if all rgos were equally common (there are about 20 of them), the scaling factor would have to be about
// 1'200'000. Assuming that grain is slightly more prevalent, we arrive at the factor below as a nice round number
constexpr inline float needs_scaling_factor = 1'000'000.0f * 2.0f;

float commodity_daily_production_amount(sys::state& state, dcon::commodity_id c);

float rgo_effective_size(sys::state const& state, dcon::nation_id n, dcon::province_id p);
float rgo_full_production_quantity(sys::state const& state, dcon::nation_id n, dcon::province_id p);
float rgo_max_employment(sys::state const& state, dcon::nation_id n, dcon::province_id p);

float factory_max_employment(sys::state const& state, dcon::factory_id f);

bool has_factory(sys::state const& state, dcon::state_instance_id si);
bool has_building(sys::state const& state, dcon::state_instance_id si, dcon::factory_type_id fac);
bool is_bankrupt_debtor_to(sys::state& state, dcon::nation_id debt_holder, dcon::nation_id debtor);

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

void update_rgo_employment(sys::state& state);
void update_factory_employment(sys::state& state);
void daily_update(sys::state& state);
void resolve_constructions(sys::state& state);

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
float estimate_loan_payments(sys::state& state, dcon::nation_id n);
float estimate_subsidy_spending(sys::state& state, dcon::nation_id n);
float estimate_diplomatic_balance(sys::state& state, dcon::nation_id n);

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
template<typename F>
void for_each_new_factory(sys::state& state, dcon::state_instance_id s,
		F&& func); // calls the function repeatedly with new_factory as parameters

struct upgraded_factory {
	float progress = 0.0f;
	dcon::factory_type_id type;
};
template<typename F>
void for_each_upgraded_factory(sys::state& state, dcon::state_instance_id s,
		F&& func); // calls the function repeatedly with new_factory as parameters

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

dcon::modifier_id get_province_selector_modifier(sys::state& state);
dcon::modifier_id get_province_immigrator_modifier(sys::state& state);

} // namespace economy
