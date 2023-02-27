#pragma once
#include <stdint.h>
#include "date_interface.hpp"


namespace sys {

namespace provincial_mod_offsets {
constexpr inline int32_t supply_limit = 0;
constexpr inline int32_t attrition = 1;
constexpr inline int32_t max_attrition = 2;
constexpr inline int32_t local_ruling_party_support = 3;
constexpr inline int32_t poor_life_needs = 4;
constexpr inline int32_t rich_life_needs = 5;
constexpr inline int32_t middle_life_needs = 6;
constexpr inline int32_t poor_everyday_needs = 7;
constexpr inline int32_t rich_everyday_needs = 8;
constexpr inline int32_t middle_everyday_needs = 9;
constexpr inline int32_t poor_luxury_needs = 10;
constexpr inline int32_t middle_luxury_needs = 11;
constexpr inline int32_t rich_luxury_needs = 12;
constexpr inline int32_t population_growth = 13;
constexpr inline int32_t local_factory_input = 14;
constexpr inline int32_t local_factory_output = 15;
constexpr inline int32_t local_factory_throughput = 16;
constexpr inline int32_t local_rgo_input = 17;
constexpr inline int32_t local_rgo_output = 18;
constexpr inline int32_t local_rgo_throughput = 19;
constexpr inline int32_t local_artisan_input = 20;
constexpr inline int32_t local_artisan_output = 21;
constexpr inline int32_t local_artisan_throughput = 22;
constexpr inline int32_t number_of_voters = 23;
constexpr inline int32_t goods_demand = 24;
constexpr inline int32_t assimilation_rate = 25;
constexpr inline int32_t life_rating = 26;
constexpr inline int32_t farm_rgo_eff = 27;
constexpr inline int32_t mine_rgo_eff = 28;
constexpr inline int32_t farm_rgo_size = 29;
constexpr inline int32_t mine_rgo_size = 30;
constexpr inline int32_t pop_militancy_modifier = 31;
constexpr inline int32_t pop_consciousness_modifier = 32;
constexpr inline int32_t rich_income_modifier = 33;
constexpr inline int32_t middle_income_modifier = 34;
constexpr inline int32_t poor_income_modifier = 35;
constexpr inline int32_t boost_strongest_party = 36;
constexpr inline int32_t immigrant_attract = 37;
constexpr inline int32_t immigrant_push = 38;
constexpr inline int32_t local_repair = 39;
constexpr inline int32_t local_ship_build = 40;
constexpr inline int32_t movement_cost = 41;
constexpr inline int32_t defense = 42;
constexpr inline int32_t attack = 43;
constexpr inline int32_t combat_width = 44;
constexpr inline int32_t min_build_naval_base = 45;
constexpr inline int32_t min_build_railroad = 46;
constexpr inline int32_t min_build_fort = 47;

constexpr inline uint32_t count = 48;
}

namespace national_mod_offsets {
constexpr inline int32_t war_exhaustion = 0 + provincial_mod_offsets::count;
constexpr inline int32_t max_war_exhaustion = 1 + provincial_mod_offsets::count;
constexpr inline int32_t leadership = 2 + provincial_mod_offsets::count;
constexpr inline int32_t leadership_modifier = 3 + provincial_mod_offsets::count;
constexpr inline int32_t supply_consumption = 4 + provincial_mod_offsets::count;
constexpr inline int32_t org_regain = 5 + provincial_mod_offsets::count;
constexpr inline int32_t reinforce_speed = 6 + provincial_mod_offsets::count;
constexpr inline int32_t land_organisation = 7 + provincial_mod_offsets::count;
constexpr inline int32_t naval_organisation = 8 + provincial_mod_offsets::count;
constexpr inline int32_t research_points = 9 + provincial_mod_offsets::count;
constexpr inline int32_t research_points_modifier = 10 + provincial_mod_offsets::count;
constexpr inline int32_t research_points_on_conquer = 11 + provincial_mod_offsets::count;
constexpr inline int32_t import_cost = 12 + provincial_mod_offsets::count;
constexpr inline int32_t loan_interest = 13 + provincial_mod_offsets::count;
constexpr inline int32_t tax_efficiency = 14 + provincial_mod_offsets::count;
constexpr inline int32_t min_tax = 15 + provincial_mod_offsets::count;
constexpr inline int32_t max_tax = 16 + provincial_mod_offsets::count;
constexpr inline int32_t min_military_spending = 17 + provincial_mod_offsets::count;
constexpr inline int32_t max_military_spending = 18 + provincial_mod_offsets::count;
constexpr inline int32_t min_social_spending = 19 + provincial_mod_offsets::count;
constexpr inline int32_t max_social_spending = 20 + provincial_mod_offsets::count;
constexpr inline int32_t factory_owner_cost = 21 + provincial_mod_offsets::count;
constexpr inline int32_t min_tariff = 22 + provincial_mod_offsets::count;
constexpr inline int32_t max_tariff = 23 + provincial_mod_offsets::count;
constexpr inline int32_t ruling_party_support = 24 + provincial_mod_offsets::count;
constexpr inline int32_t rich_vote = 25 + provincial_mod_offsets::count;
constexpr inline int32_t middle_vote = 26 + provincial_mod_offsets::count;
constexpr inline int32_t poor_vote = 27 + provincial_mod_offsets::count;
constexpr inline int32_t minimum_wage = 28 + provincial_mod_offsets::count;
constexpr inline int32_t factory_maintenance = 29 + provincial_mod_offsets::count;
constexpr inline int32_t poor_life_needs = 30 + provincial_mod_offsets::count;
constexpr inline int32_t rich_life_needs = 31 + provincial_mod_offsets::count;
constexpr inline int32_t middle_life_needs = 32 + provincial_mod_offsets::count;
constexpr inline int32_t poor_everyday_needs = 33 + provincial_mod_offsets::count;
constexpr inline int32_t rich_everyday_needs = 34 + provincial_mod_offsets::count;
constexpr inline int32_t middle_everyday_needs = 35 + provincial_mod_offsets::count;
constexpr inline int32_t poor_luxury_needs = 36 + provincial_mod_offsets::count;
constexpr inline int32_t middle_luxury_needs = 37 + provincial_mod_offsets::count;
constexpr inline int32_t rich_luxury_needs = 38 + provincial_mod_offsets::count;
constexpr inline int32_t unemployment_benefit = 39 + provincial_mod_offsets::count;
constexpr inline int32_t pension_level = 40 + provincial_mod_offsets::count;
constexpr inline int32_t global_population_growth = 41 + provincial_mod_offsets::count;
constexpr inline int32_t factory_input = 42 + provincial_mod_offsets::count;
constexpr inline int32_t factory_output = 43 + provincial_mod_offsets::count;
constexpr inline int32_t factory_throughput = 44 + provincial_mod_offsets::count;
constexpr inline int32_t rgo_input = 45 + provincial_mod_offsets::count;
constexpr inline int32_t rgo_output = 46 + provincial_mod_offsets::count;
constexpr inline int32_t rgo_throughput = 47 + provincial_mod_offsets::count;
constexpr inline int32_t artisan_input = 48 + provincial_mod_offsets::count;
constexpr inline int32_t artisan_output = 49 + provincial_mod_offsets::count;
constexpr inline int32_t artisan_throughput = 50 + provincial_mod_offsets::count;
constexpr inline int32_t goods_demand = 51 + provincial_mod_offsets::count;
constexpr inline int32_t badboy = 52 + provincial_mod_offsets::count;
constexpr inline int32_t global_assimilation_rate = 53 + provincial_mod_offsets::count;
constexpr inline int32_t prestige = 54 + provincial_mod_offsets::count;
constexpr inline int32_t factory_cost = 55 + provincial_mod_offsets::count;
constexpr inline int32_t farm_rgo_eff = 56 + provincial_mod_offsets::count;
constexpr inline int32_t mine_rgo_eff = 57 + provincial_mod_offsets::count;
constexpr inline int32_t farm_rgo_size = 58 + provincial_mod_offsets::count;
constexpr inline int32_t mine_rgo_size = 59 + provincial_mod_offsets::count;
constexpr inline int32_t issue_change_speed = 60 + provincial_mod_offsets::count;
constexpr inline int32_t social_reform_desire = 61 + provincial_mod_offsets::count;
constexpr inline int32_t political_reform_desire = 62 + provincial_mod_offsets::count;
constexpr inline int32_t literacy_con_impact = 63 + provincial_mod_offsets::count;
constexpr inline int32_t rich_income_modifier = 64 + provincial_mod_offsets::count;
constexpr inline int32_t middle_income_modifier = 65 + provincial_mod_offsets::count;
constexpr inline int32_t poor_income_modifier = 66 + provincial_mod_offsets::count;
constexpr inline int32_t global_immigrant_attract = 67 + provincial_mod_offsets::count;
constexpr inline int32_t poor_savings_modifier = 68 + provincial_mod_offsets::count;
constexpr inline int32_t influence_modifier = 69 + provincial_mod_offsets::count;
constexpr inline int32_t diplomatic_points_modifier = 70 + provincial_mod_offsets::count;
constexpr inline int32_t mobilisation_size = 71 + provincial_mod_offsets::count;
constexpr inline int32_t mobilisation_economy_impact = 72 + provincial_mod_offsets::count;
constexpr inline int32_t global_pop_militancy_modifier = 73 + provincial_mod_offsets::count;
constexpr inline int32_t global_pop_consciousness_modifier = 74 + provincial_mod_offsets::count;
constexpr inline int32_t core_pop_militancy_modifier = 75 + provincial_mod_offsets::count;
constexpr inline int32_t core_pop_consciousness_modifier = 76 + provincial_mod_offsets::count;
constexpr inline int32_t non_accepted_pop_militancy_modifier = 77 + provincial_mod_offsets::count;
constexpr inline int32_t non_accepted_pop_consciousness_modifier = 78 + provincial_mod_offsets::count;
constexpr inline int32_t cb_generation_speed_modifier = 79 + provincial_mod_offsets::count;
constexpr inline int32_t mobilization_impact = 80 + provincial_mod_offsets::count;
constexpr inline int32_t suppression_points_modifier = 81 + provincial_mod_offsets::count;
constexpr inline int32_t education_efficiency_modifier = 82 + provincial_mod_offsets::count;
constexpr inline int32_t civilization_progress_modifier = 83 + provincial_mod_offsets::count;
constexpr inline int32_t administrative_efficiency_modifier = 84 + provincial_mod_offsets::count;
constexpr inline int32_t land_unit_start_experience = 85 + provincial_mod_offsets::count;
constexpr inline int32_t naval_unit_start_experience = 86 + provincial_mod_offsets::count;
constexpr inline int32_t naval_attack_modifier = 87 + provincial_mod_offsets::count;
constexpr inline int32_t naval_defense_modifier = 88 + provincial_mod_offsets::count;
constexpr inline int32_t land_attack_modifier = 89 + provincial_mod_offsets::count;
constexpr inline int32_t land_defense_modifier = 90 + provincial_mod_offsets::count;
constexpr inline int32_t tariff_efficiency_modifier = 91 + provincial_mod_offsets::count;
constexpr inline int32_t max_loan_modifier = 92 + provincial_mod_offsets::count;
constexpr inline int32_t unciv_economic_modifier = 93 + provincial_mod_offsets::count;
constexpr inline int32_t unciv_military_modifier = 94 + provincial_mod_offsets::count;
constexpr inline int32_t self_unciv_economic_modifier = 95 + provincial_mod_offsets::count;
constexpr inline int32_t self_unciv_military_modifier = 96 + provincial_mod_offsets::count;
constexpr inline int32_t commerce_tech_research_bonus = 97 + provincial_mod_offsets::count;
constexpr inline int32_t army_tech_research_bonus = 98 + provincial_mod_offsets::count;
constexpr inline int32_t industry_tech_research_bonus = 99 + provincial_mod_offsets::count;
constexpr inline int32_t navy_tech_research_bonus = 100 + provincial_mod_offsets::count;
constexpr inline int32_t culture_tech_research_bonus = 101 + provincial_mod_offsets::count;
constexpr inline int32_t supply_limit = 102 + provincial_mod_offsets::count;
constexpr inline int32_t colonial_migration = 103 + provincial_mod_offsets::count;
constexpr inline int32_t max_national_focus = 104 + provincial_mod_offsets::count;
constexpr inline int32_t cb_creation_speed = 105 + provincial_mod_offsets::count;
constexpr inline int32_t education_efficiency = 106 + provincial_mod_offsets::count;
constexpr inline int32_t diplomatic_points = 107 + provincial_mod_offsets::count;
constexpr inline int32_t reinforce_rate = 108 + provincial_mod_offsets::count;
constexpr inline int32_t tax_eff = 109 + provincial_mod_offsets::count;
constexpr inline int32_t administrative_efficiency = 110 + provincial_mod_offsets::count;
constexpr inline int32_t influence = 111 + provincial_mod_offsets::count;
constexpr inline int32_t dig_in_cap = 112 + provincial_mod_offsets::count;
constexpr inline int32_t combat_width = 113 + provincial_mod_offsets::count;
constexpr inline int32_t morale = 114 + provincial_mod_offsets::count;
constexpr inline int32_t military_tactics = 115 + provincial_mod_offsets::count;
constexpr inline int32_t supply_range = 116 + provincial_mod_offsets::count;
constexpr inline int32_t regular_experience_level = 117 + provincial_mod_offsets::count;
constexpr inline int32_t increase_research = 118 + provincial_mod_offsets::count;
constexpr inline int32_t soldier_to_pop_loss = 119 + provincial_mod_offsets::count;
constexpr inline int32_t naval_attrition = 120 + provincial_mod_offsets::count;
constexpr inline int32_t land_attrition = 121 + provincial_mod_offsets::count;
constexpr inline int32_t pop_growth = 122 + provincial_mod_offsets::count;
constexpr inline int32_t colonial_life_rating = 123 + provincial_mod_offsets::count;
constexpr inline int32_t seperatism = 124 + provincial_mod_offsets::count;
constexpr inline int32_t plurality = 125 + provincial_mod_offsets::count;
constexpr inline int32_t colonial_prestige = 126 + provincial_mod_offsets::count;
constexpr inline int32_t permanent_prestige = 127 + provincial_mod_offsets::count;

constexpr inline uint32_t count = 128 + provincial_mod_offsets::count;
}

constexpr inline uint32_t total_modifiers_count = national_mod_offsets::count;

constexpr inline uint32_t modifier_definition_size = 10;
constexpr inline uint8_t modifier_bad_offset = 255;

struct modifier_definition {
	float values[modifier_definition_size] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f };
	uint8_t offsets[modifier_definition_size] = { 0,0,0,0,0,0,0,0 }; // stores actual offset + 1, with 0 for bad offset

	bool valid_offset_at_index(int32_t i) {
		return offsets[i] != 0;
	}
	int32_t get_offet_at_index(int32_t i) {
		return offsets[i] - 1;
	}
};

struct commodity_modifier {
	float amount = 0.0f;
	dcon::commodity_id type;
};

struct unit_variable_stats {
	int32_t build_time = 0;
	int32_t default_organisation = 0;
	float maximum_speed = 0.0f;
	float defence_or_hull = 0.0f;
	float attack_or_gun_power = 0.0f;
	float supply_consumption = 0.0f;
	float support = 0.0f;
	float siege_or_torpedo_attack = 0.0f;
	float reconnaissance_or_fire_range = 0.0f;
	float evasion = 0.0f;
	float maneuver = 0.0f;
	float discipline = 0.0f;

	void operator+=(unit_variable_stats const& other) {
		build_time += other.build_time;
		default_organisation += other.default_organisation;
		maximum_speed += other.maximum_speed;
		defence_or_hull += other.defence_or_hull;
		attack_or_gun_power += other.attack_or_gun_power;
		supply_consumption += other.supply_consumption;
		support += other.support;
		siege_or_torpedo_attack += other.siege_or_torpedo_attack;
		reconnaissance_or_fire_range += other.reconnaissance_or_fire_range;
		evasion += other.evasion;
		maneuver += other.maneuver;
		discipline += other.discipline;
	}
};

struct unit_modifier : public unit_variable_stats {
	dcon::unit_type_id type;
};

struct rebel_org_modifier {
	float amount = 0.0f;
	dcon::rebel_type_id type; // no type set = all rebels
};

struct dated_modifier {
	sys::date expiration;
	dcon::modifier_id mod_id;
};

// NOTE: these functions do not add or remove a modifier from the list of modifiers for an entity
void apply_modifier_values_to_nation(sys::state& state, dcon::nation_id target_nation, dcon::modifier_id mod_id);
void apply_modifier_values_to_province(sys::state& state, dcon::province_id target_prov, dcon::modifier_id mod_id);
void remove_modifier_values_from_nation(sys::state& state, dcon::nation_id target_nation, dcon::modifier_id mod_id);
void remove_modifier_values_from_province(sys::state& state, dcon::province_id target_prov, dcon::modifier_id mod_id); // also removes national values form owner
void apply_modifier_values_to_province_owner(sys::state& state, dcon::nation_id target_nation, dcon::modifier_id mod_id);
void remove_modifier_values_from_province_owner(sys::state& state, dcon::nation_id target_nation, dcon::modifier_id mod_id);

// restores values after loading a save
void repopulate_modifier_effects(sys::state& state);

void add_modifier_to_nation(sys::state& state, dcon::nation_id target_nation, dcon::modifier_id mod_id, sys::date expiration); // default construct date for no expiration
void add_modifier_to_province(sys::state& state, dcon::province_id target_prov, dcon::modifier_id mod_id, sys::date expiration); // default construct date for no expiration
void remove_modifier_from_nation(sys::state& state, dcon::nation_id target_nation, dcon::modifier_id mod_id);
void remove_modifier_from_province(sys::state& state, dcon::province_id target_prov, dcon::modifier_id mod_id);
void remove_expired_modifiers_from_nation(sys::state& state, dcon::nation_id target_nation);
void remove_expired_modifiers_from_province(sys::state& state, dcon::province_id target_prov);

}

