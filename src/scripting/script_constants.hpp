#pragma once

#include <stdint.h>
#include <cstring>

#include "dcon_generated.hpp"

namespace effect {
// flags
constexpr inline uint16_t no_payload = 0x4000;
constexpr inline uint16_t is_random_scope = 0x2000;
constexpr inline uint16_t scope_has_limit = 0x1000;

constexpr inline uint16_t code_mask = 0x0FFF;

// non scopes
#define EFFECT_BYTECODE_LIST \
EFFECT_BYTECODE_ELEMENT(0x0001, capital, 1) \
EFFECT_BYTECODE_ELEMENT(0x0002, add_core_tag, 1) \
EFFECT_BYTECODE_ELEMENT(0x0003, add_core_int, 1) \
EFFECT_BYTECODE_ELEMENT(0x0004, add_core_this_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x0005, add_core_this_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x0006, add_core_this_state, 0) \
EFFECT_BYTECODE_ELEMENT(0x0007, add_core_this_pop, 0) \
EFFECT_BYTECODE_ELEMENT(0x0008, add_core_from_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x0009, add_core_from_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x000A, add_core_reb, 0) \
EFFECT_BYTECODE_ELEMENT(0x000B, remove_core_tag, 1) \
EFFECT_BYTECODE_ELEMENT(0x000C, remove_core_int, 1) \
EFFECT_BYTECODE_ELEMENT(0x000D, remove_core_this_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x000E, remove_core_this_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x000F, remove_core_this_state, 0) \
EFFECT_BYTECODE_ELEMENT(0x0010, remove_core_this_pop, 0) \
EFFECT_BYTECODE_ELEMENT(0x0011, remove_core_from_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x0012, remove_core_from_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x0013, remove_core_reb, 0) \
EFFECT_BYTECODE_ELEMENT(0x0014, change_region_name_state, 2) \
EFFECT_BYTECODE_ELEMENT(0x0015, change_region_name_province, 2) \
EFFECT_BYTECODE_ELEMENT(0x0016, trade_goods, 1) \
EFFECT_BYTECODE_ELEMENT(0x0017, add_accepted_culture, 1) \
EFFECT_BYTECODE_ELEMENT(0x0018, add_accepted_culture_union, 0) \
EFFECT_BYTECODE_ELEMENT(0x0019, primary_culture, 1) \
EFFECT_BYTECODE_ELEMENT(0x001A, primary_culture_this_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x001B, primary_culture_this_state, 0) \
EFFECT_BYTECODE_ELEMENT(0x001C, primary_culture_this_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x001D, primary_culture_this_pop, 0) \
EFFECT_BYTECODE_ELEMENT(0x001E, primary_culture_from_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x001F, remove_accepted_culture, 1) \
EFFECT_BYTECODE_ELEMENT(0x0020, life_rating, 1) \
EFFECT_BYTECODE_ELEMENT(0x0021, religion, 1) \
EFFECT_BYTECODE_ELEMENT(0x0022, is_slave_state_yes, 0) \
EFFECT_BYTECODE_ELEMENT(0x0023, is_slave_pop_yes, 0) \
EFFECT_BYTECODE_ELEMENT(0x0024, research_points, 1) \
EFFECT_BYTECODE_ELEMENT(0x0025, tech_school, 1) \
EFFECT_BYTECODE_ELEMENT(0x0026, government, 1) \
EFFECT_BYTECODE_ELEMENT(0x0027, government_reb, 0) \
EFFECT_BYTECODE_ELEMENT(0x0028, treasury, 2) \
EFFECT_BYTECODE_ELEMENT(0x0029, war_exhaustion, 2) \
EFFECT_BYTECODE_ELEMENT(0x002A, prestige, 2) \
EFFECT_BYTECODE_ELEMENT(0x002B, change_tag, 1) \
EFFECT_BYTECODE_ELEMENT(0x002C, change_tag_culture, 0) \
EFFECT_BYTECODE_ELEMENT(0x002D, change_tag_no_core_switch, 1) \
EFFECT_BYTECODE_ELEMENT(0x002E, change_tag_no_core_switch_culture, 0) \
EFFECT_BYTECODE_ELEMENT(0x002F, set_country_flag, 1) \
EFFECT_BYTECODE_ELEMENT(0x0030, clr_country_flag, 1) \
EFFECT_BYTECODE_ELEMENT(0x0031, military_access, 1) \
EFFECT_BYTECODE_ELEMENT(0x0032, military_access_this_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x0033, military_access_this_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x0034, military_access_from_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x0035, military_access_from_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x0036, badboy, 2) \
EFFECT_BYTECODE_ELEMENT(0x0037, secede_province, 1) \
EFFECT_BYTECODE_ELEMENT(0x0038, secede_province_this_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x0039, secede_province_this_state, 0) \
EFFECT_BYTECODE_ELEMENT(0x003A, secede_province_this_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x003B, secede_province_this_pop, 0) \
EFFECT_BYTECODE_ELEMENT(0x003C, secede_province_from_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x003D, secede_province_from_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x003E, secede_province_reb, 0) \
EFFECT_BYTECODE_ELEMENT(0x003F, inherit, 1) \
EFFECT_BYTECODE_ELEMENT(0x0040, inherit_this_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x0041, inherit_this_state, 0) \
EFFECT_BYTECODE_ELEMENT(0x0042, inherit_this_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x0043, inherit_this_pop, 0) \
EFFECT_BYTECODE_ELEMENT(0x0044, inherit_from_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x0045, inherit_from_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x0046, annex_to, 1) \
EFFECT_BYTECODE_ELEMENT(0x0047, annex_to_this_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x0048, annex_to_this_state, 0) \
EFFECT_BYTECODE_ELEMENT(0x0049, annex_to_this_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x004A, annex_to_this_pop, 0) \
EFFECT_BYTECODE_ELEMENT(0x004B, annex_to_from_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x004C, annex_to_from_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x004D, release, 1) \
EFFECT_BYTECODE_ELEMENT(0x004E, release_this_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x004F, release_this_state, 0) \
EFFECT_BYTECODE_ELEMENT(0x0050, release_this_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x0051, release_this_pop, 0) \
EFFECT_BYTECODE_ELEMENT(0x0052, release_from_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x0053, release_from_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x0054, change_controller, 1) \
EFFECT_BYTECODE_ELEMENT(0x0055, change_controller_this_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x0056, change_controller_this_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x0057, change_controller_from_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x0058, change_controller_from_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x0059, infrastructure, 1) \
EFFECT_BYTECODE_ELEMENT(0x005A, money, 2) \
EFFECT_BYTECODE_ELEMENT(0x005B, leadership, 1) \
EFFECT_BYTECODE_ELEMENT(0x005C, create_vassal, 1) \
EFFECT_BYTECODE_ELEMENT(0x005D, create_vassal_this_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x005E, create_vassal_this_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x005F, create_vassal_from_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x0060, create_vassal_from_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x0061, end_military_access, 1) \
EFFECT_BYTECODE_ELEMENT(0x0062, end_military_access_this_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x0063, end_military_access_this_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x0064, end_military_access_from_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x0065, end_military_access_from_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x0066, leave_alliance, 1) \
EFFECT_BYTECODE_ELEMENT(0x0067, leave_alliance_this_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x0068, leave_alliance_this_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x0069, leave_alliance_from_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x006A, leave_alliance_from_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x006B, end_war, 1) \
EFFECT_BYTECODE_ELEMENT(0x006C, end_war_this_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x006D, end_war_this_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x006E, end_war_from_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x006F, end_war_from_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x0070, enable_ideology, 1) \
EFFECT_BYTECODE_ELEMENT(0x0071, ruling_party_ideology, 1) \
EFFECT_BYTECODE_ELEMENT(0x0072, plurality, 2) \
EFFECT_BYTECODE_ELEMENT(0x0073, remove_province_modifier, 1) \
EFFECT_BYTECODE_ELEMENT(0x0074, remove_country_modifier, 1) \
EFFECT_BYTECODE_ELEMENT(0x0075, create_alliance, 1) \
EFFECT_BYTECODE_ELEMENT(0x0076, create_alliance_this_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x0077, create_alliance_this_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x0078, create_alliance_from_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x0079, create_alliance_from_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x007A, release_vassal, 1) \
EFFECT_BYTECODE_ELEMENT(0x007B, release_vassal_this_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x007C, release_vassal_this_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x007D, release_vassal_from_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x007E, release_vassal_from_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x007F, release_vassal_reb, 0) \
EFFECT_BYTECODE_ELEMENT(0x0080, release_vassal_random, 0) \
EFFECT_BYTECODE_ELEMENT(0x0081, change_province_name, 2) \
EFFECT_BYTECODE_ELEMENT(0x0082, enable_canal, 1) \
EFFECT_BYTECODE_ELEMENT(0x0083, set_global_flag, 1) \
EFFECT_BYTECODE_ELEMENT(0x0084, clr_global_flag, 1) \
EFFECT_BYTECODE_ELEMENT(0x0085, nationalvalue_province, 1) \
EFFECT_BYTECODE_ELEMENT(0x0086, nationalvalue_nation, 1) \
EFFECT_BYTECODE_ELEMENT(0x0087, civilized_yes, 0) \
EFFECT_BYTECODE_ELEMENT(0x0088, civilized_no, 0) \
EFFECT_BYTECODE_ELEMENT(0x0089, is_slave_state_no, 0) \
EFFECT_BYTECODE_ELEMENT(0x008A, is_slave_pop_no, 0) \
EFFECT_BYTECODE_ELEMENT(0x008B, election, 0) \
EFFECT_BYTECODE_ELEMENT(0x008C, social_reform, 1) \
EFFECT_BYTECODE_ELEMENT(0x008D, political_reform, 1) \
EFFECT_BYTECODE_ELEMENT(0x008E, add_tax_relative_income, 2) \
EFFECT_BYTECODE_ELEMENT(0x008F, neutrality, 0) \
EFFECT_BYTECODE_ELEMENT(0x0090, reduce_pop, 2) \
EFFECT_BYTECODE_ELEMENT(0x0091, move_pop, 1) \
EFFECT_BYTECODE_ELEMENT(0x0092, pop_type, 1) \
EFFECT_BYTECODE_ELEMENT(0x0093, years_of_research, 2) \
EFFECT_BYTECODE_ELEMENT(0x0094, prestige_factor_positive, 2) \
EFFECT_BYTECODE_ELEMENT(0x0095, prestige_factor_negative, 2) \
EFFECT_BYTECODE_ELEMENT(0x0096, military_reform, 1) \
EFFECT_BYTECODE_ELEMENT(0x0097, economic_reform, 1) \
EFFECT_BYTECODE_ELEMENT(0x0098, remove_random_military_reforms, 1) \
EFFECT_BYTECODE_ELEMENT(0x0099, remove_random_economic_reforms, 1) \
EFFECT_BYTECODE_ELEMENT(0x009A, add_crime, 1) \
EFFECT_BYTECODE_ELEMENT(0x009B, add_crime_none, 0) \
EFFECT_BYTECODE_ELEMENT(0x009C, nationalize, 0) \
EFFECT_BYTECODE_ELEMENT(0x009D, build_factory_in_capital_state, 1) \
EFFECT_BYTECODE_ELEMENT(0x009E, activate_technology, 1) \
EFFECT_BYTECODE_ELEMENT(0x009F, great_wars_enabled_yes, 0) \
EFFECT_BYTECODE_ELEMENT(0x00A0, great_wars_enabled_no, 0) \
EFFECT_BYTECODE_ELEMENT(0x00A1, world_wars_enabled_yes, 0) \
EFFECT_BYTECODE_ELEMENT(0x00A2, world_wars_enabled_no, 0) \
EFFECT_BYTECODE_ELEMENT(0x00A3, assimilate_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x00A4, assimilate_pop, 0) \
EFFECT_BYTECODE_ELEMENT(0x00A5, literacy, 2) \
EFFECT_BYTECODE_ELEMENT(0x00A6, add_crisis_interest, 0) \
EFFECT_BYTECODE_ELEMENT(0x00A7, flashpoint_tension, 2) \
EFFECT_BYTECODE_ELEMENT(0x00A8, add_crisis_temperature, 2) \
EFFECT_BYTECODE_ELEMENT(0x00A9, consciousness, 2) \
EFFECT_BYTECODE_ELEMENT(0x00AA, militancy, 2) \
EFFECT_BYTECODE_ELEMENT(0x00AB, rgo_size, 1) \
EFFECT_BYTECODE_ELEMENT(0x00AC, fort, 1) \
EFFECT_BYTECODE_ELEMENT(0x00AD, naval_base, 1) \
EFFECT_BYTECODE_ELEMENT(0x00AE, trigger_revolt_nation, 4) \
EFFECT_BYTECODE_ELEMENT(0x00AF, trigger_revolt_state, 4) \
EFFECT_BYTECODE_ELEMENT(0x00B0, trigger_revolt_province, 4) \
EFFECT_BYTECODE_ELEMENT(0x00B1, diplomatic_influence, 2) \
EFFECT_BYTECODE_ELEMENT(0x00B2, diplomatic_influence_this_nation, 1) \
EFFECT_BYTECODE_ELEMENT(0x00B3, diplomatic_influence_this_province, 1) \
EFFECT_BYTECODE_ELEMENT(0x00B4, diplomatic_influence_from_nation, 1) \
EFFECT_BYTECODE_ELEMENT(0x00B5, diplomatic_influence_from_province, 1) \
EFFECT_BYTECODE_ELEMENT(0x00B6, relation, 2) \
EFFECT_BYTECODE_ELEMENT(0x00B7, relation_this_nation, 1) \
EFFECT_BYTECODE_ELEMENT(0x00B8, relation_this_province, 1) \
EFFECT_BYTECODE_ELEMENT(0x00B9, relation_from_nation, 1) \
EFFECT_BYTECODE_ELEMENT(0x00BA, relation_from_province, 1) \
EFFECT_BYTECODE_ELEMENT(0x00BB, add_province_modifier, 2) \
EFFECT_BYTECODE_ELEMENT(0x00BC, add_province_modifier_no_duration, 1) \
EFFECT_BYTECODE_ELEMENT(0x00BD, add_country_modifier, 2) \
EFFECT_BYTECODE_ELEMENT(0x00BE, add_country_modifier_no_duration, 1) \
EFFECT_BYTECODE_ELEMENT(0x00BF, casus_belli_tag, 3) \
EFFECT_BYTECODE_ELEMENT(0x00C0, casus_belli_int, 3) \
EFFECT_BYTECODE_ELEMENT(0x00C1, casus_belli_this_nation, 2) \
EFFECT_BYTECODE_ELEMENT(0x00C2, casus_belli_this_state, 2) \
EFFECT_BYTECODE_ELEMENT(0x00C3, casus_belli_this_province, 2) \
EFFECT_BYTECODE_ELEMENT(0x00C4, casus_belli_this_pop, 2) \
EFFECT_BYTECODE_ELEMENT(0x00C5, casus_belli_from_nation, 2) \
EFFECT_BYTECODE_ELEMENT(0x00C6, casus_belli_from_province, 2) \
EFFECT_BYTECODE_ELEMENT(0x00C7, add_casus_belli_tag, 3) \
EFFECT_BYTECODE_ELEMENT(0x00C8, add_casus_belli_int, 3) \
EFFECT_BYTECODE_ELEMENT(0x00C9, add_casus_belli_this_nation, 2) \
EFFECT_BYTECODE_ELEMENT(0x00CA, add_casus_belli_this_state, 2) \
EFFECT_BYTECODE_ELEMENT(0x00CB, add_casus_belli_this_province, 2) \
EFFECT_BYTECODE_ELEMENT(0x00CC, add_casus_belli_this_pop, 2) \
EFFECT_BYTECODE_ELEMENT(0x00CD, add_casus_belli_from_nation, 2) \
EFFECT_BYTECODE_ELEMENT(0x00CE, add_casus_belli_from_province, 2) \
EFFECT_BYTECODE_ELEMENT(0x00CF, remove_casus_belli_tag, 2) \
EFFECT_BYTECODE_ELEMENT(0x00D0, remove_casus_belli_int, 2) \
EFFECT_BYTECODE_ELEMENT(0x00D1, remove_casus_belli_this_nation, 1) \
EFFECT_BYTECODE_ELEMENT(0x00D2, remove_casus_belli_this_state, 1) \
EFFECT_BYTECODE_ELEMENT(0x00D3, remove_casus_belli_this_province, 1) \
EFFECT_BYTECODE_ELEMENT(0x00D4, remove_casus_belli_this_pop, 1) \
EFFECT_BYTECODE_ELEMENT(0x00D5, remove_casus_belli_from_nation, 1) \
EFFECT_BYTECODE_ELEMENT(0x00D6, remove_casus_belli_from_province, 1) \
EFFECT_BYTECODE_ELEMENT(0x00D7, this_remove_casus_belli_tag, 2) \
EFFECT_BYTECODE_ELEMENT(0x00D8, this_remove_casus_belli_int, 2) \
EFFECT_BYTECODE_ELEMENT(0x00D9, this_remove_casus_belli_this_nation, 1) \
EFFECT_BYTECODE_ELEMENT(0x00DA, this_remove_casus_belli_this_state, 1) \
EFFECT_BYTECODE_ELEMENT(0x00DB, this_remove_casus_belli_this_province, 1) \
EFFECT_BYTECODE_ELEMENT(0x00DC, this_remove_casus_belli_this_pop, 1) \
EFFECT_BYTECODE_ELEMENT(0x00DD, this_remove_casus_belli_from_nation, 1) \
EFFECT_BYTECODE_ELEMENT(0x00DE, this_remove_casus_belli_from_province, 1) \
EFFECT_BYTECODE_ELEMENT(0x00DF, war_tag, 7) \
EFFECT_BYTECODE_ELEMENT(0x00E0, war_this_nation, 6) \
EFFECT_BYTECODE_ELEMENT(0x00E1, war_this_state, 6) \
EFFECT_BYTECODE_ELEMENT(0x00E2, war_this_province, 6) \
EFFECT_BYTECODE_ELEMENT(0x00E3, war_this_pop, 6) \
EFFECT_BYTECODE_ELEMENT(0x00E4, war_from_nation, 6) \
EFFECT_BYTECODE_ELEMENT(0x00E5, war_from_province, 6) \
EFFECT_BYTECODE_ELEMENT(0x00E6, war_no_ally_tag, 7) \
EFFECT_BYTECODE_ELEMENT(0x00E7, war_no_ally_this_nation, 6) \
EFFECT_BYTECODE_ELEMENT(0x00E8, war_no_ally_this_state, 6) \
EFFECT_BYTECODE_ELEMENT(0x00E9, war_no_ally_this_province, 6) \
EFFECT_BYTECODE_ELEMENT(0x00EA, war_no_ally_this_pop, 6) \
EFFECT_BYTECODE_ELEMENT(0x00EB, war_no_ally_from_nation, 6) \
EFFECT_BYTECODE_ELEMENT(0x00EC, war_no_ally_from_province, 6) \
EFFECT_BYTECODE_ELEMENT(0x00ED, country_event_this_nation, 2) \
EFFECT_BYTECODE_ELEMENT(0x00EE, country_event_immediate_this_nation, 1) \
EFFECT_BYTECODE_ELEMENT(0x00EF, province_event_this_nation, 2) \
EFFECT_BYTECODE_ELEMENT(0x00F0, province_event_immediate_this_nation, 1) \
EFFECT_BYTECODE_ELEMENT(0x00F1, sub_unit_int, 2) \
EFFECT_BYTECODE_ELEMENT(0x00F2, sub_unit_this, 1) \
EFFECT_BYTECODE_ELEMENT(0x00F3, sub_unit_from, 1) \
EFFECT_BYTECODE_ELEMENT(0x00F4, sub_unit_current, 1) \
EFFECT_BYTECODE_ELEMENT(0x00F5, set_variable, 3) \
EFFECT_BYTECODE_ELEMENT(0x00F6, change_variable, 3) \
EFFECT_BYTECODE_ELEMENT(0x00F7, ideology, 3) \
EFFECT_BYTECODE_ELEMENT(0x00F8, upper_house, 3) \
EFFECT_BYTECODE_ELEMENT(0x00F9, scaled_militancy_issue, 3) \
EFFECT_BYTECODE_ELEMENT(0x00FA, scaled_militancy_ideology, 3) \
EFFECT_BYTECODE_ELEMENT(0x00FB, scaled_militancy_unemployment, 2) \
EFFECT_BYTECODE_ELEMENT(0x00FC, scaled_consciousness_issue, 3) \
EFFECT_BYTECODE_ELEMENT(0x00FD, scaled_consciousness_ideology, 3) \
EFFECT_BYTECODE_ELEMENT(0x00FE, scaled_consciousness_unemployment, 2) \
EFFECT_BYTECODE_ELEMENT(0x00FF, define_general, 4) \
EFFECT_BYTECODE_ELEMENT(0x0100, define_admiral, 4) \
EFFECT_BYTECODE_ELEMENT(0x0101, dominant_issue, 3) \
EFFECT_BYTECODE_ELEMENT(0x0102, add_war_goal, 1) \
EFFECT_BYTECODE_ELEMENT(0x0103, move_issue_percentage_nation, 4) \
EFFECT_BYTECODE_ELEMENT(0x0104, move_issue_percentage_state, 4) \
EFFECT_BYTECODE_ELEMENT(0x0105, move_issue_percentage_province, 4) \
EFFECT_BYTECODE_ELEMENT(0x0106, move_issue_percentage_pop, 4) \
EFFECT_BYTECODE_ELEMENT(0x0107, party_loyalty, 3) \
EFFECT_BYTECODE_ELEMENT(0x0108, party_loyalty_province, 2) \
EFFECT_BYTECODE_ELEMENT(0x0109, variable_tech_name_no, 1) \
EFFECT_BYTECODE_ELEMENT(0x010A, variable_invention_name_yes, 1) \
EFFECT_BYTECODE_ELEMENT(0x010B, build_railway_in_capital_yes_whole_state_yes_limit, 0) \
EFFECT_BYTECODE_ELEMENT(0x010C, build_railway_in_capital_yes_whole_state_no_limit, 0) \
EFFECT_BYTECODE_ELEMENT(0x010D, build_railway_in_capital_no_whole_state_yes_limit, 0) \
EFFECT_BYTECODE_ELEMENT(0x010E, build_railway_in_capital_no_whole_state_no_limit, 0) \
EFFECT_BYTECODE_ELEMENT(0x010F, build_fort_in_capital_yes_whole_state_yes_limit, 0) \
EFFECT_BYTECODE_ELEMENT(0x0110, build_fort_in_capital_yes_whole_state_no_limit, 0) \
EFFECT_BYTECODE_ELEMENT(0x0111, build_fort_in_capital_no_whole_state_yes_limit, 0) \
EFFECT_BYTECODE_ELEMENT(0x0112, build_fort_in_capital_no_whole_state_no_limit, 0) \
EFFECT_BYTECODE_ELEMENT(0x0113, relation_reb, 1) \
EFFECT_BYTECODE_ELEMENT(0x0114, variable_tech_name_yes, 1) \
EFFECT_BYTECODE_ELEMENT(0x0115, variable_good_name, 3) \
/*misplaced*/ \
EFFECT_BYTECODE_ELEMENT(0x0116, set_country_flag_province, 1) \
EFFECT_BYTECODE_ELEMENT(0x0117, add_country_modifier_province, 2) \
EFFECT_BYTECODE_ELEMENT(0x0118, add_country_modifier_province_no_duration, 1) \
EFFECT_BYTECODE_ELEMENT(0x0119, dominant_issue_nation, 3) \
EFFECT_BYTECODE_ELEMENT(0x011A, relation_province, 2) \
EFFECT_BYTECODE_ELEMENT(0x011B, relation_province_this_nation, 1) \
EFFECT_BYTECODE_ELEMENT(0x011C, relation_province_this_province, 1) \
EFFECT_BYTECODE_ELEMENT(0x011D, relation_province_from_nation, 1) \
EFFECT_BYTECODE_ELEMENT(0x011E, relation_province_from_province, 1) \
EFFECT_BYTECODE_ELEMENT(0x011F, relation_province_reb, 1) \
EFFECT_BYTECODE_ELEMENT(0x0120, scaled_militancy_nation_issue, 3) \
EFFECT_BYTECODE_ELEMENT(0x0121, scaled_militancy_nation_ideology, 3) \
EFFECT_BYTECODE_ELEMENT(0x0122, scaled_militancy_nation_unemployment, 2) \
EFFECT_BYTECODE_ELEMENT(0x0123, scaled_consciousness_nation_issue, 3) \
EFFECT_BYTECODE_ELEMENT(0x0124, scaled_consciousness_nation_ideology, 3) \
EFFECT_BYTECODE_ELEMENT(0x0125, scaled_consciousness_nation_unemployment, 2) \
EFFECT_BYTECODE_ELEMENT(0x0126, scaled_militancy_state_issue, 3) \
EFFECT_BYTECODE_ELEMENT(0x0127, scaled_militancy_state_ideology, 3) \
EFFECT_BYTECODE_ELEMENT(0x0128, scaled_militancy_state_unemployment, 2) \
EFFECT_BYTECODE_ELEMENT(0x0129, scaled_consciousness_state_issue, 3) \
EFFECT_BYTECODE_ELEMENT(0x012A, scaled_consciousness_state_ideology, 3) \
EFFECT_BYTECODE_ELEMENT(0x012B, scaled_consciousness_state_unemployment, 2) \
EFFECT_BYTECODE_ELEMENT(0x012C, scaled_militancy_province_issue, 3) \
EFFECT_BYTECODE_ELEMENT(0x012D, scaled_militancy_province_ideology, 3) \
EFFECT_BYTECODE_ELEMENT(0x012E, scaled_militancy_province_unemployment, 2) \
EFFECT_BYTECODE_ELEMENT(0x012F, scaled_consciousness_province_issue, 3) \
EFFECT_BYTECODE_ELEMENT(0x0130, scaled_consciousness_province_ideology, 3) \
EFFECT_BYTECODE_ELEMENT(0x0131, scaled_consciousness_province_unemployment, 2) \
EFFECT_BYTECODE_ELEMENT(0x0132, variable_good_name_province, 3) \
EFFECT_BYTECODE_ELEMENT(0x0133, treasury_province, 2) \
EFFECT_BYTECODE_ELEMENT(0x0134, country_event_this_state, 2) \
EFFECT_BYTECODE_ELEMENT(0x0135, country_event_immediate_this_state, 1) \
EFFECT_BYTECODE_ELEMENT(0x0136, province_event_this_state, 2) \
EFFECT_BYTECODE_ELEMENT(0x0137, province_event_immediate_this_state, 1) \
EFFECT_BYTECODE_ELEMENT(0x0138, country_event_this_province, 2) \
EFFECT_BYTECODE_ELEMENT(0x0139, country_event_immediate_this_province, 1) \
EFFECT_BYTECODE_ELEMENT(0x013A, province_event_this_province, 2) \
EFFECT_BYTECODE_ELEMENT(0x013B, province_event_immediate_this_province, 1) \
EFFECT_BYTECODE_ELEMENT(0x013C, country_event_this_pop, 2) \
EFFECT_BYTECODE_ELEMENT(0x013D, country_event_immediate_this_pop, 1) \
EFFECT_BYTECODE_ELEMENT(0x013E, province_event_this_pop, 2) \
EFFECT_BYTECODE_ELEMENT(0x013F, province_event_immediate_this_pop, 1) \
EFFECT_BYTECODE_ELEMENT(0x0140, country_event_province_this_nation, 2) \
EFFECT_BYTECODE_ELEMENT(0x0141, country_event_immediate_province_this_nation, 1) \
EFFECT_BYTECODE_ELEMENT(0x0142, country_event_province_this_state, 2) \
EFFECT_BYTECODE_ELEMENT(0x0143, country_event_immediate_province_this_state, 1) \
EFFECT_BYTECODE_ELEMENT(0x0144, country_event_province_this_province, 2) \
EFFECT_BYTECODE_ELEMENT(0x0145, country_event_immediate_province_this_province, 1) \
EFFECT_BYTECODE_ELEMENT(0x0146, country_event_province_this_pop, 2) \
EFFECT_BYTECODE_ELEMENT(0x0147, country_event_immediate_province_this_pop, 1) \
EFFECT_BYTECODE_ELEMENT(0x0148, activate_invention, 1) \
EFFECT_BYTECODE_ELEMENT(0x0149, variable_invention_name_no, 1) \
EFFECT_BYTECODE_ELEMENT(0x014A, add_core_tag_state, 1) \
EFFECT_BYTECODE_ELEMENT(0x014B, remove_core_tag_state, 1) \
EFFECT_BYTECODE_ELEMENT(0x014C, secede_province_state, 1) \
EFFECT_BYTECODE_ELEMENT(0x014D, assimilate_state, 0) \
EFFECT_BYTECODE_ELEMENT(0x014E, add_core_state_this_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x014F, add_core_state_this_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x0150, add_core_state_this_state, 0) \
EFFECT_BYTECODE_ELEMENT(0x0151, add_core_state_this_pop, 0) \
EFFECT_BYTECODE_ELEMENT(0x0152, add_core_state_from_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x0153, add_core_state_from_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x0154, add_core_state_reb, 0) \
EFFECT_BYTECODE_ELEMENT(0x0155, add_province_modifier_state, 2) \
EFFECT_BYTECODE_ELEMENT(0x0156, add_province_modifier_state_no_duration, 1) \
EFFECT_BYTECODE_ELEMENT(0x0157, remove_core_state_this_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x0158, remove_core_state_this_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x0159, remove_core_state_this_state, 0) \
EFFECT_BYTECODE_ELEMENT(0x015A, remove_core_state_this_pop, 0) \
EFFECT_BYTECODE_ELEMENT(0x015B, remove_core_state_from_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x015C, remove_core_state_from_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x015D, remove_core_state_reb, 0) \
EFFECT_BYTECODE_ELEMENT(0x015E, remove_province_modifier_state, 1) \
EFFECT_BYTECODE_ELEMENT(0x015F, life_rating_state, 1) \
EFFECT_BYTECODE_ELEMENT(0x0160, secede_province_state_this_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x0161, secede_province_state_this_state, 0) \
EFFECT_BYTECODE_ELEMENT(0x0162, secede_province_state_this_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x0163, secede_province_state_this_pop, 0) \
EFFECT_BYTECODE_ELEMENT(0x0164, secede_province_state_from_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x0165, secede_province_state_from_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x0166, secede_province_state_reb, 0) \
EFFECT_BYTECODE_ELEMENT(0x0167, infrastructure_state, 1) \
EFFECT_BYTECODE_ELEMENT(0x0168, fort_state, 1) \
EFFECT_BYTECODE_ELEMENT(0x0169, naval_base_state, 1) \
EFFECT_BYTECODE_ELEMENT(0x016A, is_slave_province_yes, 0) \
EFFECT_BYTECODE_ELEMENT(0x016B, is_slave_province_no, 0) \
EFFECT_BYTECODE_ELEMENT(0x016C, change_controller_state, 1) \
EFFECT_BYTECODE_ELEMENT(0x016D, change_controller_state_this_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x016E, change_controller_state_this_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x016F, change_controller_state_from_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x0170, change_controller_state_from_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x0171, reduce_pop_province, 2) \
EFFECT_BYTECODE_ELEMENT(0x0172, reduce_pop_state, 2) \
EFFECT_BYTECODE_ELEMENT(0x0173, reduce_pop_nation, 2) \
EFFECT_BYTECODE_ELEMENT(0x0174, consciousness_province, 2) \
EFFECT_BYTECODE_ELEMENT(0x0175, consciousness_state, 2) \
EFFECT_BYTECODE_ELEMENT(0x0176, consciousness_nation, 2) \
EFFECT_BYTECODE_ELEMENT(0x0177, militancy_province, 2) \
EFFECT_BYTECODE_ELEMENT(0x0178, militancy_state, 2) \
EFFECT_BYTECODE_ELEMENT(0x0179, militancy_nation, 2) \
EFFECT_BYTECODE_ELEMENT(0x017A, remove_core_tag_nation, 1) \
EFFECT_BYTECODE_ELEMENT(0x017B, remove_core_nation_this_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x017C, remove_core_nation_this_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x017D, remove_core_nation_this_state, 0) \
EFFECT_BYTECODE_ELEMENT(0x017E, remove_core_nation_this_pop, 0) \
EFFECT_BYTECODE_ELEMENT(0x017F, remove_core_nation_from_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x0180, remove_core_nation_from_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x0181, remove_core_nation_reb, 0) \
EFFECT_BYTECODE_ELEMENT(0x0182, set_country_flag_pop, 1) \
EFFECT_BYTECODE_ELEMENT(0x0183, social_reform_province, 1) \
EFFECT_BYTECODE_ELEMENT(0x0184, political_reform_province, 1) \
EFFECT_BYTECODE_ELEMENT(0x0185, flashpoint_tension_province, 2) \
EFFECT_BYTECODE_ELEMENT(0x0186, release_vassal_province, 1) \
EFFECT_BYTECODE_ELEMENT(0x0187, release_vassal_province_this_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x0188, release_vassal_province_this_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x0189, release_vassal_province_from_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x018A, release_vassal_province_from_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x018B, release_vassal_province_reb, 0) \
EFFECT_BYTECODE_ELEMENT(0x018C, release_vassal_province_random, 0) \
EFFECT_BYTECODE_ELEMENT(0x018D, build_bank_in_capital_yes_whole_state_yes_limit, 0) \
EFFECT_BYTECODE_ELEMENT(0x018E, build_bank_in_capital_yes_whole_state_no_limit, 0) \
EFFECT_BYTECODE_ELEMENT(0x018F, build_bank_in_capital_no_whole_state_yes_limit, 0) \
EFFECT_BYTECODE_ELEMENT(0x0190, build_bank_in_capital_no_whole_state_no_limit, 0) \
EFFECT_BYTECODE_ELEMENT(0x0191, build_university_in_capital_yes_whole_state_yes_limit, 0) \
EFFECT_BYTECODE_ELEMENT(0x0192, build_university_in_capital_yes_whole_state_no_limit, 0) \
EFFECT_BYTECODE_ELEMENT(0x0193, build_university_in_capital_no_whole_state_yes_limit, 0) \
EFFECT_BYTECODE_ELEMENT(0x0194, build_university_in_capital_no_whole_state_no_limit, 0) \
EFFECT_BYTECODE_ELEMENT(0x0195, bank, 1) \
EFFECT_BYTECODE_ELEMENT(0x0196, bank_state, 1) \
EFFECT_BYTECODE_ELEMENT(0x0197, university, 1) \
EFFECT_BYTECODE_ELEMENT(0x0198, university_state, 1) \
EFFECT_BYTECODE_ELEMENT(0x0199, kill_leader, 2) \
EFFECT_BYTECODE_ELEMENT(0x019A, annex_to_null_nation, 0) \
EFFECT_BYTECODE_ELEMENT(0x019B, annex_to_null_province, 0) \
EFFECT_BYTECODE_ELEMENT(0x019C, add_truce_tag, 2) \
EFFECT_BYTECODE_ELEMENT(0x019D, add_truce_this_nation, 1) \
EFFECT_BYTECODE_ELEMENT(0x019E, add_truce_this_state, 1) \
EFFECT_BYTECODE_ELEMENT(0x019F, add_truce_this_province, 1) \
EFFECT_BYTECODE_ELEMENT(0x01A0, add_truce_this_pop, 1) \
EFFECT_BYTECODE_ELEMENT(0x01A1, add_truce_from_nation, 1) \
EFFECT_BYTECODE_ELEMENT(0x01A2, add_truce_from_province, 1) \
EFFECT_BYTECODE_ELEMENT(0x01A3, call_allies, 0) \
EFFECT_BYTECODE_ELEMENT(0x01A4, ruling_party_this, 0) \
EFFECT_BYTECODE_ELEMENT(0x01A5, ruling_party_from, 0) \
EFFECT_BYTECODE_ELEMENT(0x01A6, add_accepted_culture_this, 0) \
EFFECT_BYTECODE_ELEMENT(0x01A7, add_accepted_culture_union_this, 0) \
EFFECT_BYTECODE_ELEMENT(0x01A8, add_accepted_culture_from, 0) \
EFFECT_BYTECODE_ELEMENT(0x01A9, add_accepted_culture_union_from, 0) \
/* fused ops */ \
EFFECT_BYTECODE_ELEMENT(0x01AA, fop_clr_global_flag_2, 2) \
EFFECT_BYTECODE_ELEMENT(0x01AB, fop_clr_global_flag_3, 3) \
EFFECT_BYTECODE_ELEMENT(0x01AC, fop_clr_global_flag_4, 4) \
EFFECT_BYTECODE_ELEMENT(0x01AD, fop_clr_global_flag_5, 5) \
EFFECT_BYTECODE_ELEMENT(0x01AE, fop_clr_global_flag_6, 6) \
EFFECT_BYTECODE_ELEMENT(0x01AF, fop_clr_global_flag_7, 7) \
EFFECT_BYTECODE_ELEMENT(0x01B0, fop_clr_global_flag_8, 8) \
EFFECT_BYTECODE_ELEMENT(0x01B1, fop_clr_global_flag_9, 9) \
EFFECT_BYTECODE_ELEMENT(0x01B2, fop_clr_global_flag_10, 10) \
EFFECT_BYTECODE_ELEMENT(0x01B3, fop_clr_global_flag_11, 11) \
EFFECT_BYTECODE_ELEMENT(0x01B4, fop_clr_global_flag_12, 12) \
EFFECT_BYTECODE_ELEMENT(0x01B5, fop_change_province_name, 2) \

#define EFFECT_BYTECODE_ELEMENT(code, name, arg) constexpr inline uint16_t name = code;
	EFFECT_BYTECODE_LIST
#undef EFFECT_BYTECODE_ELEMENT

// invalid
constexpr inline uint16_t first_scope_code = 0x01B6;

// scopes
constexpr inline uint16_t generic_scope = first_scope_code + 0x0000; // default grouping of effects (or hidden_tooltip)
constexpr inline uint16_t x_neighbor_province_scope = first_scope_code + 0x0001;
constexpr inline uint16_t x_neighbor_country_scope = first_scope_code + 0x0002;
constexpr inline uint16_t x_country_scope = first_scope_code + 0x0003;
constexpr inline uint16_t x_country_scope_nation = first_scope_code + 0x0004;
constexpr inline uint16_t x_empty_neighbor_province_scope = first_scope_code + 0x0005;
constexpr inline uint16_t x_greater_power_scope = first_scope_code + 0x0006;
constexpr inline uint16_t poor_strata_scope_nation = first_scope_code + 0x0007;
constexpr inline uint16_t poor_strata_scope_state = first_scope_code + 0x0008;
constexpr inline uint16_t poor_strata_scope_province = first_scope_code + 0x0009;
constexpr inline uint16_t middle_strata_scope_nation = first_scope_code + 0x000A;
constexpr inline uint16_t middle_strata_scope_state = first_scope_code + 0x000B;
constexpr inline uint16_t middle_strata_scope_province = first_scope_code + 0x000C;
constexpr inline uint16_t rich_strata_scope_nation = first_scope_code + 0x000D;
constexpr inline uint16_t rich_strata_scope_state = first_scope_code + 0x000E;
constexpr inline uint16_t rich_strata_scope_province = first_scope_code + 0x000F;
constexpr inline uint16_t x_pop_scope_nation = first_scope_code + 0x0010;
constexpr inline uint16_t x_pop_scope_state = first_scope_code + 0x0011;
constexpr inline uint16_t x_pop_scope_province = first_scope_code + 0x0012;
constexpr inline uint16_t x_owned_scope_nation = first_scope_code + 0x0013;
constexpr inline uint16_t x_owned_scope_state = first_scope_code + 0x0014;
constexpr inline uint16_t x_core_scope = first_scope_code + 0x0015;
constexpr inline uint16_t x_state_scope = first_scope_code + 0x0016;
constexpr inline uint16_t random_list_scope = first_scope_code + 0x0017;
constexpr inline uint16_t random_scope = first_scope_code + 0x0018;
constexpr inline uint16_t owner_scope_state = first_scope_code + 0x0019;
constexpr inline uint16_t owner_scope_province = first_scope_code + 0x001A;
constexpr inline uint16_t controller_scope = first_scope_code + 0x001B;
constexpr inline uint16_t location_scope = first_scope_code + 0x001C;
constexpr inline uint16_t country_scope_pop = first_scope_code + 0x001D;
constexpr inline uint16_t country_scope_state = first_scope_code + 0x001E;
constexpr inline uint16_t capital_scope = first_scope_code + 0x001F;
constexpr inline uint16_t this_scope_nation = first_scope_code + 0x0020;
constexpr inline uint16_t this_scope_state = first_scope_code + 0x0021;
constexpr inline uint16_t this_scope_province = first_scope_code + 0x0022;
constexpr inline uint16_t this_scope_pop = first_scope_code + 0x0023;
constexpr inline uint16_t from_scope_nation = first_scope_code + 0x0024;
constexpr inline uint16_t from_scope_state = first_scope_code + 0x0025;
constexpr inline uint16_t from_scope_province = first_scope_code + 0x0026;
constexpr inline uint16_t from_scope_pop = first_scope_code + 0x0027;
constexpr inline uint16_t sea_zone_scope = first_scope_code + 0x0028;
constexpr inline uint16_t cultural_union_scope = first_scope_code + 0x0029;
constexpr inline uint16_t overlord_scope = first_scope_code + 0x002A;
constexpr inline uint16_t sphere_owner_scope = first_scope_code + 0x002B;
constexpr inline uint16_t independence_scope = first_scope_code + 0x002C;
constexpr inline uint16_t flashpoint_tag_scope = first_scope_code + 0x002D;
constexpr inline uint16_t crisis_state_scope = first_scope_code + 0x002E;
constexpr inline uint16_t state_scope_pop = first_scope_code + 0x002F;
constexpr inline uint16_t state_scope_province = first_scope_code + 0x0030;
constexpr inline uint16_t x_substate_scope = first_scope_code + 0x0031;
constexpr inline uint16_t capital_scope_province = first_scope_code + 0x0032;
constexpr inline uint16_t x_core_scope_province = first_scope_code + 0x0033;

// variable named scopes
constexpr inline uint16_t tag_scope = first_scope_code + 0x0034;
constexpr inline uint16_t integer_scope = first_scope_code + 0x0035;
constexpr inline uint16_t pop_type_scope_nation = first_scope_code + 0x0036;
constexpr inline uint16_t pop_type_scope_state = first_scope_code + 0x0037;
constexpr inline uint16_t pop_type_scope_province = first_scope_code + 0x0038;
constexpr inline uint16_t region_proper_scope = first_scope_code + 0x0039;
constexpr inline uint16_t region_scope = first_scope_code + 0x003A;
constexpr inline uint16_t if_scope = first_scope_code + 0x003B;
constexpr inline uint16_t else_if_scope = first_scope_code + 0x003C;

constexpr inline uint16_t first_invalid_code = first_scope_code + 0x003D;

inline constexpr int8_t data_sizes[] = {
		0, // none
#define EFFECT_BYTECODE_ELEMENT(code, name, arg) arg,
		EFFECT_BYTECODE_LIST
#undef EFFECT_BYTECODE_ELEMENT
};
static_assert(sizeof(data_sizes) == first_scope_code);

inline int32_t get_effect_non_scope_payload_size(uint16_t const* data) {
	assert((data[0] & effect::code_mask) < effect::first_invalid_code);
	return effect::data_sizes[data[0] & effect::code_mask];
}
inline int32_t get_effect_scope_payload_size(uint16_t const* data) {
	return data[1];
}
inline int32_t get_generic_effect_payload_size(uint16_t const* data) {
	return (data[0] & effect::code_mask) >= first_scope_code ? get_effect_scope_payload_size(data)
		: get_effect_non_scope_payload_size(data);
}
inline int32_t effect_scope_data_payload(uint16_t code) {
	auto const masked_code = code & effect::code_mask;
	if((masked_code == effect::tag_scope) || (masked_code == effect::integer_scope) ||
			(masked_code == effect::pop_type_scope_nation) || (masked_code == effect::pop_type_scope_state) ||
			(masked_code == effect::pop_type_scope_province) || (masked_code == effect::region_scope) ||
			(masked_code == effect::region_proper_scope) || (masked_code == effect::random_scope))
		return 1 + ((code & effect::scope_has_limit) != 0);
	return 0 + ((code & effect::scope_has_limit) != 0);
}
inline bool effect_scope_has_single_member(uint16_t const* source) { // precondition: scope known to not be empty
	auto const data_offset = 2 + effect_scope_data_payload(source[0]);
	return get_effect_scope_payload_size(source) == data_offset + get_generic_effect_payload_size(source + data_offset);
}

} // namespace effect

//
// TRIGGERS
//

namespace trigger {
// flags
constexpr inline uint16_t is_disjunctive_scope = 0x4000;
constexpr inline uint16_t is_existence_scope = 0x2000;

constexpr inline uint16_t association_mask = 0x7000;
constexpr inline uint16_t association_eq = 0x1000;
constexpr inline uint16_t association_gt = 0x2000;
constexpr inline uint16_t association_ge = 0x3000;
constexpr inline uint16_t association_lt = 0x4000;
constexpr inline uint16_t association_le = 0x5000;
constexpr inline uint16_t association_ne = 0x6000;

constexpr inline uint16_t no_payload = 0x0800;
constexpr inline uint16_t code_mask = 0x07FF;

// non scopes
#define TRIGGER_BYTECODE_LIST \
TRIGGER_BYTECODE_ELEMENT(0x0001, year, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0002, month, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0003, port, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0004, rank, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0005, technology, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0006, strata_rich, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0007, life_rating_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0008, life_rating_state, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0009, has_empty_adjacent_state_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x000A, has_empty_adjacent_state_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x000B, state_id_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x000C, state_id_state, 1) \
TRIGGER_BYTECODE_ELEMENT(0x000D, cash_reserves, 2) \
TRIGGER_BYTECODE_ELEMENT(0x000E, unemployment_nation, 2) \
TRIGGER_BYTECODE_ELEMENT(0x000F, unemployment_state, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0010, unemployment_province, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0011, unemployment_pop, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0012, is_slave_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0013, is_slave_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0014, is_slave_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0015, is_slave_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0016, is_independant, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0017, has_national_minority_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0018, has_national_minority_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0019, has_national_minority_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x001A, government_nation, 1) \
TRIGGER_BYTECODE_ELEMENT(0x001B, government_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x001C, capital, 1) \
TRIGGER_BYTECODE_ELEMENT(0x001D, tech_school, 1) \
TRIGGER_BYTECODE_ELEMENT(0x001E, primary_culture, 1) \
TRIGGER_BYTECODE_ELEMENT(0x001F, accepted_culture, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0020, culture_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0021, culture_state, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0022, culture_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0023, culture_nation, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0024, culture_pop_reb, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0025, culture_state_reb, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0026, culture_province_reb, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0027, culture_nation_reb, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0028, culture_from_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0029, culture_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x002A, culture_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x002B, culture_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x002C, culture_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x002D, culture_group_nation, 1) \
TRIGGER_BYTECODE_ELEMENT(0x002E, culture_group_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x002F, culture_group_reb_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0030, culture_group_reb_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0031, culture_group_nation_from_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0032, culture_group_pop_from_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0033, culture_group_nation_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0034, culture_group_pop_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0035, culture_group_nation_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0036, culture_group_pop_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0037, culture_group_nation_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0038, culture_group_pop_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0039, culture_group_nation_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x003A, culture_group_pop_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x003B, religion, 1) \
TRIGGER_BYTECODE_ELEMENT(0x003C, religion_reb, 0) \
TRIGGER_BYTECODE_ELEMENT(0x003D, religion_from_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x003E, religion_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x003F, religion_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0040, religion_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0041, religion_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0042, terrain_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0043, terrain_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0044, trade_goods, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0045, is_secondary_power_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0046, is_secondary_power_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0047, has_faction_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0048, has_faction_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0049, has_unclaimed_cores, 0) \
TRIGGER_BYTECODE_ELEMENT(0x004A, is_cultural_union_bool, 0) \
TRIGGER_BYTECODE_ELEMENT(0x004B, is_cultural_union_this_self_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x004C, is_cultural_union_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x004D, is_cultural_union_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x004E, is_cultural_union_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x004F, is_cultural_union_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0050, is_cultural_union_this_rebel, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0051, is_cultural_union_tag_nation, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0052, is_cultural_union_tag_this_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0053, is_cultural_union_tag_this_state, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0054, is_cultural_union_tag_this_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0055, is_cultural_union_tag_this_nation, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0056, can_build_factory_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0057, war_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0058, war_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0059, war_exhaustion_nation, 2) \
TRIGGER_BYTECODE_ELEMENT(0x005A, blockade, 2) \
TRIGGER_BYTECODE_ELEMENT(0x005B, owns, 1) \
TRIGGER_BYTECODE_ELEMENT(0x005C, controls, 1) \
TRIGGER_BYTECODE_ELEMENT(0x005D, is_core_integer, 1) \
TRIGGER_BYTECODE_ELEMENT(0x005E, is_core_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x005F, is_core_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0060, is_core_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0061, is_core_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0062, is_core_from_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0063, is_core_reb, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0064, is_core_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0065, num_of_revolts, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0066, revolt_percentage, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0067, num_of_cities_int, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0068, num_of_cities_from_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0069, num_of_cities_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x006A, num_of_cities_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x006B, num_of_cities_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x006C, num_of_cities_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x006D, num_of_ports, 1) \
TRIGGER_BYTECODE_ELEMENT(0x006E, num_of_allies, 1) \
TRIGGER_BYTECODE_ELEMENT(0x006F, num_of_vassals, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0070, owned_by_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0071, owned_by_from_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0072, owned_by_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0073, owned_by_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0074, owned_by_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0075, owned_by_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0076, exists_bool, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0077, exists_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0078, has_country_flag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0079, continent_nation, 1) \
TRIGGER_BYTECODE_ELEMENT(0x007A, continent_state, 1) \
TRIGGER_BYTECODE_ELEMENT(0x007B, continent_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x007C, continent_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x007D, continent_nation_this, 0) \
TRIGGER_BYTECODE_ELEMENT(0x007E, continent_state_this, 0) \
TRIGGER_BYTECODE_ELEMENT(0x007F, continent_province_this, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0080, continent_pop_this, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0081, continent_nation_from, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0082, continent_state_from, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0083, continent_province_from, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0084, continent_pop_from, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0085, casus_belli_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0086, casus_belli_from, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0087, casus_belli_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0088, casus_belli_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0089, casus_belli_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x008A, casus_belli_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x008B, military_access_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x008C, military_access_from, 0) \
TRIGGER_BYTECODE_ELEMENT(0x008D, military_access_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x008E, military_access_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x008F, military_access_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0090, military_access_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0091, prestige_value, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0092, prestige_from, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0093, prestige_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0094, prestige_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0095, prestige_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0096, prestige_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0097, badboy, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0098, has_building_state, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0099, has_building_fort, 0) \
TRIGGER_BYTECODE_ELEMENT(0x009A, has_building_railroad, 0) \
TRIGGER_BYTECODE_ELEMENT(0x009B, has_building_naval_base, 0) \
TRIGGER_BYTECODE_ELEMENT(0x009C, empty, 0) \
TRIGGER_BYTECODE_ELEMENT(0x009D, is_blockaded, 0) \
TRIGGER_BYTECODE_ELEMENT(0x009E, has_country_modifier, 1) \
TRIGGER_BYTECODE_ELEMENT(0x009F, has_province_modifier, 1) \
TRIGGER_BYTECODE_ELEMENT(0x00A0, region, 1) \
TRIGGER_BYTECODE_ELEMENT(0x00A1, tag_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x00A2, tag_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00A3, tag_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00A4, tag_from_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00A5, tag_from_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00A6, neighbour_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x00A7, neighbour_this, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00A8, neighbour_from, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00A9, units_in_province_value, 1) \
TRIGGER_BYTECODE_ELEMENT(0x00AA, units_in_province_from, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00AB, units_in_province_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00AC, units_in_province_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00AD, units_in_province_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00AE, units_in_province_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00AF, war_with_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x00B0, war_with_from, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00B1, war_with_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00B2, war_with_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00B3, war_with_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00B4, war_with_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00B5, unit_in_battle, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00B6, total_amount_of_divisions, 1) \
TRIGGER_BYTECODE_ELEMENT(0x00B7, money, 2) \
TRIGGER_BYTECODE_ELEMENT(0x00B8, lost_national, 2) \
TRIGGER_BYTECODE_ELEMENT(0x00B9, is_vassal, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00BA, ruling_party_ideology_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x00BB, ruling_party_ideology_nation, 1) \
TRIGGER_BYTECODE_ELEMENT(0x00BC, ruling_party, 2) \
TRIGGER_BYTECODE_ELEMENT(0x00BD, is_ideology_enabled, 1) \
TRIGGER_BYTECODE_ELEMENT(0x00BE, political_reform_want_nation, 2) \
TRIGGER_BYTECODE_ELEMENT(0x00BF, political_reform_want_pop, 2) \
TRIGGER_BYTECODE_ELEMENT(0x00C0, social_reform_want_nation, 2) \
TRIGGER_BYTECODE_ELEMENT(0x00C1, social_reform_want_pop, 2) \
TRIGGER_BYTECODE_ELEMENT(0x00C2, total_amount_of_ships, 1) \
TRIGGER_BYTECODE_ELEMENT(0x00C3, plurality, 2) \
TRIGGER_BYTECODE_ELEMENT(0x00C4, corruption, 2) \
TRIGGER_BYTECODE_ELEMENT(0x00C5, is_state_religion_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00C6, is_state_religion_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00C7, is_state_religion_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00C8, is_primary_culture_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00C9, is_primary_culture_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00CA, is_primary_culture_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00CB, is_primary_culture_nation_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00CC, is_primary_culture_nation_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00CD, is_primary_culture_nation_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00CE, is_primary_culture_nation_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00CF, is_primary_culture_state_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00D0, is_primary_culture_state_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00D1, is_primary_culture_state_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00D2, is_primary_culture_state_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00D3, is_primary_culture_province_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00D4, is_primary_culture_province_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00D5, is_primary_culture_province_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00D6, is_primary_culture_province_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00D7, is_primary_culture_pop_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00D8, is_primary_culture_pop_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00D9, is_primary_culture_pop_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00DA, is_primary_culture_pop_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00DB, is_accepted_culture_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00DC, is_accepted_culture_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00DD, is_accepted_culture_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00DE, is_coastal_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00DF, in_sphere_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x00E0, in_sphere_from, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00E1, in_sphere_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00E2, in_sphere_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00E3, in_sphere_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00E4, in_sphere_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00E5, produces_nation, 1) \
TRIGGER_BYTECODE_ELEMENT(0x00E6, produces_state, 1) \
TRIGGER_BYTECODE_ELEMENT(0x00E7, produces_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x00E8, produces_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x00E9, average_militancy_nation, 2) \
TRIGGER_BYTECODE_ELEMENT(0x00EA, average_militancy_state, 2) \
TRIGGER_BYTECODE_ELEMENT(0x00EB, average_militancy_province, 2) \
TRIGGER_BYTECODE_ELEMENT(0x00EC, average_consciousness_nation, 2) \
TRIGGER_BYTECODE_ELEMENT(0x00ED, average_consciousness_state, 2) \
TRIGGER_BYTECODE_ELEMENT(0x00EE, average_consciousness_province, 2) \
TRIGGER_BYTECODE_ELEMENT(0x00EF, is_next_reform_nation, 1) \
TRIGGER_BYTECODE_ELEMENT(0x00F0, is_next_reform_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x00F1, rebel_power_fraction, 2) \
TRIGGER_BYTECODE_ELEMENT(0x00F2, recruited_percentage_nation, 2) \
TRIGGER_BYTECODE_ELEMENT(0x00F3, recruited_percentage_pop, 2) \
TRIGGER_BYTECODE_ELEMENT(0x00F4, has_culture_core, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00F5, nationalism, 1) \
TRIGGER_BYTECODE_ELEMENT(0x00F6, is_overseas, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00F7, controlled_by_rebels, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00F8, controlled_by_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x00F9, controlled_by_from, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00FA, controlled_by_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00FB, controlled_by_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00FC, controlled_by_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00FD, controlled_by_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00FE, controlled_by_owner, 0) \
TRIGGER_BYTECODE_ELEMENT(0x00FF, controlled_by_reb, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0100, is_canal_enabled, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0101, is_state_capital, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0102, truce_with_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0103, truce_with_from, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0104, truce_with_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0105, truce_with_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0106, truce_with_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0107, truce_with_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0108, total_pops_nation, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0109, total_pops_state, 2) \
TRIGGER_BYTECODE_ELEMENT(0x010A, total_pops_province, 2) \
TRIGGER_BYTECODE_ELEMENT(0x010B, total_pops_pop, 2) \
TRIGGER_BYTECODE_ELEMENT(0x010C, has_pop_type_nation, 1) \
TRIGGER_BYTECODE_ELEMENT(0x010D, has_pop_type_state, 1) \
TRIGGER_BYTECODE_ELEMENT(0x010E, has_pop_type_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x010F, has_pop_type_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0110, has_empty_adjacent_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0111, has_leader, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0112, ai, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0113, can_create_vassals, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0114, is_possible_vassal, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0115, province_id, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0116, vassal_of_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0117, vassal_of_from, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0118, vassal_of_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0119, vassal_of_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x011A, vassal_of_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x011B, vassal_of_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x011C, alliance_with_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x011D, alliance_with_from, 0) \
TRIGGER_BYTECODE_ELEMENT(0x011E, alliance_with_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x011F, alliance_with_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0120, alliance_with_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0121, alliance_with_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0122, has_recently_lost_war, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0123, is_mobilised, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0124, mobilisation_size, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0125, crime_higher_than_education_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0126, crime_higher_than_education_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0127, crime_higher_than_education_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0128, crime_higher_than_education_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0129, agree_with_ruling_party, 2) \
TRIGGER_BYTECODE_ELEMENT(0x012A, is_colonial_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x012B, is_colonial_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x012C, has_factories_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x012D, in_default_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x012E, in_default_from, 0) \
TRIGGER_BYTECODE_ELEMENT(0x012F, in_default_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0130, in_default_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0131, in_default_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0132, in_default_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0133, total_num_of_ports, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0134, always, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0135, election, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0136, has_global_flag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0137, is_capital, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0138, nationalvalue_nation, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0139, industrial_score_value, 1) \
TRIGGER_BYTECODE_ELEMENT(0x013A, industrial_score_from_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x013B, industrial_score_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x013C, industrial_score_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x013D, industrial_score_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x013E, industrial_score_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x013F, military_score_value, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0140, military_score_from_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0141, military_score_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0142, military_score_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0143, military_score_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0144, military_score_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0145, civilized_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0146, civilized_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0147, civilized_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0148, national_provinces_occupied, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0149, is_greater_power_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x014A, is_greater_power_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x014B, rich_tax, 1) \
TRIGGER_BYTECODE_ELEMENT(0x014C, middle_tax, 1) \
TRIGGER_BYTECODE_ELEMENT(0x014D, poor_tax, 1) \
TRIGGER_BYTECODE_ELEMENT(0x014E, social_spending_nation, 1) \
TRIGGER_BYTECODE_ELEMENT(0x014F, social_spending_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0150, social_spending_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0151, colonial_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0152, pop_majority_religion_nation, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0153, pop_majority_religion_state, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0154, pop_majority_religion_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0155, pop_majority_culture_nation, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0156, pop_majority_culture_state, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0157, pop_majority_culture_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0158, pop_majority_issue_nation, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0159, pop_majority_issue_state, 2) \
TRIGGER_BYTECODE_ELEMENT(0x015A, pop_majority_issue_province, 2) \
TRIGGER_BYTECODE_ELEMENT(0x015B, pop_majority_issue_pop, 2) \
TRIGGER_BYTECODE_ELEMENT(0x015C, pop_majority_ideology_nation, 1) \
TRIGGER_BYTECODE_ELEMENT(0x015D, pop_majority_ideology_state, 1) \
TRIGGER_BYTECODE_ELEMENT(0x015E, pop_majority_ideology_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x015F, pop_majority_ideology_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0160, poor_strata_militancy_nation, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0161, poor_strata_militancy_state, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0162, poor_strata_militancy_province, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0163, poor_strata_militancy_pop, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0164, middle_strata_militancy_nation, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0165, middle_strata_militancy_state, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0166, middle_strata_militancy_province, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0167, middle_strata_militancy_pop, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0168, rich_strata_militancy_nation, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0169, rich_strata_militancy_state, 2) \
TRIGGER_BYTECODE_ELEMENT(0x016A, rich_strata_militancy_province, 2) \
TRIGGER_BYTECODE_ELEMENT(0x016B, rich_strata_militancy_pop, 2) \
TRIGGER_BYTECODE_ELEMENT(0x016C, rich_tax_above_poor, 0) \
TRIGGER_BYTECODE_ELEMENT(0x016D, culture_has_union_tag_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x016E, culture_has_union_tag_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x016F, this_culture_union_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0170, this_culture_union_from, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0171, this_culture_union_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0172, this_culture_union_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0173, this_culture_union_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0174, this_culture_union_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0175, this_culture_union_this_union_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0176, this_culture_union_this_union_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0177, this_culture_union_this_union_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0178, this_culture_union_this_union_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0179, minorities_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x017A, minorities_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x017B, minorities_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x017C, revanchism_nation, 2) \
TRIGGER_BYTECODE_ELEMENT(0x017D, revanchism_pop, 2) \
TRIGGER_BYTECODE_ELEMENT(0x017E, has_crime, 1) \
TRIGGER_BYTECODE_ELEMENT(0x017F, num_of_substates, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0180, num_of_vassals_no_substates, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0181, brigades_compare_this, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0182, brigades_compare_from, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0183, constructing_cb_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0184, constructing_cb_from, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0185, constructing_cb_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0186, constructing_cb_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0187, constructing_cb_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0188, constructing_cb_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0189, constructing_cb_discovered, 0) \
TRIGGER_BYTECODE_ELEMENT(0x018A, constructing_cb_progress, 2) \
TRIGGER_BYTECODE_ELEMENT(0x018B, civilization_progress, 2) \
TRIGGER_BYTECODE_ELEMENT(0x018C, constructing_cb_type, 1) \
TRIGGER_BYTECODE_ELEMENT(0x018D, is_our_vassal_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x018E, is_our_vassal_from, 0) \
TRIGGER_BYTECODE_ELEMENT(0x018F, is_our_vassal_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0190, is_our_vassal_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0191, is_our_vassal_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0192, is_our_vassal_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0193, substate_of_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0194, substate_of_from, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0195, substate_of_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0196, substate_of_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0197, substate_of_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0198, substate_of_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0199, is_substate, 0) \
TRIGGER_BYTECODE_ELEMENT(0x019A, great_wars_enabled, 0) \
TRIGGER_BYTECODE_ELEMENT(0x019B, can_nationalize, 0) \
TRIGGER_BYTECODE_ELEMENT(0x019C, part_of_sphere, 0) \
TRIGGER_BYTECODE_ELEMENT(0x019D, is_sphere_leader_of_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x019E, is_sphere_leader_of_from, 0) \
TRIGGER_BYTECODE_ELEMENT(0x019F, is_sphere_leader_of_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x01A0, is_sphere_leader_of_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x01A1, is_sphere_leader_of_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x01A2, is_sphere_leader_of_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x01A3, number_of_states, 1) \
TRIGGER_BYTECODE_ELEMENT(0x01A4, war_score, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01A5, is_releasable_vassal_from, 0) \
TRIGGER_BYTECODE_ELEMENT(0x01A6, is_releasable_vassal_other, 0) \
TRIGGER_BYTECODE_ELEMENT(0x01A7, has_recent_imigration, 1) \
TRIGGER_BYTECODE_ELEMENT(0x01A8, province_control_days, 1) \
TRIGGER_BYTECODE_ELEMENT(0x01A9, is_disarmed, 0) \
TRIGGER_BYTECODE_ELEMENT(0x01AA, big_producer, 1) \
TRIGGER_BYTECODE_ELEMENT(0x01AB, someone_can_form_union_tag_from, 0) \
TRIGGER_BYTECODE_ELEMENT(0x01AC, someone_can_form_union_tag_other, 0) \
TRIGGER_BYTECODE_ELEMENT(0x01AD, social_movement_strength, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01AE, political_movement_strength, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01AF, can_build_factory_in_capital_state, 1) \
TRIGGER_BYTECODE_ELEMENT(0x01B0, social_movement, 0) \
TRIGGER_BYTECODE_ELEMENT(0x01B1, political_movement, 0) \
TRIGGER_BYTECODE_ELEMENT(0x01B2, has_cultural_sphere, 0) \
TRIGGER_BYTECODE_ELEMENT(0x01B3, world_wars_enabled, 0) \
TRIGGER_BYTECODE_ELEMENT(0x01B4, has_pop_culture_pop_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x01B5, has_pop_culture_state_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x01B6, has_pop_culture_province_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x01B7, has_pop_culture_nation_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x01B8, has_pop_culture_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x01B9, has_pop_culture_state, 1) \
TRIGGER_BYTECODE_ELEMENT(0x01BA, has_pop_culture_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x01BB, has_pop_culture_nation, 1) \
TRIGGER_BYTECODE_ELEMENT(0x01BC, has_pop_religion_pop_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x01BD, has_pop_religion_state_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x01BE, has_pop_religion_province_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x01BF, has_pop_religion_nation_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x01C0, has_pop_religion_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x01C1, has_pop_religion_state, 1) \
TRIGGER_BYTECODE_ELEMENT(0x01C2, has_pop_religion_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x01C3, has_pop_religion_nation, 1) \
TRIGGER_BYTECODE_ELEMENT(0x01C4, life_needs, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01C5, everyday_needs, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01C6, luxury_needs, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01C7, consciousness_pop, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01C8, consciousness_province, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01C9, consciousness_state, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01CA, consciousness_nation, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01CB, literacy_pop, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01CC, literacy_province, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01CD, literacy_state, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01CE, literacy_nation, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01CF, militancy_pop, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01D0, militancy_province, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01D1, militancy_state, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01D2, militancy_nation, 2) \
/*FEB FIX*/\
TRIGGER_BYTECODE_ELEMENT(0x01D3, military_spending_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x01D4, military_spending_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x01D5, military_spending_state, 1) \
TRIGGER_BYTECODE_ELEMENT(0x01D6, military_spending_nation, 1) \
TRIGGER_BYTECODE_ELEMENT(0x01D7, administration_spending_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x01D8, administration_spending_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x01D9, administration_spending_state, 1) \
TRIGGER_BYTECODE_ELEMENT(0x01DA, administration_spending_nation, 1) \
TRIGGER_BYTECODE_ELEMENT(0x01DB, education_spending_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x01DC, education_spending_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x01DD, education_spending_state, 1) \
TRIGGER_BYTECODE_ELEMENT(0x01DE, education_spending_nation, 1) \
TRIGGER_BYTECODE_ELEMENT(0x01DF, trade_goods_in_state_state, 1) \
TRIGGER_BYTECODE_ELEMENT(0x01E0, trade_goods_in_state_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x01E1, has_flashpoint, 0) \
TRIGGER_BYTECODE_ELEMENT(0x01E2, flashpoint_tension, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01E3, crisis_exist, 0) \
TRIGGER_BYTECODE_ELEMENT(0x01E4, is_liberation_crisis, 0) \
TRIGGER_BYTECODE_ELEMENT(0x01E5, is_claim_crisis, 0) \
TRIGGER_BYTECODE_ELEMENT(0x01E6, crisis_temperature, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01E7, involved_in_crisis_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x01E8, involved_in_crisis_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x01E9, rich_strata_life_needs_nation, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01EA, rich_strata_life_needs_state, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01EB, rich_strata_life_needs_province, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01EC, rich_strata_life_needs_pop, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01ED, rich_strata_everyday_needs_nation, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01EE, rich_strata_everyday_needs_state, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01EF, rich_strata_everyday_needs_province, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01F0, rich_strata_everyday_needs_pop, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01F1, rich_strata_luxury_needs_nation, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01F2, rich_strata_luxury_needs_state, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01F3, rich_strata_luxury_needs_province, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01F4, rich_strata_luxury_needs_pop, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01F5, middle_strata_life_needs_nation, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01F6, middle_strata_life_needs_state, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01F7, middle_strata_life_needs_province, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01F8, middle_strata_life_needs_pop, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01F9, middle_strata_everyday_needs_nation, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01FA, middle_strata_everyday_needs_state, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01FB, middle_strata_everyday_needs_province, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01FC, middle_strata_everyday_needs_pop, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01FD, middle_strata_luxury_needs_nation, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01FE, middle_strata_luxury_needs_state, 2) \
TRIGGER_BYTECODE_ELEMENT(0x01FF, middle_strata_luxury_needs_province, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0200, middle_strata_luxury_needs_pop, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0201, poor_strata_life_needs_nation, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0202, poor_strata_life_needs_state, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0203, poor_strata_life_needs_province, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0204, poor_strata_life_needs_pop, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0205, poor_strata_everyday_needs_nation, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0206, poor_strata_everyday_needs_state, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0207, poor_strata_everyday_needs_province, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0208, poor_strata_everyday_needs_pop, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0209, poor_strata_luxury_needs_nation, 2) \
TRIGGER_BYTECODE_ELEMENT(0x020A, poor_strata_luxury_needs_state, 2) \
TRIGGER_BYTECODE_ELEMENT(0x020B, poor_strata_luxury_needs_province, 2) \
TRIGGER_BYTECODE_ELEMENT(0x020C, poor_strata_luxury_needs_pop, 2) \
/*complex*/\
TRIGGER_BYTECODE_ELEMENT(0x020D, diplomatic_influence_tag, 2) \
TRIGGER_BYTECODE_ELEMENT(0x020E, diplomatic_influence_this_nation, 1) \
TRIGGER_BYTECODE_ELEMENT(0x020F, diplomatic_influence_this_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0210, diplomatic_influence_from_nation, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0211, diplomatic_influence_from_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0212, pop_unemployment_nation, 3) \
TRIGGER_BYTECODE_ELEMENT(0x0213, pop_unemployment_state, 3) \
TRIGGER_BYTECODE_ELEMENT(0x0214, pop_unemployment_province, 3) \
TRIGGER_BYTECODE_ELEMENT(0x0215, pop_unemployment_pop, 3) \
TRIGGER_BYTECODE_ELEMENT(0x0216, pop_unemployment_nation_this_pop, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0217, pop_unemployment_state_this_pop, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0218, pop_unemployment_province_this_pop, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0219, relation_tag, 2) \
TRIGGER_BYTECODE_ELEMENT(0x021A, relation_this_nation, 1) \
TRIGGER_BYTECODE_ELEMENT(0x021B, relation_this_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x021C, relation_from_nation, 1) \
TRIGGER_BYTECODE_ELEMENT(0x021D, relation_from_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x021E, check_variable, 3) \
TRIGGER_BYTECODE_ELEMENT(0x021F, upper_house, 3) \
TRIGGER_BYTECODE_ELEMENT(0x0220, unemployment_by_type_nation, 3) \
TRIGGER_BYTECODE_ELEMENT(0x0221, unemployment_by_type_state, 3) \
TRIGGER_BYTECODE_ELEMENT(0x0222, unemployment_by_type_province, 3) \
TRIGGER_BYTECODE_ELEMENT(0x0223, unemployment_by_type_pop, 3) \
TRIGGER_BYTECODE_ELEMENT(0x0224, party_loyalty_nation_province_id, 3) \
TRIGGER_BYTECODE_ELEMENT(0x0225, party_loyalty_from_nation_province_id, 3) \
TRIGGER_BYTECODE_ELEMENT(0x0226, party_loyalty_province_province_id, 3) \
TRIGGER_BYTECODE_ELEMENT(0x0227, party_loyalty_from_province_province_id, 3) \
TRIGGER_BYTECODE_ELEMENT(0x0228, party_loyalty_nation_from_province, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0229, party_loyalty_from_nation_scope_province, 2) \
TRIGGER_BYTECODE_ELEMENT(0x022A, can_build_in_province_railroad_no_limit_from_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x022B, can_build_in_province_railroad_yes_limit_from_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x022C, can_build_in_province_railroad_no_limit_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x022D, can_build_in_province_railroad_yes_limit_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x022E, can_build_in_province_fort_no_limit_from_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x022F, can_build_in_province_fort_yes_limit_from_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0230, can_build_in_province_fort_no_limit_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0231, can_build_in_province_fort_yes_limit_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0232, can_build_in_province_naval_base_no_limit_from_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0233, can_build_in_province_naval_base_yes_limit_from_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0234, can_build_in_province_naval_base_no_limit_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0235, can_build_in_province_naval_base_yes_limit_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0236, can_build_railway_in_capital_yes_whole_state_yes_limit, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0237, can_build_railway_in_capital_yes_whole_state_no_limit, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0238, can_build_railway_in_capital_no_whole_state_yes_limit, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0239, can_build_railway_in_capital_no_whole_state_no_limit, 0) \
TRIGGER_BYTECODE_ELEMENT(0x023A, can_build_fort_in_capital_yes_whole_state_yes_limit, 0) \
TRIGGER_BYTECODE_ELEMENT(0x023B, can_build_fort_in_capital_yes_whole_state_no_limit, 0) \
TRIGGER_BYTECODE_ELEMENT(0x023C, can_build_fort_in_capital_no_whole_state_yes_limit, 0) \
TRIGGER_BYTECODE_ELEMENT(0x023D, can_build_fort_in_capital_no_whole_state_no_limit, 0) \
TRIGGER_BYTECODE_ELEMENT(0x023E, work_available_nation, 1) \
TRIGGER_BYTECODE_ELEMENT(0x023F, work_available_state, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0240, work_available_province, 1) \
/*variable name*/\
TRIGGER_BYTECODE_ELEMENT(0x0241, unused_1, 0) /* UNUSED 1 */ \
TRIGGER_BYTECODE_ELEMENT(0x0242, variable_ideology_name_nation, 3) \
TRIGGER_BYTECODE_ELEMENT(0x0243, variable_ideology_name_state, 3) \
TRIGGER_BYTECODE_ELEMENT(0x0244, variable_ideology_name_province, 3) \
TRIGGER_BYTECODE_ELEMENT(0x0245, variable_ideology_name_pop, 3) \
TRIGGER_BYTECODE_ELEMENT(0x0246, variable_issue_name_nation, 3) \
TRIGGER_BYTECODE_ELEMENT(0x0247, variable_issue_name_state, 3) \
TRIGGER_BYTECODE_ELEMENT(0x0248, variable_issue_name_province, 3) \
TRIGGER_BYTECODE_ELEMENT(0x0249, variable_issue_name_pop, 3) \
TRIGGER_BYTECODE_ELEMENT(0x024A, variable_issue_group_name_nation, 2) \
TRIGGER_BYTECODE_ELEMENT(0x024B, variable_issue_group_name_state, 2) \
TRIGGER_BYTECODE_ELEMENT(0x024C, variable_issue_group_name_province, 2) \
TRIGGER_BYTECODE_ELEMENT(0x024D, variable_issue_group_name_pop, 2) \
TRIGGER_BYTECODE_ELEMENT(0x024E, variable_pop_type_name_nation, 3) \
TRIGGER_BYTECODE_ELEMENT(0x024F, variable_pop_type_name_state, 3) \
TRIGGER_BYTECODE_ELEMENT(0x0250, variable_pop_type_name_province, 3) \
TRIGGER_BYTECODE_ELEMENT(0x0251, variable_pop_type_name_pop, 3) \
TRIGGER_BYTECODE_ELEMENT(0x0252, variable_good_name, 3) \
/*misplaced*/ \
TRIGGER_BYTECODE_ELEMENT(0x0253, strata_middle, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0254, strata_poor, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0255, party_loyalty_from_province_scope_province, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0256, can_build_factory_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0257, can_build_factory_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0258, nationalvalue_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0259, nationalvalue_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x025A, war_exhaustion_pop, 2) \
TRIGGER_BYTECODE_ELEMENT(0x025B, has_culture_core_province_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x025C, tag_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x025D, has_country_flag_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x025E, has_country_flag_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x025F, has_country_modifier_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0260, religion_nation, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0261, religion_nation_reb, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0262, religion_nation_from_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0263, religion_nation_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0264, religion_nation_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0265, religion_nation_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0266, religion_nation_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0267, war_exhaustion_province, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0268, is_greater_power_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0269, is_cultural_union_pop_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x026A, has_building_factory, 0) \
TRIGGER_BYTECODE_ELEMENT(0x026B, has_building_state_from_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x026C, has_building_factory_from_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x026D, party_loyalty_generic, 2) \
TRIGGER_BYTECODE_ELEMENT(0x026E, invention, 1) \
TRIGGER_BYTECODE_ELEMENT(0x026F, political_movement_from_reb, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0270, social_movement_from_reb, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0271, is_next_rreform_nation, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0272, is_next_rreform_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0273, variable_reform_group_name_nation, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0274, variable_reform_group_name_state, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0275, variable_reform_group_name_province, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0276, variable_reform_group_name_pop, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0277, is_disarmed_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0278, owned_by_state_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0279, owned_by_state_from_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x027A, owned_by_state_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x027B, owned_by_state_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x027C, owned_by_state_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x027D, owned_by_state_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x027E, units_in_province_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x027F, primary_culture_from_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0280, primary_culture_from_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0281, neighbour_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0282, neighbour_from_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0283, technology_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0284, invention_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0285, brigades_compare_province_this, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0286, brigades_compare_province_from, 2) \
TRIGGER_BYTECODE_ELEMENT(0x0287, is_accepted_culture_nation_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0288, is_accepted_culture_nation_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0289, is_accepted_culture_nation_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x028A, is_accepted_culture_nation_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x028B, is_accepted_culture_state_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x028C, is_accepted_culture_state_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x028D, is_accepted_culture_state_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x028E, is_accepted_culture_state_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x028F, is_accepted_culture_province_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0290, is_accepted_culture_province_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0291, is_accepted_culture_province_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0292, is_accepted_culture_province_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0293, is_accepted_culture_pop_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0294, is_accepted_culture_pop_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0295, is_accepted_culture_pop_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0296, is_accepted_culture_pop_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x0297, culture_group_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0298, culture_group_state, 1) \
TRIGGER_BYTECODE_ELEMENT(0x0299, have_core_in_nation_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x029A, have_core_in_nation_this, 0) \
TRIGGER_BYTECODE_ELEMENT(0x029B, have_core_in_nation_from, 0) \
TRIGGER_BYTECODE_ELEMENT(0x029C, owns_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x029D, empty_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x029E, is_overseas_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x029F, primary_culture_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x02A0, plurality_pop, 2) \
TRIGGER_BYTECODE_ELEMENT(0x02A1, is_overseas_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02A2, stronger_army_than_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x02A3, region_state, 1) \
TRIGGER_BYTECODE_ELEMENT(0x02A4, region_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x02A5, owns_region, 1) \
TRIGGER_BYTECODE_ELEMENT(0x02A6, is_core_state_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x02A7, country_units_in_state_from, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02A8, country_units_in_state_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02A9, country_units_in_state_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02AA, country_units_in_state_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02AB, country_units_in_state_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02AC, country_units_in_state_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x02AD, stronger_army_than_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02AE, stronger_army_than_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02AF, stronger_army_than_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02B0, stronger_army_than_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02B1, stronger_army_than_from_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02B2, stronger_army_than_from_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02B3, flashpoint_tension_province, 2) \
TRIGGER_BYTECODE_ELEMENT(0x02B4, is_colonial_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02B5, has_country_flag_state, 1) \
TRIGGER_BYTECODE_ELEMENT(0x02B6, rich_tax_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x02B7, middle_tax_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x02B8, poor_tax_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x02B9, is_core_pop_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x02BA, is_core_boolean, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02BB, is_core_state_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02BC, is_core_state_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02BD, is_core_state_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02BE, is_core_state_from_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02BF, ruling_party_ideology_province, 1) \
TRIGGER_BYTECODE_ELEMENT(0x02C0, money_province, 2) \
TRIGGER_BYTECODE_ELEMENT(0x02C1, is_our_vassal_province_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x02C2, is_our_vassal_province_from, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02C3, is_our_vassal_province_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02C4, is_our_vassal_province_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02C5, is_our_vassal_province_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02C6, is_our_vassal_province_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02C7, vassal_of_province_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x02C8, vassal_of_province_from, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02C9, vassal_of_province_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02CA, vassal_of_province_this_province, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02CB, vassal_of_province_this_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02CC, vassal_of_province_this_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02CD, relation_this_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x02CE, has_recently_lost_war_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02CF, technology_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x02D0, invention_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x02D1, in_default_bool, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02D2, is_state_capital_pop, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02D3, region_proper, 1) \
TRIGGER_BYTECODE_ELEMENT(0x02D4, region_proper_state, 1) \
TRIGGER_BYTECODE_ELEMENT(0x02D5, region_proper_pop, 1) \
TRIGGER_BYTECODE_ELEMENT(0x02D6, owns_region_proper, 1) \
TRIGGER_BYTECODE_ELEMENT(0x02D7, pop_majority_religion_nation_this_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02D8, military_score_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x02D9, industrial_score_tag, 1) \
TRIGGER_BYTECODE_ELEMENT(0x02DA, has_factories_nation, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02DB, is_coastal_state, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02DC, has_building_bank, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02DD, has_building_university, 0) \
TRIGGER_BYTECODE_ELEMENT(0x02DE, test, 1) \

#define TRIGGER_BYTECODE_ELEMENT(code, name, arg) constexpr inline uint16_t name = code;
TRIGGER_BYTECODE_LIST
#undef TRIGGER_BYTECODE_ELEMENT

constexpr inline uint16_t first_scope_code = 0x02DF;

// technology name -- payload 1
// ideology name -- 4 variants payload 2
// issue name -- 4 variants payload 2
// issue group name -- 4 variants payload 2
// pop type -- 4 variants payload 2
// good name -- payload 2

// scopes
constexpr inline uint16_t generic_scope = first_scope_code + 0x0000; // or & and
constexpr inline uint16_t x_neighbor_province_scope = first_scope_code + 0x0001;
constexpr inline uint16_t x_neighbor_country_scope_nation = first_scope_code + 0x0002;
constexpr inline uint16_t x_neighbor_country_scope_pop = first_scope_code + 0x0003;
constexpr inline uint16_t x_war_countries_scope_nation = first_scope_code + 0x0004;
constexpr inline uint16_t x_war_countries_scope_pop = first_scope_code + 0x0005;
constexpr inline uint16_t x_greater_power_scope = first_scope_code + 0x0006;
constexpr inline uint16_t x_owned_province_scope_state = first_scope_code + 0x0007;
constexpr inline uint16_t x_owned_province_scope_nation = first_scope_code + 0x0008;
constexpr inline uint16_t x_core_scope_province = first_scope_code + 0x0009;
constexpr inline uint16_t x_core_scope_nation = first_scope_code + 0x000A;
constexpr inline uint16_t x_state_scope = first_scope_code + 0x000B;
constexpr inline uint16_t x_substate_scope = first_scope_code + 0x000C;
constexpr inline uint16_t x_sphere_member_scope = first_scope_code + 0x000D;
constexpr inline uint16_t x_pop_scope_province = first_scope_code + 0x000E;
constexpr inline uint16_t x_pop_scope_state = first_scope_code + 0x000F;
constexpr inline uint16_t x_pop_scope_nation = first_scope_code + 0x0010;
constexpr inline uint16_t x_provinces_in_variable_region = first_scope_code + 0x0011; // variable name
constexpr inline uint16_t owner_scope_state = first_scope_code + 0x0012;
constexpr inline uint16_t owner_scope_province = first_scope_code + 0x0013;
constexpr inline uint16_t controller_scope = first_scope_code + 0x0014;
constexpr inline uint16_t location_scope = first_scope_code + 0x0015;
constexpr inline uint16_t country_scope_state = first_scope_code + 0x0016;
constexpr inline uint16_t country_scope_pop = first_scope_code + 0x0017;
constexpr inline uint16_t capital_scope = first_scope_code + 0x0018;
constexpr inline uint16_t this_scope_pop = first_scope_code + 0x0019;
constexpr inline uint16_t this_scope_nation = first_scope_code + 0x001A;
constexpr inline uint16_t this_scope_state = first_scope_code + 0x001B;
constexpr inline uint16_t this_scope_province = first_scope_code + 0x001C;
constexpr inline uint16_t from_scope_pop = first_scope_code + 0x001D;
constexpr inline uint16_t from_scope_nation = first_scope_code + 0x001E;
constexpr inline uint16_t from_scope_state = first_scope_code + 0x001F;
constexpr inline uint16_t from_scope_province = first_scope_code + 0x0020;
constexpr inline uint16_t sea_zone_scope = first_scope_code + 0x0021;
constexpr inline uint16_t cultural_union_scope = first_scope_code + 0x0022;
constexpr inline uint16_t overlord_scope = first_scope_code + 0x0023;
constexpr inline uint16_t sphere_owner_scope = first_scope_code + 0x0024;
constexpr inline uint16_t independence_scope = first_scope_code + 0x0025;
constexpr inline uint16_t flashpoint_tag_scope = first_scope_code + 0x0026;
constexpr inline uint16_t crisis_state_scope = first_scope_code + 0x0027;
constexpr inline uint16_t state_scope_pop = first_scope_code + 0x0028;
constexpr inline uint16_t state_scope_province = first_scope_code + 0x0029;
constexpr inline uint16_t tag_scope = first_scope_code + 0x002A;		 // variable name
constexpr inline uint16_t integer_scope = first_scope_code + 0x002B; // variable name
constexpr inline uint16_t country_scope_nation = first_scope_code + 0x002C;
constexpr inline uint16_t country_scope_province = first_scope_code + 0x002D;
constexpr inline uint16_t cultural_union_scope_pop = first_scope_code + 0x002E;
constexpr inline uint16_t capital_scope_province = first_scope_code + 0x002F;
constexpr inline uint16_t capital_scope_pop = first_scope_code + 0x0030;
constexpr inline uint16_t x_country_scope = first_scope_code + 0x0031;
constexpr inline uint16_t x_neighbor_province_scope_state = first_scope_code + 0x0032;
constexpr inline uint16_t x_provinces_in_variable_region_proper = first_scope_code + 0x0033;

constexpr inline uint16_t first_invalid_code = first_scope_code + 0x0034;

constexpr inline uint16_t placeholder_not_scope = code_mask;

// variable
//  region name = 1 variant, x type, payload 1
//  tag = 1 variant, payload 1
//  integer = 1 variant, payload 1

inline constexpr int8_t data_sizes[] = {
	0, //none
#define TRIGGER_BYTECODE_ELEMENT(code, name, arg) arg,
	TRIGGER_BYTECODE_LIST
#undef TRIGGER_BYTECODE_ELEMENT
};
static_assert(sizeof(data_sizes) == first_scope_code);

enum class slot_contents { empty = 0, province = 1, state = 2, pop = 3, nation = 4, rebel = 5 };

union payload {

	uint16_t value;
	int16_t signed_value;
	bool boolean_value;
	dcon::government_type_id gov_id;
	dcon::religion_id rel_id;
	dcon::commodity_id com_id;
	dcon::rebel_type_id reb_id;
	dcon::factory_type_id fac_id;
	dcon::ideology_id ideo_id;
	dcon::issue_id iss_id;
	dcon::issue_option_id opt_id;
	dcon::cb_type_id cb_id;
	dcon::pop_type_id popt_id;
	dcon::leader_trait_id lead_id;
	dcon::unit_type_id unit_id;
	dcon::modifier_id mod_id;
	dcon::culture_id cul_id;
	dcon::culture_group_id culgrp_id;
	dcon::national_identity_id tag_id;
	dcon::state_definition_id state_id;
	dcon::province_id prov_id;
	dcon::technology_id tech_id;
	dcon::invention_id invt_id;
	dcon::national_variable_id natv_id;
	dcon::national_flag_id natf_id;
	dcon::global_flag_id glob_id;
	dcon::national_event_id nev_id;
	dcon::provincial_event_id pev_id;
	dcon::trigger_key tr_id;
	dcon::crime_id crm_id;
	dcon::political_party_id par_id;
	dcon::reform_id ref_id;
	dcon::reform_option_id ropt_id;
	dcon::region_id reg_id;
	dcon::stored_trigger_id str_id;

	// variables::national_variable_tag nat_var;
	// variables::national_flag_tag nat_flag;
	// variables::global_variable_tag global_var;
	// events::event_tag event;
	// trigger_tag trigger;

	payload(payload const& i) noexcept : value(i.value) { }
	payload(uint16_t i) : value(i) { }
	payload(int16_t i) : signed_value(i) { }
	payload(bool i) {
		memset(this, 0, sizeof(payload));
		boolean_value = i;
	}
	payload(dcon::government_type_id i) {
		memset(this, 0, sizeof(payload));
		gov_id = i;
	}
	payload(dcon::region_id i) {
		memset(this, 0, sizeof(payload));
		reg_id = i;
	}
	payload(dcon::religion_id i) {
		memset(this, 0, sizeof(payload));
		rel_id = i;
	}
	payload(dcon::commodity_id i) {
		memset(this, 0, sizeof(payload));
		com_id = i;
	}
	payload(dcon::rebel_type_id i) {
		memset(this, 0, sizeof(payload));
		reb_id = i;
	}
	payload(dcon::factory_type_id i) {
		memset(this, 0, sizeof(payload));
		fac_id = i;
	}
	payload(dcon::ideology_id i) {
		memset(this, 0, sizeof(payload));
		ideo_id = i;
	}
	payload(dcon::issue_id i) {
		memset(this, 0, sizeof(payload));
		iss_id = i;
	}
	payload(dcon::issue_option_id i) {
		memset(this, 0, sizeof(payload));
		opt_id = i;
	}
	payload(dcon::cb_type_id i) {
		memset(this, 0, sizeof(payload));
		cb_id = i;
	}
	payload(dcon::pop_type_id i) {
		memset(this, 0, sizeof(payload));
		popt_id = i;
	}
	payload(dcon::leader_trait_id i) {
		memset(this, 0, sizeof(payload));
		lead_id = i;
	}
	payload(dcon::unit_type_id i) {
		memset(this, 0, sizeof(payload));
		unit_id = i;
	}
	payload(dcon::modifier_id i) {
		memset(this, 0, sizeof(payload));
		mod_id = i;
	}
	payload(dcon::culture_id i) {
		memset(this, 0, sizeof(payload));
		cul_id = i;
	}
	payload(dcon::culture_group_id i) {
		memset(this, 0, sizeof(payload));
		culgrp_id = i;
	}
	payload(dcon::national_identity_id i) {
		memset(this, 0, sizeof(payload));
		tag_id = i;
	}
	payload(dcon::state_definition_id i) {
		memset(this, 0, sizeof(payload));
		state_id = i;
	}
	payload(dcon::province_id i) {
		memset(this, 0, sizeof(payload));
		prov_id = i;
	}
	payload(dcon::technology_id i) {
		memset(this, 0, sizeof(payload));
		tech_id = i;
	}
	payload(dcon::invention_id i) {
		memset(this, 0, sizeof(payload));
		invt_id = i;
	}
	payload(dcon::national_variable_id i) {
		memset(this, 0, sizeof(payload));
		natv_id = i;
	}
	payload(dcon::national_flag_id i) {
		memset(this, 0, sizeof(payload));
		natf_id = i;
	}
	payload(dcon::global_flag_id i) {
		memset(this, 0, sizeof(payload));
		glob_id = i;
	}
	payload(dcon::national_event_id i) {
		memset(this, 0, sizeof(payload));
		nev_id = i;
	}
	payload(dcon::provincial_event_id i) {
		memset(this, 0, sizeof(payload));
		pev_id = i;
	}
	payload(dcon::trigger_key i) {
		memset(this, 0, sizeof(payload));
		tr_id = i;
	}
	payload(dcon::crime_id i) {
		memset(this, 0, sizeof(payload));
		crm_id = i;
	}
	payload(dcon::political_party_id i) {
		memset(this, 0, sizeof(payload));
		par_id = i;
	}
	payload(dcon::reform_id i) {
		memset(this, 0, sizeof(payload));
		ref_id = i;
	}
	payload(dcon::reform_option_id i) {
		memset(this, 0, sizeof(payload));
		ropt_id = i;
	}
	payload(dcon::stored_trigger_id i) {
		memset(this, 0, sizeof(payload));
		str_id = i;
	}
};

static_assert(sizeof(payload) == 2);

inline int32_t get_trigger_non_scope_payload_size(uint16_t const* data) {
	assert((data[0] & trigger::code_mask) < trigger::first_scope_code);
	return trigger::data_sizes[data[0] & trigger::code_mask];
}
inline int32_t get_trigger_scope_payload_size(uint16_t const* data) {
	return data[1];
}
inline int32_t get_trigger_payload_size(uint16_t const* data) {
	if((data[0] & trigger::code_mask) >= trigger::first_scope_code)
		return get_trigger_scope_payload_size(data);
	else
		return get_trigger_non_scope_payload_size(data);
}
inline int32_t trigger_scope_data_payload(uint16_t code) {
	auto const masked_code = code & trigger::code_mask;
	if((masked_code == trigger::x_provinces_in_variable_region) || (masked_code == trigger::x_provinces_in_variable_region_proper) || (masked_code == trigger::tag_scope) ||
			(masked_code == trigger::integer_scope))
		return 1;
	return 0;
}

template<typename T>
uint16_t* recurse_over_triggers(uint16_t* source, T const& f) {
	f(source);
	assert((source[0] & trigger::code_mask) < trigger::first_invalid_code || (source[0] & trigger::code_mask) == trigger::code_mask);

	if((source[0] & trigger::code_mask) >= trigger::first_scope_code) {
		auto const source_size = 1 + get_trigger_scope_payload_size(source);

		auto sub_units_start = source + 2 + trigger_scope_data_payload(source[0]);
		while(sub_units_start < source + source_size) {
			sub_units_start = recurse_over_triggers(sub_units_start, f);
		}
		return source + source_size;
	} else {
		return source + 1 + get_trigger_non_scope_payload_size(source);
	}
}

inline uint32_t count_subtriggers(uint16_t const* source) {
	uint32_t count = 0;
	if((source[0] & trigger::code_mask) >= trigger::first_scope_code) {
		auto const source_size = 1 + get_trigger_scope_payload_size(source);
		auto sub_units_start = source + 2 + trigger_scope_data_payload(source[0]);
		while(sub_units_start < source + source_size) {
			++count;
			sub_units_start += 1 + get_trigger_payload_size(sub_units_start);
		}
	}
	return count;
}

} // namespace trigger
