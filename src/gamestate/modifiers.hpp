#pragma once
#include <stdint.h>
#include "date_interface.hpp"
#include "dcon_generated.hpp"

namespace sys {

namespace provincial_mod_offsets {
constexpr inline dcon::provincial_modifier_value supply_limit{ 0 };
constexpr inline dcon::provincial_modifier_value attrition{ 1 };
constexpr inline dcon::provincial_modifier_value max_attrition{ 2 };
constexpr inline dcon::provincial_modifier_value local_ruling_party_support{ 3 };
constexpr inline dcon::provincial_modifier_value poor_life_needs{ 4 };
constexpr inline dcon::provincial_modifier_value rich_life_needs{ 5 };
constexpr inline dcon::provincial_modifier_value middle_life_needs{ 6 };
constexpr inline dcon::provincial_modifier_value poor_everyday_needs{ 7 };
constexpr inline dcon::provincial_modifier_value rich_everyday_needs{ 8 };
constexpr inline dcon::provincial_modifier_value middle_everyday_needs{ 9 };
constexpr inline dcon::provincial_modifier_value poor_luxury_needs{ 10 };
constexpr inline dcon::provincial_modifier_value middle_luxury_needs{ 11 };
constexpr inline dcon::provincial_modifier_value rich_luxury_needs{ 12 };
constexpr inline dcon::provincial_modifier_value population_growth{ 13 };
constexpr inline dcon::provincial_modifier_value local_factory_input{ 14 };
constexpr inline dcon::provincial_modifier_value local_factory_output{ 15 };
constexpr inline dcon::provincial_modifier_value local_factory_throughput{ 16 };
constexpr inline dcon::provincial_modifier_value local_rgo_input{ 17 };
constexpr inline dcon::provincial_modifier_value local_rgo_output{ 18 };
constexpr inline dcon::provincial_modifier_value local_rgo_throughput{ 19 };
constexpr inline dcon::provincial_modifier_value local_artisan_input{ 20 };
constexpr inline dcon::provincial_modifier_value local_artisan_output{ 21 };
constexpr inline dcon::provincial_modifier_value local_artisan_throughput{ 22 };
constexpr inline dcon::provincial_modifier_value number_of_voters{ 23 };
constexpr inline dcon::provincial_modifier_value goods_demand{ 24 };
constexpr inline dcon::provincial_modifier_value assimilation_rate{ 25 };
constexpr inline dcon::provincial_modifier_value life_rating{ 26 };
constexpr inline dcon::provincial_modifier_value farm_rgo_eff{ 27 };
constexpr inline dcon::provincial_modifier_value mine_rgo_eff{ 28 };
constexpr inline dcon::provincial_modifier_value farm_rgo_size{ 29 };
constexpr inline dcon::provincial_modifier_value mine_rgo_size{ 30 };
constexpr inline dcon::provincial_modifier_value pop_militancy_modifier{ 31 };
constexpr inline dcon::provincial_modifier_value pop_consciousness_modifier{ 32 };
constexpr inline dcon::provincial_modifier_value rich_income_modifier{ 33 };
constexpr inline dcon::provincial_modifier_value middle_income_modifier{ 34 };
constexpr inline dcon::provincial_modifier_value poor_income_modifier{ 35 };
constexpr inline dcon::provincial_modifier_value boost_strongest_party{ 36 };
constexpr inline dcon::provincial_modifier_value immigrant_attract{ 37 };
constexpr inline dcon::provincial_modifier_value immigrant_push{ 38 };
constexpr inline dcon::provincial_modifier_value local_repair{ 39 };
constexpr inline dcon::provincial_modifier_value local_ship_build{ 40 };
constexpr inline dcon::provincial_modifier_value movement_cost{ 41 };
constexpr inline dcon::provincial_modifier_value defense{ 42 };
constexpr inline dcon::provincial_modifier_value attack{ 43 };
constexpr inline dcon::provincial_modifier_value combat_width{ 44 };
constexpr inline dcon::provincial_modifier_value min_build_naval_base{ 45 };
constexpr inline dcon::provincial_modifier_value min_build_railroad{ 46 };
constexpr inline dcon::provincial_modifier_value min_build_fort{ 47 };

constexpr inline uint32_t count = 48;
} 

namespace national_mod_offsets {
constexpr inline dcon::national_modifier_value war_exhaustion{0};
constexpr inline dcon::national_modifier_value max_war_exhaustion{1};
constexpr inline dcon::national_modifier_value leadership{2};
constexpr inline dcon::national_modifier_value leadership_modifier{3};
constexpr inline dcon::national_modifier_value supply_consumption{4};
constexpr inline dcon::national_modifier_value org_regain{5};
constexpr inline dcon::national_modifier_value reinforce_speed{6};
constexpr inline dcon::national_modifier_value land_organisation{7};
constexpr inline dcon::national_modifier_value naval_organisation{8};
constexpr inline dcon::national_modifier_value research_points{9};
constexpr inline dcon::national_modifier_value research_points_modifier{10};
constexpr inline dcon::national_modifier_value research_points_on_conquer{11};
constexpr inline dcon::national_modifier_value import_cost{12};
constexpr inline dcon::national_modifier_value loan_interest{13};
constexpr inline dcon::national_modifier_value tax_efficiency{14};
constexpr inline dcon::national_modifier_value min_tax{15};
constexpr inline dcon::national_modifier_value max_tax{16};
constexpr inline dcon::national_modifier_value min_military_spending{17};
constexpr inline dcon::national_modifier_value max_military_spending{18};
constexpr inline dcon::national_modifier_value min_social_spending{19};
constexpr inline dcon::national_modifier_value max_social_spending{20};
constexpr inline dcon::national_modifier_value factory_owner_cost{21};
constexpr inline dcon::national_modifier_value min_tariff{22};
constexpr inline dcon::national_modifier_value max_tariff{23};
constexpr inline dcon::national_modifier_value ruling_party_support{24};
constexpr inline dcon::national_modifier_value rich_vote{25};
constexpr inline dcon::national_modifier_value middle_vote{26};
constexpr inline dcon::national_modifier_value poor_vote{27};
constexpr inline dcon::national_modifier_value minimum_wage{28};
constexpr inline dcon::national_modifier_value factory_maintenance{29};
constexpr inline dcon::national_modifier_value poor_life_needs{30};
constexpr inline dcon::national_modifier_value rich_life_needs{31};
constexpr inline dcon::national_modifier_value middle_life_needs{32};
constexpr inline dcon::national_modifier_value poor_everyday_needs{33};
constexpr inline dcon::national_modifier_value rich_everyday_needs{34};
constexpr inline dcon::national_modifier_value middle_everyday_needs{35};
constexpr inline dcon::national_modifier_value poor_luxury_needs{36};
constexpr inline dcon::national_modifier_value middle_luxury_needs{37};
constexpr inline dcon::national_modifier_value rich_luxury_needs{38};
constexpr inline dcon::national_modifier_value unemployment_benefit{39};
constexpr inline dcon::national_modifier_value pension_level{40};
constexpr inline dcon::national_modifier_value global_population_growth{41};
constexpr inline dcon::national_modifier_value factory_input{42};
constexpr inline dcon::national_modifier_value factory_output{43};
constexpr inline dcon::national_modifier_value factory_throughput{44};
constexpr inline dcon::national_modifier_value rgo_input{45};
constexpr inline dcon::national_modifier_value rgo_output{46};
constexpr inline dcon::national_modifier_value rgo_throughput{47};
constexpr inline dcon::national_modifier_value artisan_input{48};
constexpr inline dcon::national_modifier_value artisan_output{49};
constexpr inline dcon::national_modifier_value artisan_throughput{50};
constexpr inline dcon::national_modifier_value goods_demand{51};
constexpr inline dcon::national_modifier_value badboy{52};
constexpr inline dcon::national_modifier_value global_assimilation_rate{53};
constexpr inline dcon::national_modifier_value prestige{54};
constexpr inline dcon::national_modifier_value factory_cost{55};
constexpr inline dcon::national_modifier_value farm_rgo_eff{56};
constexpr inline dcon::national_modifier_value mine_rgo_eff{57};
constexpr inline dcon::national_modifier_value farm_rgo_size{58};
constexpr inline dcon::national_modifier_value mine_rgo_size{59};
constexpr inline dcon::national_modifier_value issue_change_speed{60};
constexpr inline dcon::national_modifier_value social_reform_desire{61};
constexpr inline dcon::national_modifier_value political_reform_desire{62};
constexpr inline dcon::national_modifier_value literacy_con_impact{63};
constexpr inline dcon::national_modifier_value rich_income_modifier{64};
constexpr inline dcon::national_modifier_value middle_income_modifier{65};
constexpr inline dcon::national_modifier_value poor_income_modifier{66};
constexpr inline dcon::national_modifier_value global_immigrant_attract{67};
constexpr inline dcon::national_modifier_value poor_savings_modifier{68};
constexpr inline dcon::national_modifier_value influence_modifier{69};
constexpr inline dcon::national_modifier_value diplomatic_points_modifier{70};
constexpr inline dcon::national_modifier_value mobilization_size{71};
constexpr inline dcon::national_modifier_value global_pop_militancy_modifier{72};
constexpr inline dcon::national_modifier_value global_pop_consciousness_modifier{73};
constexpr inline dcon::national_modifier_value core_pop_militancy_modifier{74};
constexpr inline dcon::national_modifier_value core_pop_consciousness_modifier{75};
constexpr inline dcon::national_modifier_value non_accepted_pop_militancy_modifier{76};
constexpr inline dcon::national_modifier_value non_accepted_pop_consciousness_modifier{77};
constexpr inline dcon::national_modifier_value cb_generation_speed_modifier{78};
constexpr inline dcon::national_modifier_value mobilization_impact{79};
constexpr inline dcon::national_modifier_value suppression_points_modifier{80};
constexpr inline dcon::national_modifier_value education_efficiency_modifier{81};
constexpr inline dcon::national_modifier_value civilization_progress_modifier{82};
constexpr inline dcon::national_modifier_value administrative_efficiency_modifier{83};
constexpr inline dcon::national_modifier_value land_unit_start_experience{84};
constexpr inline dcon::national_modifier_value naval_unit_start_experience{85};
constexpr inline dcon::national_modifier_value naval_attack_modifier{86};
constexpr inline dcon::national_modifier_value naval_defense_modifier{87};
constexpr inline dcon::national_modifier_value land_attack_modifier{88};
constexpr inline dcon::national_modifier_value land_defense_modifier{89};
constexpr inline dcon::national_modifier_value tariff_efficiency_modifier{90};
constexpr inline dcon::national_modifier_value max_loan_modifier{91};
constexpr inline dcon::national_modifier_value unciv_economic_modifier{92};
constexpr inline dcon::national_modifier_value unciv_military_modifier{93};
constexpr inline dcon::national_modifier_value self_unciv_economic_modifier{94};
constexpr inline dcon::national_modifier_value self_unciv_military_modifier{95};
constexpr inline dcon::national_modifier_value commerce_tech_research_bonus{96};
constexpr inline dcon::national_modifier_value army_tech_research_bonus{97};
constexpr inline dcon::national_modifier_value industry_tech_research_bonus{98};
constexpr inline dcon::national_modifier_value navy_tech_research_bonus{99};
constexpr inline dcon::national_modifier_value culture_tech_research_bonus{100};
constexpr inline dcon::national_modifier_value supply_limit{101};
constexpr inline dcon::national_modifier_value colonial_migration{102};
constexpr inline dcon::national_modifier_value max_national_focus{103};
constexpr inline dcon::national_modifier_value cb_creation_speed{104};
constexpr inline dcon::national_modifier_value education_efficiency{105};
constexpr inline dcon::national_modifier_value reinforce_rate{106};
constexpr inline dcon::national_modifier_value tax_eff{107};
constexpr inline dcon::national_modifier_value administrative_efficiency{108};
constexpr inline dcon::national_modifier_value influence{109};
constexpr inline dcon::national_modifier_value dig_in_cap{110};
constexpr inline dcon::national_modifier_value combat_width{111};
constexpr inline dcon::national_modifier_value military_tactics{112};
constexpr inline dcon::national_modifier_value supply_range{113};
constexpr inline dcon::national_modifier_value regular_experience_level{114};
constexpr inline dcon::national_modifier_value soldier_to_pop_loss{115};
constexpr inline dcon::national_modifier_value naval_attrition{116};
constexpr inline dcon::national_modifier_value land_attrition{117};
constexpr inline dcon::national_modifier_value pop_growth{118};
constexpr inline dcon::national_modifier_value colonial_life_rating{119};
constexpr inline dcon::national_modifier_value seperatism{120};
constexpr inline dcon::national_modifier_value plurality{121};
constexpr inline dcon::national_modifier_value colonial_prestige{122};
constexpr inline dcon::national_modifier_value permanent_prestige{123};
constexpr inline dcon::national_modifier_value prestige_modifier{124};
constexpr inline dcon::national_modifier_value unit_start_experience{125};

constexpr inline uint32_t count = 127;
}



struct provincial_modifier_definition {
	static constexpr uint32_t modifier_definition_size = 10;

	float values[modifier_definition_size] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f };
	dcon::provincial_modifier_value offsets[modifier_definition_size] = { dcon::provincial_modifier_value{} };
};

struct national_modifier_definition {
	static constexpr uint32_t modifier_definition_size = 10;

	float values[modifier_definition_size] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f };
	dcon::national_modifier_value offsets[modifier_definition_size] = { dcon::national_modifier_value{} }; 
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

