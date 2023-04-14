#include <string_view>
#include "dcon_generated.hpp"
#include "system_state.hpp"
#include "text.hpp"
#include "script_constants.hpp"

namespace ui {

enum class modifier_display_type : uint8_t {
    integer, percent, fp_two_places, fp_three_places,
};
struct modifier_display_info {
    bool positive_is_green;
    modifier_display_type type;
    std::string_view name;
};

static const modifier_display_info province_modifier_names[47 + 1] = {
    modifier_display_info{ true, modifier_display_type::integer, "modifier_supply_limit" },// supply_limit 0
    modifier_display_info{ false, modifier_display_type::percent, "modifier_attrition" },// attrition 1
    modifier_display_info{ false, modifier_display_type::integer, "modifier_max_attrition" },// max_attrition 2
    modifier_display_info{ true, modifier_display_type::percent, "modifier_local_ruling_party_support" },// local_ruling_party_support 3
    modifier_display_info{ false, modifier_display_type::percent, "modifier_poor_life_needs" },// poor_life_needs 4
    modifier_display_info{ false, modifier_display_type::percent, "modifier_rich_life_needs" },// rich_life_needs 5
    modifier_display_info{ false, modifier_display_type::percent, "modifier_middle_life_needs" },// middle_life_needs 6
    modifier_display_info{ false, modifier_display_type::percent, "modifier_poor_everyday_needs" },// poor_everyday_needs 7
    modifier_display_info{ false, modifier_display_type::percent, "modifier_rich_everyday_needs" },// rich_everyday_needs 8
    modifier_display_info{ false, modifier_display_type::percent, "modifier_middle_everyday_needs" },// middle_everyday_needs 9
    modifier_display_info{ false, modifier_display_type::percent, "modifier_poor_luxury_needs" },// poor_luxury_needs 10
    modifier_display_info{ false, modifier_display_type::percent, "modifier_middle_luxury_needs" },// middle_luxury_needs 11
    modifier_display_info{ false, modifier_display_type::percent, "modifier_rich_luxury_needs" },// rich_luxury_needs 12
    modifier_display_info{ true, modifier_display_type::fp_three_places, "modifier_population_growth" },// population_growth 13
    modifier_display_info{ false, modifier_display_type::percent, "modifier_local_factory_input" },// local_factory_input 14
    modifier_display_info{ true, modifier_display_type::percent, "modifier_local_factory_output" },// local_factory_output 15
    modifier_display_info{ true, modifier_display_type::percent, "modifier_local_factory_throughput" },// local_factory_throughput 16
    modifier_display_info{ true, modifier_display_type::percent, "modifier_local_rgo_input" },// local_rgo_input 17
    modifier_display_info{ true, modifier_display_type::percent, "modifier_local_rgo_output" },// local_rgo_output 18
    modifier_display_info{ true, modifier_display_type::percent, "modifier_local_rgo_throughput" },// local_rgo_throughput 19
    modifier_display_info{ false, modifier_display_type::percent, "modifier_local_artisan_input" },// local_artisan_input 20
    modifier_display_info{ true, modifier_display_type::percent, "modifier_local_artisan_output" },// local_artisan_output 21
    modifier_display_info{ true, modifier_display_type::percent, "modifier_local_artisan_throughput" },// local_artisan_throughput 22
    modifier_display_info{ true, modifier_display_type::percent, "modifier_number_of_voters" },// number_of_voters 23
    modifier_display_info{ true, modifier_display_type::percent, "modifier_goods_demand" },// goods_demand 24
    modifier_display_info{ true, modifier_display_type::percent, "modifier_assimilation_rate" },// assimilation_rate 25
    modifier_display_info{ true, modifier_display_type::percent, "modifier_life_rating" },// life_rating 26
    modifier_display_info{ true, modifier_display_type::percent, "modifier_farm_efficiency" },// farm_rgo_eff 27
    modifier_display_info{ true, modifier_display_type::percent, "modifier_mine_efficiency" },// mine_rgo_eff 28
    modifier_display_info{ true, modifier_display_type::percent, "modifier_farm_size" },// farm_rgo_size 29
    modifier_display_info{ true, modifier_display_type::percent, "modifier_mine_size" },// mine_rgo_size 30
    modifier_display_info{ false, modifier_display_type::fp_three_places, "modifier_pop_militancy_modifier" },// pop_militancy_modifier 31
    modifier_display_info{ false, modifier_display_type::fp_three_places, "modifier_pop_consciousness_modifier" },// pop_consciousness_modifier 32
    modifier_display_info{ true, modifier_display_type::percent, "modifier_rich_income_modifier" },// rich_income_modifier 33
    modifier_display_info{ true, modifier_display_type::percent, "modifier_middle_income_modifier" },// middle_income_modifier 34
    modifier_display_info{ true, modifier_display_type::percent, "modifier_poor_income_modifier" },// poor_income_modifier 35
    modifier_display_info{ true, modifier_display_type::percent, "modifier_boost_strongest_party" },// boost_strongest_party 36
    modifier_display_info{ true, modifier_display_type::percent, "modifier_immigrant_attract" },// immigrant_attract 37
    modifier_display_info{ true, modifier_display_type::percent, "modifier_immigrant_push" },// immigrant_push 38
    modifier_display_info{ true, modifier_display_type::percent, "modifier_local_repair" },// local_repair 39
    modifier_display_info{ false, modifier_display_type::percent, "modifier_local_ship_build" },// local_ship_build 40
    modifier_display_info{ false, modifier_display_type::percent, "modifier_movement_cost" },// movement_cost 41
    modifier_display_info{ true, modifier_display_type::integer, "defence" },// defense 42
    modifier_display_info{ true, modifier_display_type::integer, "attack" },// attack 43
    modifier_display_info{ false, modifier_display_type::percent, "modifier_combat_width" },// combat_width 44
    modifier_display_info{ false, modifier_display_type::integer, "naval_base_level" },// min_build_naval_base 45
    modifier_display_info{ false, modifier_display_type::integer, "railroad_level" },// min_build_railroad 46
    modifier_display_info{ false, modifier_display_type::integer, "fort_level" },// min_build_fort 47
};
static const modifier_display_info national_modifier_names[101 + 1] = {
    modifier_display_info{ false, modifier_display_type::fp_two_places, "war_exhaustion" },// war_exhaustion 0
    modifier_display_info{ true, modifier_display_type::integer, "max_war_exhaustion" },// max_war_exhaustion 1
    modifier_display_info{ true, modifier_display_type::integer, "leadership" },// leadership 2
    modifier_display_info{ true, modifier_display_type::percent, "modifier_global_leadership_modifier" },// leadership_modifier 3
    modifier_display_info{ false, modifier_display_type::percent, "modifier_supply_consumption" },// supply_consumption 4
    modifier_display_info{ true, modifier_display_type::percent, "modifier_org_regain" },// org_regain 5
    modifier_display_info{ true, modifier_display_type::percent, "modifier_reinforce_speed" },// reinforce_speed 6
    modifier_display_info{ true, modifier_display_type::percent, "modifier_land_organisation" },// land_organisation 7
    modifier_display_info{ true, modifier_display_type::percent, "modifier_naval_organisation" },// naval_organisation 8
    modifier_display_info{ true, modifier_display_type::fp_two_places, "modifier_research_points" },// research_points 9
    modifier_display_info{ true, modifier_display_type::percent, "modifier_research_points_modifier" },// research_points_modifier 10
    modifier_display_info{ true, modifier_display_type::percent, "modifier_research_points_on_conquer" },// research_points_on_conquer 11
    modifier_display_info{ false, modifier_display_type::percent, "modifier_import_cost" },// import_cost 12
    modifier_display_info{ false, modifier_display_type::percent, "modifier_loan_interest" },// loan_interest 13
    modifier_display_info{ true, modifier_display_type::percent, "modifier_tax_efficiency" },// tax_efficiency 14
    modifier_display_info{ true, modifier_display_type::percent, "modifier_min_tax" },// min_tax 15
    modifier_display_info{ true, modifier_display_type::percent, "modifier_max_tax" },// max_tax 16
    modifier_display_info{ true, modifier_display_type::percent, "modifier_min_military_spending" },// min_military_spending 17
    modifier_display_info{ true, modifier_display_type::percent, "modifier_max_military_spending" },// max_military_spending 18
    modifier_display_info{ true, modifier_display_type::percent, "modifier_min_social_spending" },// min_social_spending 19
    modifier_display_info{ true, modifier_display_type::percent, "modifier_max_social_spending" },// max_social_spending 20
    modifier_display_info{ false, modifier_display_type::percent, "modifier_factory_owner_cost" },// factory_owner_cost 21
    modifier_display_info{ true, modifier_display_type::percent, "modifier_min_tariff" },// min_tariff 22
    modifier_display_info{ true, modifier_display_type::percent, "modifier_max_tariff" },// max_tariff 23
    modifier_display_info{ true, modifier_display_type::percent, "modifier_ruling_party_support" },// ruling_party_support 24
    modifier_display_info{ true, modifier_display_type::percent, "modifier_rich_vote" },// rich_vote 25
    modifier_display_info{ true, modifier_display_type::percent, "modifier_middle_vote" },// middle_vote 26
    modifier_display_info{ true, modifier_display_type::percent, "modifier_poor_vote" },// poor_vote 27
    modifier_display_info{ true, modifier_display_type::percent, "modifier_minimum_wage" },// minimum_wage 28
    modifier_display_info{ false, modifier_display_type::percent, "modifier_factory_maintenance" },// factory_maintenance 29
    modifier_display_info{ false, modifier_display_type::percent, "modifier_poor_life_needs" },// poor_life_needs 30
    modifier_display_info{ false, modifier_display_type::percent, "modifier_rich_life_needs" },// rich_life_needs 31
    modifier_display_info{ false, modifier_display_type::percent, "modifier_middle_life_needs" },// middle_life_needs 32
    modifier_display_info{ false, modifier_display_type::percent, "modifier_poor_everyday_needs" },// poor_everyday_needs 33
    modifier_display_info{ false, modifier_display_type::percent, "modifier_rich_everyday_needs" },// rich_everyday_needs 34
    modifier_display_info{ false, modifier_display_type::percent, "modifier_middle_everyday_needs" },// middle_everyday_needs 35
    modifier_display_info{ false, modifier_display_type::percent, "modifier_poor_luxury_needs" },// poor_luxury_needs 36
    modifier_display_info{ false, modifier_display_type::percent, "modifier_middle_luxury_needs" },// middle_luxury_needs 37
    modifier_display_info{ false, modifier_display_type::percent, "modifier_rich_luxury_needs" },// rich_luxury_needs 38
    modifier_display_info{ true, modifier_display_type::percent, "modifier_unemployment_benefit" },// unemployment_benefit 39
    modifier_display_info{ true, modifier_display_type::percent, "modifier_pension_level" },// pension_level 40
    modifier_display_info{ true, modifier_display_type::fp_two_places, "modifier_global_population_growth" },// global_population_growth 41
    modifier_display_info{ false, modifier_display_type::percent, "modifier_factory_input" },// factory_input 42
    modifier_display_info{ true, modifier_display_type::percent, "modifier_factory_output" },// factory_output 43
    modifier_display_info{ true, modifier_display_type::percent, "modifier_factory_throughput" },// factory_throughput 44
    modifier_display_info{ true, modifier_display_type::percent, "modifier_rgo_input" },// rgo_input 45
    modifier_display_info{ true, modifier_display_type::percent, "modifier_rgo_output" },// rgo_output 46
    modifier_display_info{ true, modifier_display_type::percent, "modifier_rgo_throughput" },// rgo_throughput 47
    modifier_display_info{ false, modifier_display_type::percent, "modifier_artisan_input" },// artisan_input 48
    modifier_display_info{ true, modifier_display_type::percent, "modifier_artisan_output" },// artisan_output 49
    modifier_display_info{ true, modifier_display_type::percent, "modifier_artisan_throughput" },// artisan_throughput 50
    modifier_display_info{ true, modifier_display_type::percent, "modifier_goods_demand" },// goods_demand 51
    modifier_display_info{ false, modifier_display_type::fp_two_places, "modifier_badboy" },// badboy 52
    modifier_display_info{ true, modifier_display_type::percent, "modifier_global_assimilation_rate" },// global_assimilation_rate 53
    modifier_display_info{ true, modifier_display_type::percent, "modifier_prestige" },// prestige 54
    modifier_display_info{ false, modifier_display_type::percent, "modifier_factory_cost" },// factory_cost 55
    modifier_display_info{ true, modifier_display_type::percent, "modifier_farm_rgo_eff" },// farm_rgo_eff 56
    modifier_display_info{ true, modifier_display_type::percent, "modifier_mine_rgo_eff" },// mine_rgo_eff 57
    modifier_display_info{ true, modifier_display_type::percent, "modifier_farm_rgo_size" },// farm_rgo_size 58
    modifier_display_info{ true, modifier_display_type::percent, "modifier_mine_rgo_size" },// mine_rgo_size 59
    modifier_display_info{ true, modifier_display_type::percent, "modifier_issue_change_speed" },// issue_change_speed 60
    modifier_display_info{ false, modifier_display_type::fp_three_places, "modifier_social_reform_desire" },// social_reform_desire 61
    modifier_display_info{ false, modifier_display_type::fp_three_places, "modifier_political_reform_desire" },// political_reform_desire 62
    modifier_display_info{ true, modifier_display_type::percent, "modifier_literacy_con_impact" },// literacy_con_impact 63
    modifier_display_info{ true, modifier_display_type::percent, "modifier_rich_income_modifier" },// rich_income_modifier 64
    modifier_display_info{ true, modifier_display_type::percent, "modifier_middle_income_modifier" },// middle_income_modifier 65
    modifier_display_info{ true, modifier_display_type::percent, "modifier_poor_income_modifier" },// poor_income_modifier 66
    modifier_display_info{ true, modifier_display_type::percent, "modifier_global_immigrant_attract" },// global_immigrant_attract 67
    modifier_display_info{ true, modifier_display_type::percent, "modifier_poor_savings_modifier" },// poor_savings_modifier 68
    modifier_display_info{ true, modifier_display_type::percent, "modifier_greatpower_influence_gain" },// influence_modifier 69
    modifier_display_info{ true, modifier_display_type::fp_two_places, "modifier_diplopoints_gain" },// diplomatic_points_modifier 70
    modifier_display_info{ true, modifier_display_type::percent, "modifier_mobilisation_size" },// mobilisation_size 71
    modifier_display_info{ false, modifier_display_type::percent, "modifier_mobilisation_economy_impact" },// mobilisation_economy_impact 72
    modifier_display_info{ false, modifier_display_type::fp_three_places, "modifier_global_pop_militancy_modifier" },// global_pop_militancy_modifier 73
    modifier_display_info{ false, modifier_display_type::fp_three_places, "modifier_global_pop_consciousness_modifier" },// global_pop_consciousness_modifier 74
    modifier_display_info{ false, modifier_display_type::fp_three_places, "modifier_core_pop_militancy_modifier" },// core_pop_militancy_modifier 75
    modifier_display_info{ false, modifier_display_type::fp_three_places, "modifier_core_pop_consciousness_modifier" },// core_pop_consciousness_modifier 76
    modifier_display_info{ false, modifier_display_type::fp_three_places, "modifier_non_accepted_pop_militancy_modifier" },// non_accepted_pop_militancy_modifier 77
    modifier_display_info{ false, modifier_display_type::fp_three_places, "modifier_non_accepted_pop_consciousness_modifier" },// non_accepted_pop_consciousness_modifier 78
    modifier_display_info{ true, modifier_display_type::percent, "modifier_cb_generation_speed_modifier" },// cb_generation_speed_modifier 79
    modifier_display_info{ false, modifier_display_type::percent, "modifier_mobilization_impact" },// mobilization_impact 80
    modifier_display_info{ true, modifier_display_type::percent, "modifier_suppression_point_gain" },// suppression_points_modifier 81
    modifier_display_info{ true, modifier_display_type::percent, "modifier_education_efficiency" },// education_efficiency_modifier 82
    modifier_display_info{ true, modifier_display_type::percent, "modifier_civilization_progress" },// civilization_progress_modifier 83
    modifier_display_info{ true, modifier_display_type::percent, "modifier_administrative_efficiency" },// administrative_efficiency_modifier 84
    modifier_display_info{ true, modifier_display_type::percent, "modifier_land_unit_start_experience" },// land_unit_start_experience 85
    modifier_display_info{ true, modifier_display_type::percent, "modifier_naval_unit_start_experience" },// naval_unit_start_experience 86
    modifier_display_info{ true, modifier_display_type::percent, "modifier_naval_attack" },// naval_attack_modifier 87
    modifier_display_info{ true, modifier_display_type::percent, "modifier_naval_defense" },// naval_defense_modifier 88
    modifier_display_info{ true, modifier_display_type::percent, "modifier_land_attack" },// land_attack_modifier 89
    modifier_display_info{ true, modifier_display_type::percent, "modifier_land_defense" },// land_defense_modifier 90
    modifier_display_info{ true, modifier_display_type::percent, "modifier_tariff_efficiency" },// tariff_efficiency_modifier 91
    modifier_display_info{ true, modifier_display_type::percent, "modifier_max_loan_amount" },// max_loan_modifier 92
    modifier_display_info{ true, modifier_display_type::percent, "modifier_unciv_economic" },// unciv_economic_modifier 93
    modifier_display_info{ true, modifier_display_type::percent, "modifier_unciv_military" },// unciv_military_modifier 94
    modifier_display_info{ true, modifier_display_type::percent, "modifier_self_unciv_economic" },// self_unciv_economic_modifier 95
    modifier_display_info{ true, modifier_display_type::percent, "modifier_self_unciv_military" },// self_unciv_military_modifier 96
    modifier_display_info{ true, modifier_display_type::percent, "commerce_tech_research_bonus" },// commerce_tech_research_bonus 97
    modifier_display_info{ true, modifier_display_type::percent, "army_tech_research_bonus" },// army_tech_research_bonus 98
    modifier_display_info{ true, modifier_display_type::percent, "industry_tech_research_bonus" },// industry_tech_research_bonus 99
    modifier_display_info{ true, modifier_display_type::percent, "navy_tech_research_bonus" },// navy_tech_research_bonus 100
    modifier_display_info{ true, modifier_display_type::percent, "culture_tech_research_bonus" },// culture_tech_research_bonus 101
};

std::string format_modifier_value(sys::state& state, float value, modifier_display_type type) {
    switch(type) {
    case modifier_display_type::integer:
        return (value > 0.f ? "+" : "") + text::prettify(int64_t(value));
    case modifier_display_type::percent:
        return (value > 0.f ? "+" : "") + text::format_percentage(value, 1);
    case modifier_display_type::fp_two_places:
        return text::format_float(value, 2);
    case modifier_display_type::fp_three_places:
        return text::format_float(value, 3);
    }
    return "x%";
}

void modifier_description(sys::state& state, text::layout_base& layout, dcon::modifier_id mid) {
    auto fat_id = dcon::fatten(state.world, mid);

    const auto prov_def = fat_id.get_province_values();
    for(uint32_t i = 0; i < prov_def.modifier_definition_size; ++i) {
        if(prov_def.values[i] == 0.f)
            continue;
        
        auto offset = uint32_t(prov_def.offsets[i].index());
        auto data = province_modifier_names[offset];

        auto box = text::open_layout_box(layout, 0);
        text::add_to_layout_box(layout, state, box, text::produce_simple_string(state, data.name), text::text_color::white);
        text::add_to_layout_box(layout, state, box, std::string_view{ ":" }, text::text_color::white);
        text::add_space_to_layout_box(layout, state, box);
        auto color = data.positive_is_green
            ? (prov_def.values[i] > 0.f ? text::text_color::green : text::text_color::red)
                : (prov_def.values[i] > 0.f ? text::text_color::red : text::text_color::green);
        text::add_to_layout_box(layout, state, box, format_modifier_value(state, prov_def.values[i], data.type), color);
        text::close_layout_box(layout, box);
    }
    
    const auto nat_def = fat_id.get_national_values();
    for(uint32_t i = 0; i < nat_def.modifier_definition_size; ++i) {
        if(nat_def.values[i] == 0.f)
            continue;
        
        auto offset = uint32_t(nat_def.offsets[i].index());
        auto data = national_modifier_names[offset];

        auto box = text::open_layout_box(layout, 0);
        text::add_to_layout_box(layout, state, box, text::produce_simple_string(state, data.name), text::text_color::white);
        text::add_to_layout_box(layout, state, box, std::string_view{ ":" }, text::text_color::white);
        text::add_space_to_layout_box(layout, state, box);
        auto color = data.positive_is_green
            ? (nat_def.values[i] > 0.f ? text::text_color::green : text::text_color::red)
                : (nat_def.values[i] > 0.f ? text::text_color::red : text::text_color::green);
        text::add_to_layout_box(layout, state, box, format_modifier_value(state, nat_def.values[i], data.type), color);
        text::close_layout_box(layout, box);
    }
}

}
