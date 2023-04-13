#include <string_view>
#include "dcon_generated.hpp"
#include "system_state.hpp"
#include "text.hpp"
#include "script_constants.hpp"

namespace ui {

namespace effect_tooltip {
#define EFFECT_DISPLAY_PARAMS \
    sys::state& state, text::columnar_layout& layout, uint16_t const* eval, int32_t primary_slot, int32_t this_slot, int32_t from_slot, int32_t indentation

enum effect_tp_flags {
    // Payload type
    fp_two_places = 0x00,
    fp_one_place,
    integer,
    modifier,
    modifier_no_duration,
    fp_to_integer,
    payload_type_mask = 0x0F,
    // Display flags
    negative = 0x10,
    display_mask = 0xF0,
};

#define EFFECT_STATMENTS /* non scopes */ \
    EFFECT_STATMENT(capital, 0x0001, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(add_core_tag, 0x0002, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(add_core_int, 0x0003, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(add_core_this_nation, 0x0004, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(add_core_this_province, 0x0005, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(add_core_this_state, 0x0006, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(add_core_this_pop, 0x0007, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(add_core_from_province, 0x0008, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(add_core_from_nation, 0x0009, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(add_core_reb, 0x000A, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(remove_core_tag, 0x000B, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(remove_core_int, 0x000C, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(remove_core_this_nation, 0x000D, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(remove_core_this_province, 0x000E, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(remove_core_this_state, 0x000F, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(remove_core_this_pop, 0x0010, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(remove_core_from_province, 0x0011, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(remove_core_from_nation, 0x0012, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(remove_core_reb, 0x0013, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(change_region_name_state, 0x0014, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(change_region_name_province, 0x0015, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(trade_goods, 0x0016, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(add_accepted_culture, 0x0017, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(add_accepted_culture_union, 0x0018, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(primary_culture, 0x0019, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(primary_culture_this_nation, 0x001A, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(primary_culture_this_state, 0x001B, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(primary_culture_this_province, 0x001C, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(primary_culture_this_pop, 0x001D, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(primary_culture_from_nation, 0x001E, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(remove_accepted_culture, 0x001F, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(life_rating, 0x0020, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(religion, 0x0021, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(is_slave_state_yes, 0x0022, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(is_slave_pop_yes, 0x0023, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(research_points, 0x0024, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(tech_school, 0x0025, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(government, 0x0026, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(government_reb, 0x0027, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(treasury, 0x0028, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(war_exhaustion, 0x0029, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(prestige, 0x002A, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(change_tag, 0x002B, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(change_tag_culture, 0x002C, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(change_tag_no_core_switch, 0x002D, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(change_tag_no_core_switch_culture, 0x002E, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(set_country_flag, 0x002F, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(clr_country_flag, 0x0030, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(military_access, 0x0031, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(military_access_this_nation, 0x0032, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(military_access_this_province, 0x0033, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(military_access_from_nation, 0x0034, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(military_access_from_province, 0x0035, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(badboy, 0x0036, effect_tp_flags::fp_to_integer | effect_tp_flags::negative) \
    EFFECT_STATMENT(secede_province, 0x0037, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(secede_province_this_nation, 0x0038, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(secede_province_this_state, 0x0039, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(secede_province_this_province, 0x003A, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(secede_province_this_pop, 0x003B, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(secede_province_from_nation, 0x003C, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(secede_province_from_province, 0x003D, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(secede_province_reb, 0x003E, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(inherit, 0x003F, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(inherit_this_nation, 0x0040, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(inherit_this_state, 0x0041, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(inherit_this_province, 0x0042, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(inherit_this_pop, 0x0043, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(inherit_from_nation, 0x0044, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(inherit_from_province, 0x0045, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(annex_to, 0x0046, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(annex_to_this_nation, 0x0047, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(annex_to_this_state, 0x0048, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(annex_to_this_province, 0x0049, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(annex_to_this_pop, 0x004A, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(annex_to_from_nation, 0x004B, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(annex_to_from_province, 0x004C, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(release, 0x004D, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(release_this_nation, 0x004E, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(release_this_state, 0x004F, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(release_this_province, 0x0050, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(release_this_pop, 0x0051, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(release_from_nation, 0x0052, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(release_from_province, 0x0053, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(change_controller, 0x0054, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(change_controller_this_nation, 0x0055, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(change_controller_this_province, 0x0056, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(change_controller_from_nation, 0x0057, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(change_controller_from_province, 0x0058, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(infrastructure, 0x0059, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(money, 0x005A, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(leadership, 0x005B, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(create_vassal, 0x005C, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(create_vassal_this_nation, 0x005D, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(create_vassal_this_province, 0x005E, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(create_vassal_from_nation, 0x005F, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(create_vassal_from_province, 0x0060, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(end_military_access, 0x0061, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(end_military_access_this_nation, 0x0062, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(end_military_access_this_province, 0x0063, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(end_military_access_from_nation, 0x0064, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(end_military_access_from_province, 0x0065, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(leave_alliance, 0x0066, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(leave_alliance_this_nation, 0x0067, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(leave_alliance_this_province, 0x0068, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(leave_alliance_from_nation, 0x0069, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(leave_alliance_from_province, 0x006A, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(end_war, 0x006B, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(end_war_this_nation, 0x006C, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(end_war_this_province, 0x006D, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(end_war_from_nation, 0x006E, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(end_war_from_province, 0x006F, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(enable_ideology, 0x0070, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(ruling_party_ideology, 0x0071, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(plurality, 0x0072, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(remove_province_modifier, 0x0073, effect_tp_flags::modifier) \
    EFFECT_STATMENT(remove_country_modifier, 0x0074, effect_tp_flags::modifier) \
    EFFECT_STATMENT(create_alliance, 0x0075, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(create_alliance_this_nation, 0x0076, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(create_alliance_this_province, 0x0077, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(create_alliance_from_nation, 0x0078, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(create_alliance_from_province, 0x0079, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(release_vassal, 0x007A, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(release_vassal_this_nation, 0x007B, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(release_vassal_this_province, 0x007C, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(release_vassal_from_nation, 0x007D, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(release_vassal_from_province, 0x007E, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(release_vassal_reb, 0x007F, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(release_vassal_random, 0x0080, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(change_province_name, 0x0081, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(enable_canal, 0x0082, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(set_global_flag, 0x0083, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(clr_global_flag, 0x0084, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(nationalvalue_province, 0x0085, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(nationalvalue_nation, 0x0086, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(civilized_yes, 0x0087, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(civilized_no, 0x0088, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(is_slave_state_no, 0x0089, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(is_slave_pop_no, 0x008A, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(election, 0x008B, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(social_reform, 0x008C, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(political_reform, 0x008D, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(add_tax_relative_income, 0x008E, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(neutrality, 0x008F, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(reduce_pop, 0x0090, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(move_pop, 0x0091, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(pop_type, 0x0092, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(years_of_research, 0x0093, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(prestige_factor_positive, 0x0094, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(prestige_factor_negative, 0x0095, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(military_reform, 0x0096, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(economic_reform, 0x0097, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(remove_random_military_reforms, 0x0098, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(remove_random_economic_reforms, 0x0099, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(add_crime, 0x009A, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(add_crime_none, 0x009B, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(nationalize, 0x009C, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(build_factory_in_capital_state, 0x009D, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(activate_technology, 0x009E, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(great_wars_enabled_yes, 0x009F, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(great_wars_enabled_no, 0x00A0, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(world_wars_enabled_yes, 0x00A1, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(world_wars_enabled_no, 0x00A2, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(assimilate_province, 0x00A3, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(assimilate_pop, 0x00A4, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(literacy, 0x00A5, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(add_crisis_interest, 0x00A6, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(flashpoint_tension, 0x00A7, effect_tp_flags::fp_two_places | effect_tp_flags::negative) \
    EFFECT_STATMENT(add_crisis_temperature, 0x00A8, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(consciousness, 0x00A9, effect_tp_flags::fp_two_places | effect_tp_flags::negative) \
    EFFECT_STATMENT(militancy, 0x00AA, effect_tp_flags::fp_two_places | effect_tp_flags::negative) \
    EFFECT_STATMENT(rgo_size, 0x00AB, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(fort, 0x00AC, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(naval_base, 0x00AD, effect_tp_flags::fp_two_places) \
/* discard only set_province_flag clr_province_flag complex */ \
    EFFECT_STATMENT(trigger_revolt_nation, 0x00AE, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(trigger_revolt_state, 0x00AF, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(trigger_revolt_province, 0x00B0, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(diplomatic_influence, 0x00B1, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(diplomatic_influence_this_nation, 0x00B2, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(diplomatic_influence_this_province, 0x00B3, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(diplomatic_influence_from_nation, 0x00B4, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(diplomatic_influence_from_province, 0x00B5, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(relation, 0x00B6, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(relation_this_nation, 0x00B7, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(relation_this_province, 0x00B8, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(relation_from_nation, 0x00B9, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(relation_from_province, 0x00BA, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(add_province_modifier, 0x00BB, effect_tp_flags::modifier) \
    EFFECT_STATMENT(add_province_modifier_no_duration, 0x00BC, effect_tp_flags::modifier_no_duration) \
    EFFECT_STATMENT(add_country_modifier, 0x00BD, effect_tp_flags::modifier) \
    EFFECT_STATMENT(add_country_modifier_no_duration, 0x00BE, effect_tp_flags::modifier_no_duration) \
    EFFECT_STATMENT(casus_belli_tag, 0x00BF, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(casus_belli_int, 0x00C0, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(casus_belli_this_nation, 0x00C1, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(casus_belli_this_state, 0x00C2, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(casus_belli_this_province, 0x00C3, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(casus_belli_this_pop, 0x00C4, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(casus_belli_from_nation, 0x00C5, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(casus_belli_from_province, 0x00C6, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(add_casus_belli_tag, 0x00C7, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(add_casus_belli_int, 0x00C8, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(add_casus_belli_this_nation, 0x00C9, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(add_casus_belli_this_state, 0x00CA, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(add_casus_belli_this_province, 0x00CB, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(add_casus_belli_this_pop, 0x00CC, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(add_casus_belli_from_nation, 0x00CD, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(add_casus_belli_from_province, 0x00CE, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(remove_casus_belli_tag, 0x00CF, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(remove_casus_belli_int, 0x00D0, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(remove_casus_belli_this_nation, 0x00D1, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(remove_casus_belli_this_state, 0x00D2, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(remove_casus_belli_this_province, 0x00D3, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(remove_casus_belli_this_pop, 0x00D4, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(remove_casus_belli_from_nation, 0x00D5, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(remove_casus_belli_from_province, 0x00D6, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(this_remove_casus_belli_tag, 0x00D7, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(this_remove_casus_belli_int, 0x00D8, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(this_remove_casus_belli_this_nation, 0x00D9, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(this_remove_casus_belli_this_state, 0x00DA, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(this_remove_casus_belli_this_province, 0x00DB, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(this_remove_casus_belli_this_pop, 0x00DC, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(this_remove_casus_belli_from_nation, 0x00DD, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(this_remove_casus_belli_from_province, 0x00DE, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(war_tag, 0x00DF, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(war_this_nation, 0x00E0, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(war_this_state, 0x00E1, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(war_this_province, 0x00E2, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(war_this_pop, 0x00E3, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(war_from_nation, 0x00E4, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(war_from_province, 0x00E5, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(war_no_ally_tag, 0x00E6, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(war_no_ally_this_nation, 0x00E7, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(war_no_ally_this_state, 0x00E8, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(war_no_ally_this_province, 0x00E9, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(war_no_ally_this_pop, 0x00EA, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(war_no_ally_from_nation, 0x00EB, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(war_no_ally_from_province, 0x00EC, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(country_event_this_nation, 0x00ED, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(country_event_immediate_this_nation, 0x00EE, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(province_event_this_nation, 0x00EF, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(province_event_immediate_this_nation, 0x00F0, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(sub_unit_int, 0x00F1, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(sub_unit_this, 0x00F2, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(sub_unit_from, 0x00F3, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(sub_unit_current, 0x00F4, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(set_variable, 0x00F5, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(change_variable, 0x00F6, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(ideology, 0x00F7, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(upper_house, 0x00F8, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(scaled_militancy_issue, 0x00F9, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(scaled_militancy_ideology, 0x00FA, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(scaled_militancy_unemployment, 0x00FB, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(scaled_consciousness_issue, 0x00FC, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(scaled_consciousness_ideology, 0x00FD, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(scaled_consciousness_unemployment, 0x00FE, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(define_general, 0x00FF, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(define_admiral, 0x0100, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(dominant_issue, 0x0101, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(add_war_goal, 0x0102, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(move_issue_percentage_nation, 0x0103, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(move_issue_percentage_state, 0x0104, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(move_issue_percentage_province, 0x0105, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(move_issue_percentage_pop, 0x0106, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(party_loyalty, 0x0107, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(party_loyalty_province, 0x0108, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(variable_tech_name_no, 0x0109, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(variable_invention_name_yes, 0x010A, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(build_railway_in_capital_yes_whole_state_yes_limit, 0x010B, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(build_railway_in_capital_yes_whole_state_no_limit, 0x010C, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(build_railway_in_capital_no_whole_state_yes_limit, 0x010D, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(build_railway_in_capital_no_whole_state_no_limit, 0x010E, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(build_fort_in_capital_yes_whole_state_yes_limit, 0x010F, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(build_fort_in_capital_yes_whole_state_no_limit, 0x0110, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(build_fort_in_capital_no_whole_state_yes_limit, 0x0111, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(build_fort_in_capital_no_whole_state_no_limit, 0x0112, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(relation_reb, 0x0113, effect_tp_flags::fp_two_places) \
/* variable name */ \
    EFFECT_STATMENT(variable_tech_name_yes, 0x0114, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(variable_good_name, 0x0115, effect_tp_flags::fp_two_places) \
/* misplaced */ \
    EFFECT_STATMENT(set_country_flag_province, 0x0116, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(add_country_modifier_province, 0x0117, effect_tp_flags::modifier) \
    EFFECT_STATMENT(add_country_modifier_province_no_duration, 0x0118, effect_tp_flags::modifier_no_duration) \
    EFFECT_STATMENT(dominant_issue_nation, 0x0119, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(relation_province, 0x011A, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(relation_province_this_nation, 0x011B, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(relation_province_this_province, 0x011C, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(relation_province_from_nation, 0x011D, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(relation_province_from_province, 0x011E, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(relation_province_reb, 0x011F, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(scaled_militancy_nation_issue, 0x0120, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(scaled_militancy_nation_ideology, 0x0121, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(scaled_militancy_nation_unemployment, 0x0122, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(scaled_consciousness_nation_issue, 0x0123, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(scaled_consciousness_nation_ideology, 0x0124, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(scaled_consciousness_nation_unemployment, 0x0125, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(scaled_militancy_state_issue, 0x0126, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(scaled_militancy_state_ideology, 0x0127, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(scaled_militancy_state_unemployment, 0x0128, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(scaled_consciousness_state_issue, 0x0129, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(scaled_consciousness_state_ideology, 0x012A, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(scaled_consciousness_state_unemployment, 0x012B, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(scaled_militancy_province_issue, 0x012C, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(scaled_militancy_province_ideology, 0x012D, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(scaled_militancy_province_unemployment, 0x012E, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(scaled_consciousness_province_issue, 0x012F, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(scaled_consciousness_province_ideology, 0x0130, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(scaled_consciousness_province_unemployment, 0x0131, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(variable_good_name_province, 0x0132, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(treasury_province, 0x0133, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(country_event_this_state, 0x0134, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(country_event_immediate_this_state, 0x0135, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(province_event_this_state, 0x0136, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(province_event_immediate_this_state, 0x0137, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(country_event_this_province, 0x0138, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(country_event_immediate_this_province, 0x0139, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(province_event_this_province, 0x013A, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(province_event_immediate_this_province, 0x013B, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(country_event_this_pop, 0x013C, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(country_event_immediate_this_pop, 0x013D, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(province_event_this_pop, 0x013E, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(province_event_immediate_this_pop, 0x013F, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(country_event_province_this_nation, 0x0140, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(country_event_immediate_province_this_nation, 0x0141, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(country_event_province_this_state, 0x0142, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(country_event_immediate_province_this_state, 0x0143, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(country_event_province_this_province, 0x0144, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(country_event_immediate_province_this_province, 0x0145, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(country_event_province_this_pop, 0x0146, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(country_event_immediate_province_this_pop, 0x0147, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(activate_invention, 0x0148, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(variable_invention_name_no, 0x0149, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(add_core_tag_state, 0x014A, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(remove_core_tag_state, 0x014B, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(secede_province_state, 0x014C, effect_tp_flags::fp_two_places) \
    EFFECT_STATMENT(assimilate_state, 0x014D, effect_tp_flags::fp_two_places)

    // prototypes for functions
    void ef_none(EFFECT_DISPLAY_PARAMS);
#define EFFECT_STATMENT(x, n, f) void ef_##x (EFFECT_DISPLAY_PARAMS);
    EFFECT_STATMENTS
#undef EFFECT_STATMENT
    // table of functions (0 = for scopes and stuff)
constexpr inline void(* effect_functions[])(EFFECT_DISPLAY_PARAMS) = {
    ef_none,
#define EFFECT_STATMENT(x, n, f) effect_tooltip::ef_##x,
    EFFECT_STATMENTS
#undef EFFECT_STATMENT
};

inline constexpr int32_t indentation_amount = 15;

void display_subeffects(EFFECT_DISPLAY_PARAMS);

void make_effect_description(EFFECT_DISPLAY_PARAMS) {
    if((*eval & effect::code_mask) >= sizeof(effect_functions) / sizeof(effect_functions[0])) {
        auto box = text::open_layout_box(layout, indentation);
        text::add_to_layout_box(layout, state, box, std::string{"OUT_OF_BOUNDS"}, text::text_color::light_blue);
        text::close_layout_box(layout, box);
        display_subeffects(state, layout, eval, primary_slot, this_slot, from_slot, indentation);
        return;
    }
    effect_tooltip::effect_functions[*eval & effect::code_mask](state, layout, eval, primary_slot, this_slot, from_slot, 0);
}

void display_subeffects(EFFECT_DISPLAY_PARAMS) {
    if((eval[0] & effect::is_scope) == 0)
        return;
    const auto source_size = 1 + effect::get_generic_effect_payload_size(eval);
    // [code+scope] [payload size] [...]
    auto sub_units_start = eval + 2 + effect::effect_scope_data_payload(eval[0]);
    while(sub_units_start < eval + source_size) {
        make_effect_description(state, layout, sub_units_start, primary_slot, this_slot, from_slot, indentation + indentation_amount);
        sub_units_start += 1 + effect::get_generic_effect_payload_size(sub_units_start);
    }
}

void ef_none(EFFECT_DISPLAY_PARAMS) {
    display_subeffects(state, layout, eval, primary_slot, this_slot, from_slot, indentation);
}

#define EFFECT_STATMENT(x, n, f) \
void ef_##x (EFFECT_DISPLAY_PARAMS) { \
    auto box = text::open_layout_box(layout, indentation); \
    /* value_p points to the value corresponding to this statment, \
        for example if militancy is a scope, with 4 cells after it, \
        value_p will point to [base + 4] so we can properly retrieve the \
        value for display. */ \
    auto value_p = eval + 1; \
    /* if scope, skip [scope size] and the scope's associated payload */ \
    if(eval[0] & effect::is_scope) { \
        value_p += effect::get_effect_scope_payload_size(eval) - effect::get_effect_non_scope_payload_size(eval) + 1; \
        switch(eval[0]) { \
        case effect::tag_scope: \
            text::add_to_layout_box(layout, state, box, text::produce_simple_string(state, "tag_scope"), text::text_color::white); \
            break; \
        case effect::integer_scope: \
            text::add_to_layout_box(layout, state, box, text::produce_simple_string(state, "integer_scope"), text::text_color::white); \
            break; \
        case effect::pop_type_scope_nation: \
            text::add_to_layout_box(layout, state, box, text::produce_simple_string(state, "pop_type_scope_nation"), text::text_color::white); \
            break; \
        case effect::pop_type_scope_state: \
            text::add_to_layout_box(layout, state, box, text::produce_simple_string(state, "pop_type_scope_state"), text::text_color::white); \
            break; \
        case effect::pop_type_scope_province: \
            text::add_to_layout_box(layout, state, box, text::produce_simple_string(state, "pop_type_scope_province"), text::text_color::white); \
            break; \
        case effect::region_scope: \
            text::add_to_layout_box(layout, state, box, text::produce_simple_string(state, "region_scope"), text::text_color::white); \
            break; \
        default: \
            text::add_to_layout_box(layout, state, box, text::produce_simple_string(state, #x), text::text_color::white); \
            break; \
        } \
    } else { \
        text::add_to_layout_box(layout, state, box, text::produce_simple_string(state, #x), text::text_color::white); \
    } \
    text::add_space_to_layout_box(layout, state, box); \
    /* Constants */ \
    if(effect::get_effect_non_scope_payload_size(eval) != 0) { \
        switch((f) & effect_tp_flags::payload_type_mask) { \
        case effect_tp_flags::fp_to_integer: { \
            auto v = int32_t(trigger::read_float_from_payload(value_p)); \
            auto color = (f & effect_tp_flags::negative) \
                ? (v > 0 ? text::text_color::red : text::text_color::green) \
                : (v > 0 ? text::text_color::green : text::text_color::red); \
            auto s = std::to_string(v); \
            text::add_to_layout_box(layout, state, box, text::produce_simple_string(state, s), color); \
        } break; \
        case effect_tp_flags::integer: { \
            auto v = value_p[0];\
            auto color = (f & effect_tp_flags::negative) \
                ? (v > 0 ? text::text_color::red : text::text_color::green) \
                : (v > 0 ? text::text_color::green : text::text_color::red); \
            auto s = std::to_string(v); \
            text::add_to_layout_box(layout, state, box, text::produce_simple_string(state, s), color); \
        } break; \
        case effect_tp_flags::fp_one_place: { \
            auto v = trigger::read_float_from_payload(value_p); \
            auto color = (f & effect_tp_flags::negative) \
                ? (v > 0.f ? text::text_color::red : text::text_color::green) \
                : (v > 0.f ? text::text_color::green : text::text_color::red); \
            auto s = text::format_float(v, 1); \
            text::add_to_layout_box(layout, state, box, text::produce_simple_string(state, s), color); \
        } break; \
        case effect_tp_flags::fp_two_places: { \
            auto v = trigger::read_float_from_payload(value_p); \
            auto color = (f & effect_tp_flags::negative) \
                ? (v > 0.f ? text::text_color::red : text::text_color::green) \
                : (v > 0.f ? text::text_color::green : text::text_color::red); \
            auto s = text::format_float(v, 2); \
            text::add_to_layout_box(layout, state, box, text::produce_simple_string(state, s), color); \
        } break; \
        /* modifier names */ \
        case effect_tp_flags::modifier: { \
            auto mid = dcon::modifier_id(value_p[0] - 1); \
            auto duration = value_p[1]; \
            text::add_to_layout_box(layout, state, box, text::produce_simple_string(state, state.world.modifier_get_name(mid)), text::text_color::light_blue); \
            text::add_space_to_layout_box(layout, state, box); \
            text::add_to_layout_box(layout, state, box, std::to_string(duration), text::text_color::yellow); \
        } break; \
        case effect_tp_flags::modifier_no_duration: { \
            auto mid = dcon::modifier_id(value_p[0] - 1); \
            text::add_to_layout_box(layout, state, box, text::produce_simple_string(state, state.world.modifier_get_name(mid)), text::text_color::light_blue); \
        } break; \
        default: \
            break; \
        } \
    } \
    /*text::add_to_layout_box(layout, state, box, std::string{"("}, text::text_color::yellow); \
    for(int32_t i = 0; i < effect::get_effect_non_scope_payload_size(eval); i++) { \
        text::add_to_layout_box(layout, state, box, std::to_string(eval[i]), text::text_color::yellow); \
        text::add_space_to_layout_box(layout, state, box); \
    } \
    text::add_to_layout_box(layout, state, box, std::string{")"}, text::text_color::yellow);*/ \
    text::close_layout_box(layout, box); \
    /* Modifiers */ \
    if(effect::get_effect_non_scope_payload_size(eval) != 0) { \
        switch((f) & effect_tp_flags::payload_type_mask) { \
        case effect_tp_flags::modifier: \
        case effect_tp_flags::modifier_no_duration: { \
            auto mid = dcon::modifier_id(value_p[0] - 1); \
            modifier_description(state, layout, mid, indentation + indentation_amount); \
        } break; \
        default: \
            break; \
        } \
    } \
    display_subeffects(state, layout, eval, primary_slot, this_slot, from_slot, indentation); \
}
EFFECT_STATMENTS
#undef EFFECT_STATMENT

#undef EFFECT_STATMENTS
#undef EFFECT_DISPLAY_PARAMS
}

void effect_description(sys::state& state, text::columnar_layout& layout, dcon::effect_key k, int32_t primary_slot, int32_t this_slot, int32_t from_slot) {
    effect_tooltip::make_effect_description(state, layout, state.effect_data.data() + k.index(), primary_slot, this_slot, from_slot, 0);
}

}
