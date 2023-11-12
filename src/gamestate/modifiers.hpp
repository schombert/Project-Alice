#pragma once
#include <stdint.h>
#include "date_interface.hpp"
#include "dcon_generated.hpp"

namespace sys {

// cat t.cpp | awk '{print $1 $6 ", " $5 ", " $2 " " $3 " " $4}' >t2.cpp
#define MOD_PROV_LIST                                                                                                            \
	MOD_LIST_ELEMENT(0, supply_limit, true, modifier_display_type::integer, "modifier_supply_limit")                               \
	MOD_LIST_ELEMENT(1, attrition, false, modifier_display_type::percent, "modifier_attrition")                                    \
	MOD_LIST_ELEMENT(2, max_attrition, false, modifier_display_type::integer, "modifier_max_attrition")                            \
	MOD_LIST_ELEMENT(3, local_ruling_party_support, true, modifier_display_type::percent, "modifier_ruling_party_support")   \
	MOD_LIST_ELEMENT(4, poor_life_needs, false, modifier_display_type::percent, "modifier_poor_life_needs")                        \
	MOD_LIST_ELEMENT(5, rich_life_needs, false, modifier_display_type::percent, "modifier_rich_life_needs")                        \
	MOD_LIST_ELEMENT(6, middle_life_needs, false, modifier_display_type::percent, "modifier_middle_life_needs")                    \
	MOD_LIST_ELEMENT(7, poor_everyday_needs, false, modifier_display_type::percent, "modifier_poor_everyday_needs")                \
	MOD_LIST_ELEMENT(8, rich_everyday_needs, false, modifier_display_type::percent, "modifier_rich_everyday_needs")                \
	MOD_LIST_ELEMENT(9, middle_everyday_needs, false, modifier_display_type::percent, "modifier_middle_everyday_needs")            \
	MOD_LIST_ELEMENT(10, poor_luxury_needs, false, modifier_display_type::percent, "modifier_poor_luxury_needs")                   \
	MOD_LIST_ELEMENT(11, middle_luxury_needs, false, modifier_display_type::percent, "modifier_middle_luxury_needs")               \
	MOD_LIST_ELEMENT(12, rich_luxury_needs, false, modifier_display_type::percent, "modifier_rich_luxury_needs")                   \
	MOD_LIST_ELEMENT(13, population_growth, true, modifier_display_type::fp_three_places, "modifier_population_growth")            \
	MOD_LIST_ELEMENT(14, local_factory_input, false, modifier_display_type::percent, "modifier_factory_input")               \
	MOD_LIST_ELEMENT(15, local_factory_output, true, modifier_display_type::percent, "modifier_factory_output")              \
	MOD_LIST_ELEMENT(16, local_factory_throughput, true, modifier_display_type::percent, "modifier_factory_throughput")      \
	MOD_LIST_ELEMENT(17, local_rgo_input, true, modifier_display_type::percent, "modifier_rgo_input")                        \
	MOD_LIST_ELEMENT(18, local_rgo_output, true, modifier_display_type::percent, "modifier_rgo_output")                      \
	MOD_LIST_ELEMENT(19, local_rgo_throughput, true, modifier_display_type::percent, "modifier_rgo_throughput")              \
	MOD_LIST_ELEMENT(20, local_artisan_input, false, modifier_display_type::percent, "modifier_artisan_input")               \
	MOD_LIST_ELEMENT(21, local_artisan_output, true, modifier_display_type::percent, "modifier_artisan_output")              \
	MOD_LIST_ELEMENT(22, local_artisan_throughput, true, modifier_display_type::percent, "modifier_artisan_throughput")      \
	MOD_LIST_ELEMENT(23, number_of_voters, true, modifier_display_type::percent, "modifier_number_of_voters")                      \
	MOD_LIST_ELEMENT(24, goods_demand, true, modifier_display_type::percent, "modifier_goods_demand")                              \
	MOD_LIST_ELEMENT(25, assimilation_rate, true, modifier_display_type::percent, "modifier_assimilation_rate")                    \
	MOD_LIST_ELEMENT(26, life_rating, true, modifier_display_type::percent, "modifier_life_rating")                                \
	MOD_LIST_ELEMENT(27, farm_rgo_eff, true, modifier_display_type::percent, "modifier_farm_efficiency")                           \
	MOD_LIST_ELEMENT(28, mine_rgo_eff, true, modifier_display_type::percent, "modifier_mine_efficiency")                           \
	MOD_LIST_ELEMENT(29, farm_rgo_size, true, modifier_display_type::percent, "modifier_farm_size")                                \
	MOD_LIST_ELEMENT(30, mine_rgo_size, true, modifier_display_type::percent, "modifier_mine_size")                                \
	MOD_LIST_ELEMENT(31, pop_militancy_modifier, false, modifier_display_type::fp_two_places, "modifier_pop_militancy_modifier") \
	MOD_LIST_ELEMENT(32, pop_consciousness_modifier, true, modifier_display_type::fp_two_places,                                \
			"modifier_pop_consciousness_modifier")                                                                                     \
	MOD_LIST_ELEMENT(33, rich_income_modifier, true, modifier_display_type::percent, "modifier_rich_income_modifier")              \
	MOD_LIST_ELEMENT(34, middle_income_modifier, true, modifier_display_type::percent, "modifier_middle_income_modifier")          \
	MOD_LIST_ELEMENT(35, poor_income_modifier, true, modifier_display_type::percent, "modifier_poor_income_modifier")              \
	MOD_LIST_ELEMENT(36, boost_strongest_party, true, modifier_display_type::percent, "modifier_boost_strongest_party")            \
	MOD_LIST_ELEMENT(37, immigrant_attract, true, modifier_display_type::percent, "modifier_immigant_attract")                    \
	MOD_LIST_ELEMENT(38, immigrant_push, true, modifier_display_type::percent, "modifier_immigant_push")                          \
	MOD_LIST_ELEMENT(39, local_repair, true, modifier_display_type::percent, "modifier_local_repair")                              \
	MOD_LIST_ELEMENT(40, local_ship_build, false, modifier_display_type::percent, "modifier_local_ship_build")                     \
	MOD_LIST_ELEMENT(41, movement_cost, false, modifier_display_type::percent, "modifier_movement_cost")                           \
	MOD_LIST_ELEMENT(42, defense, true, modifier_display_type::integer, "defence")                                                 \
	MOD_LIST_ELEMENT(43, attack, true, modifier_display_type::integer, "attack")                                                   \
	MOD_LIST_ELEMENT(44, combat_width, true, modifier_display_type::integer, "modifier_combat_width")                              \
	MOD_LIST_ELEMENT(45, min_build_naval_base, false, modifier_display_type::integer, "naval_base_level")                          \
	MOD_LIST_ELEMENT(46, min_build_railroad, false, modifier_display_type::integer, "railroad_level")                              \
	MOD_LIST_ELEMENT(47, min_build_fort, false, modifier_display_type::integer, "fort_level") \
	MOD_LIST_ELEMENT(48, min_build_bank, false, modifier_display_type::integer, "bank_level") \
	MOD_LIST_ELEMENT(49, min_build_university, false, modifier_display_type::integer, "university_level")
#define MOD_PROV_LIST_COUNT 50

#define MOD_NAT_LIST                                                                                                             \
	MOD_LIST_ELEMENT(0, war_exhaustion, false, modifier_display_type::fp_two_places, "war_exhaustion")                             \
	MOD_LIST_ELEMENT(1, max_war_exhaustion, true, modifier_display_type::integer, "max_war_exhaustion")                            \
	MOD_LIST_ELEMENT(2, leadership, true, modifier_display_type::integer, "leadership")                                            \
	MOD_LIST_ELEMENT(3, leadership_modifier, true, modifier_display_type::percent, "modifier_global_leadership_modifier")          \
	MOD_LIST_ELEMENT(4, supply_consumption, false, modifier_display_type::percent, "modifier_supply_consumption")                  \
	MOD_LIST_ELEMENT(5, org_regain, true, modifier_display_type::percent, "modifier_org_regain")                                   \
	MOD_LIST_ELEMENT(6, reinforce_speed, true, modifier_display_type::percent, "modifier_reinforce_speed")                         \
	MOD_LIST_ELEMENT(7, land_organisation, true, modifier_display_type::percent, "modifier_land_organisation")                     \
	MOD_LIST_ELEMENT(8, naval_organisation, true, modifier_display_type::percent, "modifier_naval_organisation")                   \
	MOD_LIST_ELEMENT(9, research_points, true, modifier_display_type::integer, "modifier_research_points")                         \
	MOD_LIST_ELEMENT(10, research_points_modifier, true, modifier_display_type::percent, "modifier_research_points_modifier")      \
	MOD_LIST_ELEMENT(11, research_points_on_conquer, true, modifier_display_type::percent, "modifier_research_points_on_conquer")  \
	MOD_LIST_ELEMENT(12, import_cost, false, modifier_display_type::percent, "modifier_import_cost")                               \
	MOD_LIST_ELEMENT(13, loan_interest, false, modifier_display_type::percent, "loan_interest_tech")                               \
	MOD_LIST_ELEMENT(14, tax_efficiency, true, modifier_display_type::percent, "modifier_tax_efficiency")                          \
	MOD_LIST_ELEMENT(15, min_tax, true, modifier_display_type::percent, "modifier_min_tax")                                        \
	MOD_LIST_ELEMENT(16, max_tax, true, modifier_display_type::percent, "modifier_max_tax")                                        \
	MOD_LIST_ELEMENT(17, min_military_spending, true, modifier_display_type::percent, "modifier_min_military_spending")            \
	MOD_LIST_ELEMENT(18, max_military_spending, true, modifier_display_type::percent, "modifier_max_military_spending")            \
	MOD_LIST_ELEMENT(19, min_social_spending, true, modifier_display_type::percent, "modifier_min_social_spending")                \
	MOD_LIST_ELEMENT(20, max_social_spending, true, modifier_display_type::percent, "modifier_max_social_spending")                \
	MOD_LIST_ELEMENT(21, factory_owner_cost, false, modifier_display_type::percent, "modifier_factory_owner_cost")                 \
	MOD_LIST_ELEMENT(22, min_tariff, true, modifier_display_type::percent, "modifier_min_tariff")                                  \
	MOD_LIST_ELEMENT(23, max_tariff, true, modifier_display_type::percent, "modifier_max_tariff")                                  \
	MOD_LIST_ELEMENT(24, ruling_party_support, true, modifier_display_type::percent, "modifier_ruling_party_support")              \
	MOD_LIST_ELEMENT(25, rich_vote, true, modifier_display_type::percent, "modifier_rich_vote")                                    \
	MOD_LIST_ELEMENT(26, middle_vote, true, modifier_display_type::percent, "modifier_middle_vote")                                \
	MOD_LIST_ELEMENT(27, poor_vote, true, modifier_display_type::percent, "modifier_poor_vote")                                    \
	MOD_LIST_ELEMENT(28, minimum_wage, true, modifier_display_type::percent, "modifier_minimun_wage")                              \
	MOD_LIST_ELEMENT(29, factory_maintenance, false, modifier_display_type::percent, "modifier_factory_maintenance")               \
	MOD_LIST_ELEMENT(30, poor_life_needs, false, modifier_display_type::percent, "modifier_poor_life_needs")                       \
	MOD_LIST_ELEMENT(31, rich_life_needs, false, modifier_display_type::percent, "modifier_rich_life_needs")                       \
	MOD_LIST_ELEMENT(32, middle_life_needs, false, modifier_display_type::percent, "modifier_middle_life_needs")                   \
	MOD_LIST_ELEMENT(33, poor_everyday_needs, false, modifier_display_type::percent, "modifier_poor_everyday_needs")               \
	MOD_LIST_ELEMENT(34, rich_everyday_needs, false, modifier_display_type::percent, "modifier_rich_everyday_needs")               \
	MOD_LIST_ELEMENT(35, middle_everyday_needs, false, modifier_display_type::percent, "modifier_middle_everyday_needs")           \
	MOD_LIST_ELEMENT(36, poor_luxury_needs, false, modifier_display_type::percent, "modifier_poor_luxury_needs")                   \
	MOD_LIST_ELEMENT(37, middle_luxury_needs, false, modifier_display_type::percent, "modifier_middle_luxury_needs")               \
	MOD_LIST_ELEMENT(38, rich_luxury_needs, false, modifier_display_type::percent, "modifier_rich_luxury_needs")                   \
	MOD_LIST_ELEMENT(39, unemployment_benefit, true, modifier_display_type::percent, "modifier_unemployment_benefit")              \
	MOD_LIST_ELEMENT(40, pension_level, true, modifier_display_type::percent, "modifier_pension_level")                            \
	MOD_LIST_ELEMENT(41, factory_input, false, modifier_display_type::percent, "modifier_factory_input")                           \
	MOD_LIST_ELEMENT(42, factory_output, true, modifier_display_type::percent, "modifier_factory_output")                          \
	MOD_LIST_ELEMENT(43, factory_throughput, true, modifier_display_type::percent, "modifier_factory_throughput")                  \
	MOD_LIST_ELEMENT(44, rgo_input, true, modifier_display_type::percent, "modifier_rgo_input")                                    \
	MOD_LIST_ELEMENT(45, rgo_output, true, modifier_display_type::percent, "modifier_rgo_output")                                  \
	MOD_LIST_ELEMENT(46, rgo_throughput, true, modifier_display_type::percent, "modifier_rgo_throughput")                          \
	MOD_LIST_ELEMENT(47, artisan_input, false, modifier_display_type::percent, "modifier_artisan_input")                           \
	MOD_LIST_ELEMENT(48, artisan_output, true, modifier_display_type::percent, "modifier_artisan_output")                          \
	MOD_LIST_ELEMENT(49, artisan_throughput, true, modifier_display_type::percent, "modifier_artisan_throughput")                  \
	MOD_LIST_ELEMENT(50, goods_demand, true, modifier_display_type::percent, "modifier_goods_demand")                              \
	MOD_LIST_ELEMENT(51, badboy, false, modifier_display_type::fp_two_places, "modifier_badboy")                                   \
	MOD_LIST_ELEMENT(52, global_assimilation_rate, true, modifier_display_type::percent, "modifier_assimilation_rate")      \
	MOD_LIST_ELEMENT(53, prestige, true, modifier_display_type::percent, "modifier_prestige")                                      \
	MOD_LIST_ELEMENT(54, factory_cost, false, modifier_display_type::percent, "modifier_factory_cost")                             \
	MOD_LIST_ELEMENT(55, farm_rgo_eff, true, modifier_display_type::percent, "tech_farm_output")                              \
	MOD_LIST_ELEMENT(56, mine_rgo_eff, true, modifier_display_type::percent, "tech_mine_output")                              \
	MOD_LIST_ELEMENT(57, farm_rgo_size, true, modifier_display_type::percent, "modifier_farm_rgo_size")                            \
	MOD_LIST_ELEMENT(58, mine_rgo_size, true, modifier_display_type::percent, "modifier_mine_rgo_size")                            \
	MOD_LIST_ELEMENT(59, issue_change_speed, true, modifier_display_type::percent, "modifier_issue_change_speed")                  \
	MOD_LIST_ELEMENT(60, social_reform_desire, true, modifier_display_type::percent, "modifier_social_reform_desire")     \
	MOD_LIST_ELEMENT(61, political_reform_desire, true, modifier_display_type::percent, "modifier_political_reform_desire")     \
	MOD_LIST_ELEMENT(62, literacy_con_impact, true, modifier_display_type::percent, "modifier_literacy_con_impact")                \
	MOD_LIST_ELEMENT(63, rich_income_modifier, true, modifier_display_type::percent, "modifier_rich_income_modifier")              \
	MOD_LIST_ELEMENT(64, middle_income_modifier, true, modifier_display_type::percent, "modifier_middle_income_modifier")          \
	MOD_LIST_ELEMENT(65, poor_income_modifier, true, modifier_display_type::percent, "modifier_poor_income_modifier")              \
	MOD_LIST_ELEMENT(66, global_immigrant_attract, true, modifier_display_type::percent, "modifier_immigant_attract")      \
	MOD_LIST_ELEMENT(67, poor_savings_modifier, true, modifier_display_type::percent, "modifier_poor_savings_modifier")            \
	MOD_LIST_ELEMENT(68, influence_modifier, true, modifier_display_type::percent, "modifier_greatpower_influence_gain")           \
	MOD_LIST_ELEMENT(69, diplomatic_points_modifier, true, modifier_display_type::percent, "modifier_diplopoints_gain")            \
	MOD_LIST_ELEMENT(70, mobilization_size, true, modifier_display_type::percent, "modifier_mobilisation_size")                    \
	MOD_LIST_ELEMENT(71, global_pop_militancy_modifier, false, modifier_display_type::fp_two_places, \
			"modifier_global_pop_militancy_modifier")                                                                                  \
	MOD_LIST_ELEMENT(72, global_pop_consciousness_modifier, true, modifier_display_type::fp_two_places, \
			"modifier_global_pop_consciousness_modifier")                                                                              \
	MOD_LIST_ELEMENT(73, core_pop_militancy_modifier, false, modifier_display_type::fp_two_places,                                       \
			"modifier_core_pop_militancy_modifier")                                                                                    \
	MOD_LIST_ELEMENT(74, core_pop_consciousness_modifier, true, modifier_display_type::fp_two_places, \
			"modifier_core_pop_consciousness_modifier")                                                                                \
	MOD_LIST_ELEMENT(75, non_accepted_pop_militancy_modifier, false, modifier_display_type::fp_two_places, \
			"modifier_non_accepted_pop_militancy_modifier")                                                                            \
	MOD_LIST_ELEMENT(76, non_accepted_pop_consciousness_modifier, true, modifier_display_type::fp_two_places, \
			"modifier_non_accepted_pop_consciousness_modifier")                                                                        \
	MOD_LIST_ELEMENT(77, cb_generation_speed_modifier, true, modifier_display_type::percent, "cb_manufacture_tech")                \
	MOD_LIST_ELEMENT(78, mobilization_impact, false, modifier_display_type::percent, "modifier_mobilization_impact")               \
	MOD_LIST_ELEMENT(79, suppression_points_modifier, true, modifier_display_type::percent, "suppression_tech")                    \
	MOD_LIST_ELEMENT(80, education_efficiency_modifier, true, modifier_display_type::percent, "modifier_education_efficiency")     \
	MOD_LIST_ELEMENT(81, civilization_progress_modifier, true, modifier_display_type::percent, "modifier_civilization_progress")   \
	MOD_LIST_ELEMENT(82, administrative_efficiency_modifier, true, modifier_display_type::percent,                                 \
			"modifier_administrative_efficiency")                                                                                      \
	MOD_LIST_ELEMENT(83, land_unit_start_experience, true, modifier_display_type::percent, "modifier_land_unit_start_experience")  \
	MOD_LIST_ELEMENT(84, naval_unit_start_experience, true, modifier_display_type::percent,                                        \
			"modifier_naval_unit_start_experience")                                                                                    \
	MOD_LIST_ELEMENT(85, naval_attack_modifier, true, modifier_display_type::percent, "modifier_naval_attack")                     \
	MOD_LIST_ELEMENT(86, naval_defense_modifier, true, modifier_display_type::percent, "modifier_naval_defense")                   \
	MOD_LIST_ELEMENT(87, land_attack_modifier, true, modifier_display_type::percent, "modifier_land_attack")                       \
	MOD_LIST_ELEMENT(88, land_defense_modifier, true, modifier_display_type::percent, "modifier_land_defense")                     \
	MOD_LIST_ELEMENT(89, tariff_efficiency_modifier, true, modifier_display_type::percent, "modifier_tariff_efficiency")           \
	MOD_LIST_ELEMENT(90, max_loan_modifier, true, modifier_display_type::percent, "modifier_max_loan_amount")                      \
	MOD_LIST_ELEMENT(91, unciv_economic_modifier, true, modifier_display_type::percent, "modifier_unciv_economic")                 \
	MOD_LIST_ELEMENT(92, unciv_military_modifier, true, modifier_display_type::percent, "modifier_unciv_military")                 \
	MOD_LIST_ELEMENT(93, self_unciv_economic_modifier, true, modifier_display_type::percent, "modifier_self_unciv_economic")       \
	MOD_LIST_ELEMENT(94, self_unciv_military_modifier, true, modifier_display_type::percent, "modifier_self_unciv_military")       \
	MOD_LIST_ELEMENT(95, commerce_tech_research_bonus, true, modifier_display_type::percent, "commerce_tech_research_bonus")       \
	MOD_LIST_ELEMENT(96, army_tech_research_bonus, true, modifier_display_type::percent, "army_tech_research_bonus")               \
	MOD_LIST_ELEMENT(97, industry_tech_research_bonus, true, modifier_display_type::percent, "industry_tech_research_bonus")       \
	MOD_LIST_ELEMENT(98, navy_tech_research_bonus, true, modifier_display_type::percent, "navy_tech_research_bonus")               \
	MOD_LIST_ELEMENT(99, culture_tech_research_bonus, true, modifier_display_type::percent, "culture_tech_research_bonus")         \
	MOD_LIST_ELEMENT(100, supply_limit, true, modifier_display_type::percent, "supply_limit_tech")                                 \
	MOD_LIST_ELEMENT(101, colonial_migration, true, modifier_display_type::percent, "colonial_migration_tech")                     \
	MOD_LIST_ELEMENT(102, max_national_focus, true, modifier_display_type::integer, "tech_max_focus")                              \
	MOD_LIST_ELEMENT(103, education_efficiency, true, modifier_display_type::percent, "edu_eff_tech")                              \
	MOD_LIST_ELEMENT(104, reinforce_rate, true, modifier_display_type::percent, "reinforce_tech")                                  \
	MOD_LIST_ELEMENT(105, influence, true, modifier_display_type::percent, "tech_gp_influence")                                    \
	MOD_LIST_ELEMENT(106, dig_in_cap, true, modifier_display_type::integer, "digin_from_tech")                                     \
	MOD_LIST_ELEMENT(107, combat_width, false, modifier_display_type::integer, "combat_width_tech")                                \
	MOD_LIST_ELEMENT(108, military_tactics, true, modifier_display_type::percent, "mil_tactics_tech")                              \
	MOD_LIST_ELEMENT(109, supply_range, true, modifier_display_type::percent, "supply_range_tech")                                 \
	MOD_LIST_ELEMENT(110, regular_experience_level, true, modifier_display_type::integer, "regular_exp_tech")                      \
	MOD_LIST_ELEMENT(111, soldier_to_pop_loss, true, modifier_display_type::percent, "soldier_to_pop_loss_tech")                   \
	MOD_LIST_ELEMENT(112, naval_attrition, false, modifier_display_type::percent, "naval_attrition_tech")                           \
	MOD_LIST_ELEMENT(113, land_attrition, false, modifier_display_type::percent, "land_attrition_tech")                             \
	MOD_LIST_ELEMENT(114, pop_growth, true, modifier_display_type::fp_three_places, "tech_pop_growth")                                     \
	MOD_LIST_ELEMENT(115, colonial_life_rating, false, modifier_display_type::integer, "modifier_life_rating")                      \
	MOD_LIST_ELEMENT(116, seperatism, false, modifier_display_type::percent, "separatism_tech")                                     \
	MOD_LIST_ELEMENT(117, colonial_prestige, true, modifier_display_type::percent, "colonial_prestige_modifier_tech")              \
	MOD_LIST_ELEMENT(118, permanent_prestige, true, modifier_display_type::fp_two_places, "permanent_prestige_tech")
#define MOD_NAT_LIST_COUNT 119

namespace provincial_mod_offsets {
#define MOD_LIST_ELEMENT(num, name, green_is_negative, display_type, locale_name)                                                \
	constexpr inline dcon::provincial_modifier_value name{num};
MOD_PROV_LIST
#undef MOD_LIST_ELEMENT
constexpr inline uint32_t count = MOD_PROV_LIST_COUNT;
} // namespace provincial_mod_offsets

namespace national_mod_offsets {
#define MOD_LIST_ELEMENT(num, name, green_is_negative, display_type, locale_name)                                                \
	constexpr inline dcon::national_modifier_value name{num};
MOD_NAT_LIST
#undef MOD_LIST_ELEMENT
constexpr inline uint32_t count = MOD_NAT_LIST_COUNT;
} // namespace national_mod_offsets

struct provincial_modifier_definition {
	static constexpr uint32_t modifier_definition_size = 22;

	float values[modifier_definition_size] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	dcon::provincial_modifier_value offsets[modifier_definition_size] = {dcon::provincial_modifier_value{}};
	uint16_t padding = 0;
};
static_assert(sizeof(provincial_modifier_definition) ==
	sizeof(provincial_modifier_definition::values)
	+ sizeof(provincial_modifier_definition::offsets)
	+ sizeof(provincial_modifier_definition::padding));

struct national_modifier_definition {
	static constexpr uint32_t modifier_definition_size = 22;

	float values[modifier_definition_size] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	dcon::national_modifier_value offsets[modifier_definition_size] = {dcon::national_modifier_value{}};
	uint16_t padding = 0;
};
static_assert(sizeof(national_modifier_definition) ==
	sizeof(national_modifier_definition::values)
	+ sizeof(national_modifier_definition::offsets)
	+ sizeof(national_modifier_definition::padding));

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
	float discipline_or_evasion = 0.0f;

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
		discipline_or_evasion += other.discipline_or_evasion;
	}
	void operator-=(unit_variable_stats const& other) {
		build_time -= other.build_time;
		default_organisation -= other.default_organisation;
		maximum_speed -= other.maximum_speed;
		defence_or_hull -= other.defence_or_hull;
		attack_or_gun_power -= other.attack_or_gun_power;
		supply_consumption -= other.supply_consumption;
		support -= other.support;
		siege_or_torpedo_attack -= other.siege_or_torpedo_attack;
		reconnaissance_or_fire_range -= other.reconnaissance_or_fire_range;
		discipline_or_evasion -= other.discipline_or_evasion;
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

// restores values after loading a save
void repopulate_modifier_effects(sys::state& state);

void update_modifier_effects(sys::state& state);
void update_single_nation_modifiers(sys::state& state, dcon::nation_id n);

void add_modifier_to_nation(sys::state& state, dcon::nation_id target_nation, dcon::modifier_id mod_id,
		sys::date expiration); // default construct date for no expiration
void add_modifier_to_province(sys::state& state, dcon::province_id target_prov, dcon::modifier_id mod_id,
		sys::date expiration); // default construct date for no expiration
void remove_modifier_from_nation(sys::state& state, dcon::nation_id target_nation, dcon::modifier_id mod_id);
void remove_modifier_from_province(sys::state& state, dcon::province_id target_prov, dcon::modifier_id mod_id);
void remove_expired_modifiers_from_nation(sys::state& state, dcon::nation_id target_nation);
void remove_expired_modifiers_from_province(sys::state& state, dcon::province_id target_prov);

void toggle_modifier_from_province(sys::state& state, dcon::province_id target_prov, dcon::modifier_id mod_id, sys::date expiration);

} // namespace sys
