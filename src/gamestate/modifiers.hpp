#pragma once
#include <stdint.h>
#include "date_interface.hpp"
#include "dcon_generated_ids.hpp"
#include "system_state_forward.hpp"

namespace sys {


enum class modifier_display_type : uint8_t {
	integer,
	percent,
	fp_two_places,
	fp_three_places,
	yesno,
	percent_two_places,
	multiplier_two_places
};

enum class modifier_color_type : uint8_t {
	positive_is_green,
	positive_is_red,
	less_than_one_is_red
};
enum class modifier_operation : uint8_t {
	add,
	mul
};
struct modifier_type_metadata {
	modifier_color_type color_type; // Color type of the modifier value
	modifier_display_type type; // Display type of the modifier value
	modifier_operation op; // The math operation to use on the existing modifier value when a new modifier is added
	float start_value; // Start value of the modifier. On each modifier update the type is set to this value before modifiers are re-applied. Most should start with 0.0f, but a multiplier/percent type modifier should probably start with 1.0f
	std::string_view name; // loc name of the modifier type
};




// cat t.cpp | awk '{print $1 $6 ", " $5 ", " $2 " " $3 " " $4}' >t2.cpp
#define MOD_PROV_LIST                                                                                                            \
	MOD_LIST_ELEMENT(0, supply_limit, modifier_color_type::positive_is_green, modifier_display_type::integer, modifier_operation::add, 0.0f, "modifier_supply_limit")                               \
	MOD_LIST_ELEMENT(1, attrition, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_attrition")                                    \
	MOD_LIST_ELEMENT(2, max_attrition, modifier_color_type::positive_is_red, modifier_display_type::integer, modifier_operation::add, 0.0f, "modifier_max_attrition")                            \
	MOD_LIST_ELEMENT(3, local_ruling_party_support, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_ruling_party_support")   \
	MOD_LIST_ELEMENT(4, poor_life_needs, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_poor_life_needs")                        \
	MOD_LIST_ELEMENT(5, rich_life_needs, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_rich_life_needs")                        \
	MOD_LIST_ELEMENT(6, middle_life_needs, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_middle_life_needs")                    \
	MOD_LIST_ELEMENT(7, poor_everyday_needs, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_poor_everyday_needs")                \
	MOD_LIST_ELEMENT(8, rich_everyday_needs, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_rich_everyday_needs")                \
	MOD_LIST_ELEMENT(9, middle_everyday_needs, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_middle_everyday_needs")            \
	MOD_LIST_ELEMENT(10, poor_luxury_needs, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_poor_luxury_needs")                   \
	MOD_LIST_ELEMENT(11, middle_luxury_needs, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_middle_luxury_needs")               \
	MOD_LIST_ELEMENT(12, rich_luxury_needs, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_rich_luxury_needs")                   \
	MOD_LIST_ELEMENT(13, population_growth, modifier_color_type::positive_is_green, modifier_display_type::percent_two_places, modifier_operation::add, 0.0f, "modifier_population_growth")            \
	MOD_LIST_ELEMENT(14, local_factory_input, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_factory_input")               \
	MOD_LIST_ELEMENT(15, local_factory_output, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_factory_output")              \
	MOD_LIST_ELEMENT(16, local_factory_throughput, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_factory_throughput")      \
	MOD_LIST_ELEMENT(17, local_rgo_input, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_rgo_input")                        \
	MOD_LIST_ELEMENT(18, local_rgo_output, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_rgo_output")                      \
	MOD_LIST_ELEMENT(19, local_rgo_throughput, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_rgo_throughput")              \
	MOD_LIST_ELEMENT(20, local_artisan_input, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_artisan_input")               \
	MOD_LIST_ELEMENT(21, local_artisan_output, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_artisan_output")              \
	MOD_LIST_ELEMENT(22, local_artisan_throughput, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_artisan_throughput")      \
	MOD_LIST_ELEMENT(23, number_of_voters, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_number_of_voters")                      \
	MOD_LIST_ELEMENT(24, goods_demand, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_goods_demand")                              \
	MOD_LIST_ELEMENT(25, assimilation_rate, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_assimilation_rate")                    \
	MOD_LIST_ELEMENT(26, life_rating, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_life_rating")                                \
	MOD_LIST_ELEMENT(27, farm_rgo_eff, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_farm_efficiency")                           \
	MOD_LIST_ELEMENT(28, mine_rgo_eff, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_mine_efficiency")                           \
	MOD_LIST_ELEMENT(29, farm_rgo_size, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_farm_size")                                \
	MOD_LIST_ELEMENT(30, mine_rgo_size, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_mine_size")                                \
	MOD_LIST_ELEMENT(31, pop_militancy_modifier, modifier_color_type::positive_is_red, modifier_display_type::fp_two_places, modifier_operation::add, 0.0f, "modifier_pop_militancy_modifier") \
	MOD_LIST_ELEMENT(32, pop_consciousness_modifier, modifier_color_type::positive_is_green, modifier_display_type::fp_two_places, modifier_operation::add, 0.0f,                             \
			"modifier_pop_consciousness_modifier")                                                                                     \
	MOD_LIST_ELEMENT(33, rich_income_modifier, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_rich_income_modifier")              \
	MOD_LIST_ELEMENT(34, middle_income_modifier, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_middle_income_modifier")          \
	MOD_LIST_ELEMENT(35, poor_income_modifier, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_poor_income_modifier")              \
	MOD_LIST_ELEMENT(36, boost_strongest_party, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_boost_strongest_party")            \
	MOD_LIST_ELEMENT(37, immigrant_attract, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_immigant_attract")                    \
	MOD_LIST_ELEMENT(38, immigrant_push, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_immigant_push")                          \
	MOD_LIST_ELEMENT(39, local_repair, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_local_repair")                              \
	MOD_LIST_ELEMENT(40, local_ship_build, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_local_ship_build")                     \
	MOD_LIST_ELEMENT(41, movement_cost, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_movement_cost")                           \
	MOD_LIST_ELEMENT(42, defense, modifier_color_type::positive_is_green, modifier_display_type::integer, modifier_operation::add, 0.0f, "defence")                                                 \
	MOD_LIST_ELEMENT(43, attack, modifier_color_type::positive_is_green, modifier_display_type::integer, modifier_operation::add, 0.0f, "attack")                                                   \
	MOD_LIST_ELEMENT(44, combat_width, modifier_color_type::positive_is_green, modifier_display_type::fp_two_places, modifier_operation::add, 0.0f, "modifier_combat_width")                              \
	MOD_LIST_ELEMENT(45, min_build_naval_base, modifier_color_type::positive_is_red, modifier_display_type::integer, modifier_operation::add, 0.0f, "naval_base_level")                          \
	MOD_LIST_ELEMENT(46, min_build_railroad, modifier_color_type::positive_is_red, modifier_display_type::integer, modifier_operation::add, 0.0f, "railroad_level")                              \
	MOD_LIST_ELEMENT(47, min_build_fort, modifier_color_type::positive_is_red, modifier_display_type::integer, modifier_operation::add, 0.0f, "fort_level") \
	MOD_LIST_ELEMENT(48, min_build_bank, modifier_color_type::positive_is_red, modifier_display_type::integer, modifier_operation::add, 0.0f, "bank_level") \
	MOD_LIST_ELEMENT(49, min_build_university, modifier_color_type::positive_is_red, modifier_display_type::integer, modifier_operation::add, 0.0f, "university_level") \
	MOD_LIST_ELEMENT(50, conversion_rate, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_conversion_rate") \
	MOD_LIST_ELEMENT(51, fort_level, modifier_color_type::positive_is_green, modifier_display_type::integer, modifier_operation::add, 0.0f, "fort_level")  \
	MOD_LIST_ELEMENT(52, supply_throughput_add, modifier_color_type::positive_is_green, modifier_display_type::fp_two_places, modifier_operation::add, 0.0f, "modifier_supply_throughput")  \
	MOD_LIST_ELEMENT(53, supply_throughput_percent, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_supply_throughput")  \
	MOD_LIST_ELEMENT(54, supply_throughput_mul, modifier_color_type::less_than_one_is_red, modifier_display_type::multiplier_two_places, modifier_operation::mul, 1.0f, "modifier_supply_throughput")  \
	MOD_LIST_ELEMENT(55, supply_loss_add, modifier_color_type::positive_is_red, modifier_display_type::fp_two_places, modifier_operation::add, 0.0f, "modifier_supply_loss")  \
	MOD_LIST_ELEMENT(56, supply_loss_percent, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_supply_loss")  \
	MOD_LIST_ELEMENT(57, supply_loss_mul, modifier_color_type::less_than_one_is_red, modifier_display_type::multiplier_two_places, modifier_operation::mul, 1.0f, "modifier_supply_loss")  \
	MOD_LIST_ELEMENT(58, port_supply_capacity_add, modifier_color_type::positive_is_green, modifier_display_type::fp_two_places, modifier_operation::add, 0.0f, "modifier_port_supply_capacity")  \
	MOD_LIST_ELEMENT(59, port_supply_capacity_percent, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_port_supply_capacity")  \
	MOD_LIST_ELEMENT(60, port_supply_capacity_mul, modifier_color_type::less_than_one_is_red, modifier_display_type::multiplier_two_places, modifier_operation::mul, 1.0f, "modifier_port_supply_capacity") 
#define MOD_PROV_LIST_COUNT 61

#define MOD_NAT_LIST                                                                                                             \
	MOD_LIST_ELEMENT(0, war_exhaustion, modifier_color_type::positive_is_red, modifier_display_type::fp_two_places, modifier_operation::add, 0.0f, "war_exhaustion")                             \
	MOD_LIST_ELEMENT(1, max_war_exhaustion, modifier_color_type::positive_is_green, modifier_display_type::integer, modifier_operation::add, 0.0f, "max_war_exhaustion")                            \
	MOD_LIST_ELEMENT(2, leadership, modifier_color_type::positive_is_green, modifier_display_type::fp_two_places, modifier_operation::add, 0.0f, "leadership")                                            \
	MOD_LIST_ELEMENT(3, leadership_modifier, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_global_leadership_modifier")          \
	MOD_LIST_ELEMENT(4, supply_consumption, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_supply_consumption")                  \
	MOD_LIST_ELEMENT(5, org_regain, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_org_regain")                                   \
	MOD_LIST_ELEMENT(6, reinforce_speed, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_reinforce_speed")                         \
	MOD_LIST_ELEMENT(7, land_organisation, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_land_organisation")                     \
	MOD_LIST_ELEMENT(8, naval_organisation, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_naval_organisation")                   \
	MOD_LIST_ELEMENT(9, research_points, modifier_color_type::positive_is_green, modifier_display_type::fp_two_places, modifier_operation::add, 0.0f, "modifier_research_points")                         \
	MOD_LIST_ELEMENT(10, research_points_modifier, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_research_points_modifier")      \
	MOD_LIST_ELEMENT(11, research_points_on_conquer, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_research_points_on_conquer")  \
	MOD_LIST_ELEMENT(12, import_cost, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_import_cost")                               \
	MOD_LIST_ELEMENT(13, loan_interest, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "loan_interest_tech")                               \
	MOD_LIST_ELEMENT(14, tax_efficiency, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_tax_efficiency")                          \
	MOD_LIST_ELEMENT(15, min_tax, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_min_tax")                                        \
	MOD_LIST_ELEMENT(16, max_tax, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_max_tax")                                        \
	MOD_LIST_ELEMENT(17, min_military_spending, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_min_military_spending")            \
	MOD_LIST_ELEMENT(18, max_military_spending, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_max_military_spending")            \
	MOD_LIST_ELEMENT(19, min_social_spending, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_min_social_spending")                \
	MOD_LIST_ELEMENT(20, max_social_spending, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_max_social_spending")                \
	MOD_LIST_ELEMENT(21, factory_owner_cost, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_factory_owner_cost")                 \
	MOD_LIST_ELEMENT(22, min_tariff, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_min_tariff")                                  \
	MOD_LIST_ELEMENT(23, max_tariff, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_max_tariff")                                  \
	MOD_LIST_ELEMENT(24, ruling_party_support, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_ruling_party_support")              \
	MOD_LIST_ELEMENT(25, rich_vote, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_rich_vote")                                    \
	MOD_LIST_ELEMENT(26, middle_vote, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_middle_vote")                                \
	MOD_LIST_ELEMENT(27, poor_vote, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_poor_vote")                                    \
	MOD_LIST_ELEMENT(28, minimum_wage, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_minimun_wage")                              \
	MOD_LIST_ELEMENT(29, factory_maintenance, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_factory_maintenance")               \
	MOD_LIST_ELEMENT(30, poor_life_needs, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_poor_life_needs")                       \
	MOD_LIST_ELEMENT(31, rich_life_needs, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_rich_life_needs")                       \
	MOD_LIST_ELEMENT(32, middle_life_needs, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_middle_life_needs")                   \
	MOD_LIST_ELEMENT(33, poor_everyday_needs, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_poor_everyday_needs")               \
	MOD_LIST_ELEMENT(34, rich_everyday_needs, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_rich_everyday_needs")               \
	MOD_LIST_ELEMENT(35, middle_everyday_needs, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_middle_everyday_needs")           \
	MOD_LIST_ELEMENT(36, poor_luxury_needs, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_poor_luxury_needs")                   \
	MOD_LIST_ELEMENT(37, middle_luxury_needs, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_middle_luxury_needs")               \
	MOD_LIST_ELEMENT(38, rich_luxury_needs, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_rich_luxury_needs")                   \
	MOD_LIST_ELEMENT(39, unemployment_benefit, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_unemployment_benefit")              \
	MOD_LIST_ELEMENT(40, pension_level, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_pension_level")                            \
	MOD_LIST_ELEMENT(41, factory_input, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_factory_input")                           \
	MOD_LIST_ELEMENT(42, factory_output, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_factory_output")                          \
	MOD_LIST_ELEMENT(43, factory_throughput, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_factory_throughput")                  \
	MOD_LIST_ELEMENT(44, rgo_input, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_rgo_input")                                    \
	MOD_LIST_ELEMENT(45, rgo_output, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_rgo_output")                                  \
	MOD_LIST_ELEMENT(46, rgo_throughput, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_rgo_throughput")                          \
	MOD_LIST_ELEMENT(47, artisan_input, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_artisan_input")                           \
	MOD_LIST_ELEMENT(48, artisan_output, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_artisan_output")                          \
	MOD_LIST_ELEMENT(49, artisan_throughput, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_artisan_throughput")                  \
	MOD_LIST_ELEMENT(50, goods_demand, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_goods_demand")                              \
	MOD_LIST_ELEMENT(51, badboy, modifier_color_type::positive_is_red, modifier_display_type::fp_two_places, modifier_operation::add, 0.0f, "modifier_badboy")                                   \
	MOD_LIST_ELEMENT(52, global_assimilation_rate, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_assimilation_rate")      \
	MOD_LIST_ELEMENT(53, prestige, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_prestige")                                      \
	MOD_LIST_ELEMENT(54, factory_cost, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_factory_cost")                             \
	MOD_LIST_ELEMENT(55, farm_rgo_eff, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "tech_farm_output")                              \
	MOD_LIST_ELEMENT(56, mine_rgo_eff, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f,"tech_mine_output")                              \
	MOD_LIST_ELEMENT(57, farm_rgo_size, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_farm_rgo_size")                            \
	MOD_LIST_ELEMENT(58, mine_rgo_size, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_mine_rgo_size")                            \
	MOD_LIST_ELEMENT(59, issue_change_speed, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_issue_change_speed")                  \
	MOD_LIST_ELEMENT(60, social_reform_desire, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_social_reform_desire")     \
	MOD_LIST_ELEMENT(61, political_reform_desire, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_political_reform_desire")     \
	MOD_LIST_ELEMENT(62, literacy_con_impact, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_literacy_con_impact")                \
	MOD_LIST_ELEMENT(63, rich_income_modifier, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_rich_income_modifier")              \
	MOD_LIST_ELEMENT(64, middle_income_modifier, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_middle_income_modifier")          \
	MOD_LIST_ELEMENT(65, poor_income_modifier, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_poor_income_modifier")              \
	MOD_LIST_ELEMENT(66, global_immigrant_attract, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_immigant_attract")      \
	MOD_LIST_ELEMENT(67, poor_savings_modifier, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_poor_savings_modifier")            \
	MOD_LIST_ELEMENT(68, influence_modifier, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_greatpower_influence_gain")           \
	MOD_LIST_ELEMENT(69, diplomatic_points_modifier, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_diplopoints_gain")            \
	MOD_LIST_ELEMENT(70, mobilization_size, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_mobilisation_size")                    \
	MOD_LIST_ELEMENT(71, global_pop_militancy_modifier, modifier_color_type::positive_is_red, modifier_display_type::fp_two_places, modifier_operation::add, 0.0f, \
			"modifier_global_pop_militancy_modifier")                                                                                  \
	MOD_LIST_ELEMENT(72, global_pop_consciousness_modifier, modifier_color_type::positive_is_green, modifier_display_type::fp_two_places, modifier_operation::add, 0.0f, \
			"modifier_global_pop_consciousness_modifier")                                                                              \
	MOD_LIST_ELEMENT(73, core_pop_militancy_modifier, modifier_color_type::positive_is_red, modifier_display_type::fp_two_places, modifier_operation::add, 0.0f,                                       \
			"modifier_core_pop_militancy_modifier")                                                                                    \
	MOD_LIST_ELEMENT(74, core_pop_consciousness_modifier, modifier_color_type::positive_is_green, modifier_display_type::fp_two_places, modifier_operation::add, 0.0f, \
			"modifier_core_pop_consciousness_modifier")                                                                                \
	MOD_LIST_ELEMENT(75, non_accepted_pop_militancy_modifier, modifier_color_type::positive_is_red, modifier_display_type::fp_two_places, modifier_operation::add, 0.0f, \
			"modifier_non_accepted_pop_militancy_modifier")                                                                            \
	MOD_LIST_ELEMENT(76, non_accepted_pop_consciousness_modifier, modifier_color_type::positive_is_green, modifier_display_type::fp_two_places, modifier_operation::add, 0.0f, \
			"modifier_non_accepted_pop_consciousness_modifier")                                                                        \
	MOD_LIST_ELEMENT(77, cb_generation_speed_modifier, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "cb_manufacture_tech")                \
	MOD_LIST_ELEMENT(78, mobilization_impact, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_mobilization_impact")               \
	MOD_LIST_ELEMENT(79, suppression_points_modifier, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "suppression_tech")                    \
	MOD_LIST_ELEMENT(80, education_efficiency_modifier, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_education_efficiency")     \
	MOD_LIST_ELEMENT(81, civilization_progress_modifier, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_civilization_progress")   \
	MOD_LIST_ELEMENT(82, administrative_efficiency_modifier, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f,                                \
			"modifier_administrative_efficiency")                                                                                      \
	MOD_LIST_ELEMENT(83, land_unit_start_experience, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_land_unit_start_experience")  \
	MOD_LIST_ELEMENT(84, naval_unit_start_experience, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f,                                        \
			"modifier_naval_unit_start_experience")                                                                                    \
	MOD_LIST_ELEMENT(85, naval_attack_modifier, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_naval_attack")                     \
	MOD_LIST_ELEMENT(86, naval_defense_modifier, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_naval_defense")                   \
	MOD_LIST_ELEMENT(87, land_attack_modifier, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_land_attack")                       \
	MOD_LIST_ELEMENT(88, land_defense_modifier, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_land_defense")                     \
	MOD_LIST_ELEMENT(89, tariff_efficiency_modifier, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_tariff_efficiency")           \
	MOD_LIST_ELEMENT(90, max_loan_modifier, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_max_loan_amount")                      \
	MOD_LIST_ELEMENT(91, unciv_economic_modifier, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_unciv_economic")                 \
	MOD_LIST_ELEMENT(92, unciv_military_modifier, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_unciv_military")                 \
	MOD_LIST_ELEMENT(93, self_unciv_economic_modifier, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_self_unciv_economic")       \
	MOD_LIST_ELEMENT(94, self_unciv_military_modifier, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_self_unciv_military")       \
	MOD_LIST_ELEMENT(95, commerce_tech_research_bonus, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "commerce_tech_research_bonus")       \
	MOD_LIST_ELEMENT(96, army_tech_research_bonus, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "army_tech_research_bonus")               \
	MOD_LIST_ELEMENT(97, industry_tech_research_bonus, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "industry_tech_research_bonus")       \
	MOD_LIST_ELEMENT(98, navy_tech_research_bonus, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "navy_tech_research_bonus")               \
	MOD_LIST_ELEMENT(99, culture_tech_research_bonus, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "culture_tech_research_bonus")         \
	MOD_LIST_ELEMENT(100, supply_limit, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "supply_limit_tech")                                 \
	MOD_LIST_ELEMENT(101, colonial_migration, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "colonial_migration_tech")                     \
	MOD_LIST_ELEMENT(102, max_national_focus, modifier_color_type::positive_is_green, modifier_display_type::integer, modifier_operation::add, 0.0f, "tech_max_focus")                              \
	MOD_LIST_ELEMENT(103, education_efficiency, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "edu_eff_tech")                              \
	MOD_LIST_ELEMENT(104, reinforce_rate, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "reinforce_tech")                                  \
	MOD_LIST_ELEMENT(105, influence, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "tech_gp_influence")                                    \
	MOD_LIST_ELEMENT(106, dig_in_cap, modifier_color_type::positive_is_green, modifier_display_type::integer, modifier_operation::add, 0.0f, "digin_from_tech")                                     \
	MOD_LIST_ELEMENT(107, combat_width, modifier_color_type::positive_is_red, modifier_display_type::integer, modifier_operation::add, 0.0f, "combat_width_tech")                                \
	MOD_LIST_ELEMENT(108, military_tactics, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "mil_tactics_tech")                              \
	MOD_LIST_ELEMENT(109, supply_range, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "supply_range_tech")                                 \
	MOD_LIST_ELEMENT(110, regular_experience_level, modifier_color_type::positive_is_green, modifier_display_type::integer, modifier_operation::add, 0.0f, "regular_exp_tech")                      \
	MOD_LIST_ELEMENT(111, soldier_to_pop_loss, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "soldier_to_pop_loss_tech")                   \
	MOD_LIST_ELEMENT(112, naval_attrition, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "naval_attrition_tech")                           \
	MOD_LIST_ELEMENT(113, land_attrition, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "land_attrition_tech")                             \
	MOD_LIST_ELEMENT(114, pop_growth, modifier_color_type::positive_is_green, modifier_display_type::percent_two_places, modifier_operation::add, 0.0f, "tech_pop_growth")                                     \
	MOD_LIST_ELEMENT(115, colonial_life_rating, modifier_color_type::positive_is_red, modifier_display_type::integer, modifier_operation::add, 0.0f, "modifier_life_rating")                      \
	MOD_LIST_ELEMENT(116, colonial_prestige, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "colonial_prestige_modifier_tech")              \
	MOD_LIST_ELEMENT(117, permanent_prestige, modifier_color_type::positive_is_green, modifier_display_type::fp_two_places, modifier_operation::add, 0.0f, "permanent_prestige_tech")			   \
	MOD_LIST_ELEMENT(118, global_conversion_rate, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_conversion_rate") \
	MOD_LIST_ELEMENT(119, min_domestic_investment, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_min_domestic_investment") \
	MOD_LIST_ELEMENT(120, max_domestic_investment, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_max_domestic_investment") \
	MOD_LIST_ELEMENT(121, military_theory_tech_research_bonus, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "military_theory_tech_research_bonus") \
	MOD_LIST_ELEMENT(122, population_tech_research_bonus, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "population_tech_research_bonus") \
	MOD_LIST_ELEMENT(123, diplomacy_tech_research_bonus, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "diplomacy_theory_tech_research_bonus") \
	MOD_LIST_ELEMENT(124, flavor_tech_research_bonus, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "flavor_theory_tech_research_bonus") \
	MOD_LIST_ELEMENT(125, seperatism, modifier_color_type::positive_is_red, modifier_display_type::fp_two_places, modifier_operation::add, 0.0f, "seperatism") \
	MOD_LIST_ELEMENT(126, aristocrat_reinvestment, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "aristocrat_reinvestment") \
	MOD_LIST_ELEMENT(127, capitalist_reinvestment, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "capitalist_reinvestment") \
	MOD_LIST_ELEMENT(128, middle_class_reinvestment, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "middle_class_reinvestment") \
	MOD_LIST_ELEMENT(129, farmers_reinvestment, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "farmers_reinvestment") \
	MOD_LIST_ELEMENT(130, aristocrat_savings, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "aristocrat_savings") \
	MOD_LIST_ELEMENT(131, capitalist_savings, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "capitalist_savings") \
	MOD_LIST_ELEMENT(132, middle_class_savings, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "middle_class_savings") \
	MOD_LIST_ELEMENT(133, farmers_savings, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "farmers_savings") \
	MOD_LIST_ELEMENT(134, disallow_naval_trade, modifier_color_type::positive_is_red, modifier_display_type::yesno, modifier_operation::add, 0.0f, "disallow_naval_trade") \
	MOD_LIST_ELEMENT(135, disallow_land_trade, modifier_color_type::positive_is_red, modifier_display_type::yesno, modifier_operation::add, 0.0f, "disallow_land_trade") \
	MOD_LIST_ELEMENT(136, trade_routes_attraction, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "alice_trade_attractiveness") \
	MOD_LIST_ELEMENT(137, min_land_upkeep, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_min_land_upkeep")            \
	MOD_LIST_ELEMENT(138, land_supply_speed_add, modifier_color_type::positive_is_green, modifier_display_type::fp_two_places, modifier_operation::add, 0.0f, "modifier_land_supply_speed")            \
	MOD_LIST_ELEMENT(139, land_supply_speed_percent, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_land_supply_speed")            \
	MOD_LIST_ELEMENT(140, land_supply_speed_mul, modifier_color_type::less_than_one_is_red, modifier_display_type::multiplier_two_places, modifier_operation::mul, 1.0f, "modifier_land_supply_speed")            \
	MOD_LIST_ELEMENT(141, naval_supply_speed_add, modifier_color_type::positive_is_green, modifier_display_type::fp_two_places, modifier_operation::add, 0.0f, "modifier_naval_supply_speed")            \
	MOD_LIST_ELEMENT(142, naval_supply_speed_percent, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_naval_supply_speed")            \
	MOD_LIST_ELEMENT(143, naval_supply_speed_mul, modifier_color_type::less_than_one_is_red, modifier_display_type::multiplier_two_places, modifier_operation::mul, 1.0f, "modifier_naval_supply_speed")            \
	MOD_LIST_ELEMENT(144, national_land_supply_throughput_add, modifier_color_type::positive_is_green, modifier_display_type::fp_two_places, modifier_operation::add, 0.0f, "modifier_national_land_supply_throughput")  \
	MOD_LIST_ELEMENT(145, national_land_supply_throughput_percent, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_national_land_supply_throughput")  \
	MOD_LIST_ELEMENT(146, national_land_supply_throughput_mul, modifier_color_type::less_than_one_is_red, modifier_display_type::multiplier_two_places, modifier_operation::mul, 1.0f, "modifier_national_land_supply_throughput")  \
	MOD_LIST_ELEMENT(147, national_land_supply_loss_add, modifier_color_type::positive_is_red, modifier_display_type::fp_two_places, modifier_operation::add, 0.0f, "modifier_national_land_supply_loss")  \
	MOD_LIST_ELEMENT(148, national_land_supply_loss_percent, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_national_land_supply_loss")  \
	MOD_LIST_ELEMENT(149, national_land_supply_loss_mul, modifier_color_type::less_than_one_is_red, modifier_display_type::multiplier_two_places, modifier_operation::mul, 1.0f, "modifier_national_land_supply_loss")  \
	MOD_LIST_ELEMENT(150, national_naval_supply_throughput_add, modifier_color_type::positive_is_green, modifier_display_type::fp_two_places, modifier_operation::add, 0.0f, "modifier_national_naval_supply_throughput")  \
	MOD_LIST_ELEMENT(151, national_naval_supply_throughput_percent, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_national_naval_supply_throughput")  \
	MOD_LIST_ELEMENT(152, national_naval_supply_throughput_mul, modifier_color_type::less_than_one_is_red, modifier_display_type::multiplier_two_places, modifier_operation::mul, 1.0f, "modifier_national_naval_supply_throughput")  \
	MOD_LIST_ELEMENT(153, national_naval_supply_loss_add, modifier_color_type::positive_is_red, modifier_display_type::fp_two_places, modifier_operation::add, 0.0f, "modifier_national_naval_supply_loss")  \
	MOD_LIST_ELEMENT(154, national_naval_supply_loss_percent, modifier_color_type::positive_is_red, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_national_naval_supply_loss")  \
	MOD_LIST_ELEMENT(155, national_naval_supply_loss_mul, modifier_color_type::less_than_one_is_red, modifier_display_type::multiplier_two_places, modifier_operation::mul, 1.0f, "modifier_national_naval_supply_loss")  \
	MOD_LIST_ELEMENT(156, national_port_supply_capacity_add, modifier_color_type::positive_is_green, modifier_display_type::fp_two_places, modifier_operation::add, 0.0f, "modifier_national_port_supply_capacity")  \
	MOD_LIST_ELEMENT(157, national_port_supply_capacity_percent, modifier_color_type::positive_is_green, modifier_display_type::percent, modifier_operation::add, 0.0f, "modifier_national_port_supply_capacity") \
	MOD_LIST_ELEMENT(158, national_port_supply_capacity_mul, modifier_color_type::less_than_one_is_red, modifier_display_type::multiplier_two_places, modifier_operation::mul, 1.0f, "modifier_national_port_supply_capacity")

#define MOD_NAT_LIST_COUNT 159

namespace provincial_mod_offsets {
#define MOD_LIST_ELEMENT(num, name, color_type, display_type, op, start_value, locale_name)                                                \
	constexpr inline dcon::provincial_modifier_value name{num};
MOD_PROV_LIST
#undef MOD_LIST_ELEMENT
constexpr inline uint32_t count = MOD_PROV_LIST_COUNT;
} // namespace provincial_mod_offsets

namespace national_mod_offsets {
#define MOD_LIST_ELEMENT(num, name, color_type, display_type, op, start_value, locale_name)                                                \
	constexpr inline dcon::national_modifier_value name{num};
MOD_NAT_LIST
#undef MOD_LIST_ELEMENT
constexpr inline uint32_t count = MOD_NAT_LIST_COUNT;
} // namespace national_mod_offsets

static const sys::modifier_type_metadata province_modifier_metadata[sys::provincial_mod_offsets::count] = {
#define MOD_LIST_ELEMENT(num, name, color_type, display_type, op, start_value, locale_name)                                                \
	modifier_type_metadata{color_type, display_type, op, start_value, locale_name},
		MOD_PROV_LIST
#undef MOD_LIST_ELEMENT
};
static const sys::modifier_type_metadata national_modifier_metadata[sys::national_mod_offsets::count] = {
#define MOD_LIST_ELEMENT(num, name, color_type, display_type, op, start_value, locale_name)                                                \
	modifier_type_metadata{color_type, display_type, op, start_value, locale_name},
		MOD_NAT_LIST
#undef MOD_LIST_ELEMENT
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
