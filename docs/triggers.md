# Triggers and Effects

## Triggers

### What's a trigger?

Triggers are used as conditional statments, they only evaluate the current game state and return `TRUE` or `FALSE` upon each evaluation, think of them as asking a question.

### Trigger bytecode

The triggers are stored into a bytecode which can then be optimized to reduce internal bytecode size.

All of the bytecode is stored in 16-bit cells. It's important to know the starting position of the first scope/instruction because the bytecode is position sensitive.

For example, take the following sequence of trigger bytecode:

| Position | Data |
|---|---|
| 0 | `trigger::generic_scope` |
| 1 | `1` |

This describes a scope with a payload size 1. The payload includes everything within the scope + the size of the payload itself.

| Position | Data |
|---|---|
| 0 | `trigger::is_disjunctive_scope | trigger::generic_scope` |
| 1 | `8` |
| 2 | `trigger::association_ne | trigger::owns` |
| 3 | `100` |
| 4 | `trigger::x_core_scope_nation` |
| 5 | `4` |
| 6 | `trigger::association_lt | trigger::blockade` |
| 7 | `trigger::payload(float(2.0))` |

This is roughly equivalent to the following code:
```sh
OR = { #Disjuntive scope
    owns != 100 #Does not own province 100
    all_core = { #x_core_scope_nation
        blockade < 1 #blockade is < 1
    }
}
```
The `trigger::is_disjunctive_scope` tells us we're dealing with a scope where we have to choose only one path (as opposed to AND where it chooses all paths). `trigger::generic_scope` marks this 16-bit cell as pertaining to a scope. This means the following cell contains the payload size. In this case, 8 cells (including the payload size itself) which is correct. Now every child within the trigger scope will need to be parsed individually - this makes the bytecode very fragile so it's important to double check that it's being read from a valid position. 

`trigger::owns` is a question of "do we own the following?" then `trigger::association_ne` negates the conditional question into "do we NOT own the following?". With `100` being the Id of the province. To obtain the fixed size of non-scope bytecode instructions a lookup table is used.

Afterwards, we see a `trigger::x_core_scope_nation` which corresponds to `all_core = { ... }` (`any_core` has an aditional OR'ed `trigger::is_existence_scope`). Next, the payload size, which is 4, this means the total data (plus the payload) has a size of 4. Then we obtain the first conditional, which is a `trigger::association_lt` "this is less than" OR'ed with a `trigger::blockade`
 "is this blockaded?", to form the question: "is the blockade value, less than x?", where `x` corresponds to the first cell, which is encoded from a float into an uint16_t by the means of `trigger::payload(float(2.0))`.

### Packed floats

Sometimes one might see that the size of a trigger's payload is 2 (on the table below for example), but it only has 1 argument, before going to fix the supposed bug, this is an intentional feature. Floats are stored as two consecutive `uint16_t` cells that are composed by packing them into an union and reading them back as their binary form.

- `void add_float_to_payload(float f)` : Allows to write the float payload.
- `float read_float_from_payload(uint16_t const* data)` : Allows to read the float payload back.

### List of triggers

Not an exhaustive list of triggers, be wary the trigger codes **may** have flags alongside them, so accounting for that is very important in order to parse them correctly. It's as simple as performing a bitwise AND, as such: `value & trigger::code_mask` (where `value` is the `uint16_t` value of the cell that is believed to be the code of the trigger). Triggers only evaluate 

| Code | Name | Number of arguments | Notes/Arguments |
|---|---|---|---|
| 0x0000 | none | 0 | Empty, dummy trigger that does nothing |
| 0x0001 | year | 1 | Compares to the year #1 (`uint16_t`) |
| 0x0002 | month | 1 | Compares the current month to #1 (`uint16_t`) |
| 0x0003 | port | 0 | Does this `dcon::province_id` have a port? |
| 0x0004 | rank | 1 | Compares the rank of the primary `dcon::nation_id` to #1 (`uint16_t`) | 
| 0x0005 | technology | 1 | Checks that primary `dcon::nation_id` has technology #1 (`dcon::technology_id`) |
| 0x0006 | strata_rich | 0 | Is the primary `dcon::pop_id` part of the rich strata? |
| 0x0007 | life_rating_province | 1 | Checks the liferating of the primary `dcon::province_id` with #1 (`uint16_t`) |
| 0x0008 | life_rating_state | 1 | Checks the liferating of the primary `dcon::state_instance_id` with #1 (`uint16_t`) |
| 0x0009 | has_empty_adjacent_state_province | 0 | Does the primary `dcon::province_id` have an empty adjacent state? |
| 0x000A | has_empty_adjacent_state_state | 0 | Does the primary `dcon::state_instance_id` have an empty adjacent state? |
| 0x000B | state_id_province | 1 | Does the primary `dcon::province_id` pertain to the same state as #1 (`dcon::province_id`)? |
| 0x000C | state_id_state | 1 | Does the primary `dcon::state_instance_id` pertain to the same state as #1 (`dcon::province_id`)? |
| 0x000D | cash_reserves | 2 | Tests that the primary `dcon::pop_id` has an amount of cash reserves, told by #1 (`float`) |
| 0x000E | unemployment_nation | 2 | Unemployment of the primary `dcon::nation_id` has #1 (`float`) |
| 0x000F | unemployment_state | 2 | Unemployment of the primary `dcon::state_instance_id` has #1 (`float`) |
| 0x0010 | unemployment_province | 2 | Unemployment of the primary `dcon::province_id` has #1 (`float`) |
| 0x0011 | unemployment_pop | 2 | Unemployment of the primary `dcon::pop_id` has #1 (`float`) |
| 0x0012 | is_slave_nation | 0 | Is the primary `dcon::nation_id` a nation that allows slavery? |
| 0x0013 | is_slave_state | 0 | Is the primary `dcon::state_instance_id` a slave state? |
| 0x0014 | is_slave_province | 0 | Is the primary `dcon::province_id` a province pertaining to a slave state? |
| 0x0015 | is_slave_pop | 0 | Is the primary `dcon::pop_id` a slave? |
| 0x0016 | is_independant | 0 | Is the primary `dcon::nation_id` independent? (No puppet overlord and not a substate) |
| 0x0017 | has_national_minority_province | 0 | **All** pops on `dcon::province_id` are of non-primary culture | 
| 0x0018 | has_national_minority_state | 0 | **All** pops on `dcon::state_instance_id` are of non-primary culture |
| 0x0019 | has_national_minority_nation | 0 | **All** pops on `dcon::nation_id` are of non-primary culture |
| 0x001A | government_nation | 1 | Government of primary `dcon::nation_id` are #1 (`dcon::goverment_type_id`) |
| 0x001B | government_pop | 1 | Government of primary `dcon::pop_id` is #1 (`dcon::goverment_type`) |
| 0x001C | capital | 1 | Checks that primary `dcon::nation_id` capital is #1 (`dcon::province_id`) |
| 0x001D | tech_school | 1 | 
| 0x001E | primary_culture | 1 | 
| 0x001F | accepted_culture | 1 | 
| 0x0020 | culture_pop | 1 | 
| 0x0021 | culture_state | 1 | 
| 0x0022 | culture_province | 1 | 
| 0x0023 | culture_nation | 1 | 
| 0x0024 | culture_pop_reb | 0 | 
| 0x0025 | culture_state_reb | 0 | 
| 0x0026 | culture_province_reb | 0 | 
| 0x0027 | culture_nation_reb | 0 | 
| 0x0028 | culture_from_nation | 0 | 
| 0x0029 | culture_this_nation | 0 | 
| 0x002A | culture_this_state | 0 | 
| 0x002B | culture_this_pop | 0 | 
| 0x002C | culture_this_province | 0 | 
| 0x002D | culture_group_nation | 1 | 
| 0x002E | culture_group_pop | 1 | 
| 0x002F | culture_group_reb_nation | 0 | 
| 0x0030 | culture_group_reb_pop | 0 | 
| 0x0031 | culture_group_nation_from_nation | 0 | 
| 0x0032 | culture_group_pop_from_nation | 0 | 
| 0x0033 | culture_group_nation_this_nation | 0 | 
| 0x0034 | culture_group_pop_this_nation | 0 | 
| 0x0035 | culture_group_nation_this_province | 0 | 
| 0x0036 | culture_group_pop_this_province | 0 | 
| 0x0037 | culture_group_nation_this_state | 0 | 
| 0x0038 | culture_group_pop_this_state | 0 | 
| 0x0039 | culture_group_nation_this_pop | 0 | 
| 0x003A | culture_group_pop_this_pop | 0 | 
| 0x003B | religion | 1 | 
| 0x003C | religion_reb | 0 | 
| 0x003D | religion_from_nation | 0 | 
| 0x003E | religion_this_nation | 0 | 
| 0x003F | religion_this_state | 0 | 
| 0x0040 | religion_this_province | 0 | 
| 0x0041 | religion_this_pop | 0 | 
| 0x0042 | terrain_province | 1 | 
| 0x0043 | terrain_pop | 1 | 
| 0x0044 | trade_goods | 1 | 
| 0x0045 | is_secondary_power_pop | 0 | 
| 0x0046 | is_secondary_power_nation | 0 | 
| 0x0047 | has_faction_nation | 0 | 
| 0x0048 | has_faction_pop | 1 | 
| 0x0049 | has_unclaimed_cores | 0 | 
| 0x004A | is_cultural_union_bool | 0 | 
| 0x004B | is_cultural_union_this_self_pop | 0 | 
| 0x004C | is_cultural_union_this_pop | 0 | 
| 0x004D | is_cultural_union_this_state | 0 | 
| 0x004E | is_cultural_union_this_province | 0 | 
| 0x004F | is_cultural_union_this_nation | 0 | 
| 0x0050 | is_cultural_union_this_rebel | 0 | 
| 0x0051 | is_cultural_union_tag_nation | 1 | 
| 0x0052 | is_cultural_union_tag_this_pop | 1 | 
| 0x0053 | is_cultural_union_tag_this_state | 1 | 
| 0x0054 | is_cultural_union_tag_this_province | 1 | 
| 0x0055 | is_cultural_union_tag_this_nation | 1 | 
| 0x0056 | can_build_factory_pop | 0 | 
| 0x0057 | war_pop | 0 | 
| 0x0058 | war_nation | 0 | 
| 0x0059 | war_exhaustion_nation | 2 | War exhaustion of primary `dcon::nation_id` compared with #1 (`float`) |
| 0x005A | blockade | 2 | 
| 0x005B | owns | 1 | Primary `dcon::nation_id` owns #1 (`dcon::province_id`) |
| 0x005C | controls | 1 | Primary `dcon::nation_id` controls #1 (`dcon::province_id`) |
| 0x005D | is_core_integer | 1 | 
| 0x005E | is_core_this_nation | 0 | 
| 0x005F | is_core_this_state | 0 | 
| 0x0060 | is_core_this_province | 0 | 
| 0x0061 | is_core_this_pop | 0 | 
| 0x0062 | is_core_from_nation | 0 | 
| 0x0063 | is_core_reb | 0 | 
| 0x0064 | is_core_tag | 1 | 
| 0x0065 | num_of_revolts | 1 | 
| 0x0066 | revolt_percentage | 2 | 
| 0x0067 | num_of_cities_int | 1 | 
| 0x0068 | num_of_cities_from_nation | 0 | 
| 0x0069 | num_of_cities_this_nation | 0 | 
| 0x006A | num_of_cities_this_state | 0 | 
| 0x006B | num_of_cities_this_province | 0 | 
| 0x006C | num_of_cities_this_pop | 0 | 
| 0x006D | num_of_ports | 1 | 
| 0x006E | num_of_allies | 1 | 
| 0x006F | num_of_vassals | 1 | 
| 0x0070 | owned_by_tag | 1 | 
| 0x0071 | owned_by_from_nation | 0 | 
| 0x0072 | owned_by_this_nation | 0 | 
| 0x0073 | owned_by_this_province | 0 | 
| 0x0074 | owned_by_this_state | 0 | 
| 0x0075 | owned_by_this_pop | 0 | 
| 0x0076 | exists_bool | 0 | 
| 0x0077 | exists_tag | 1 | 
| 0x0078 | has_country_flag | 1 | 
| 0x0079 | continent_nation | 1 | 
| 0x007A | continent_state | 1 | 
| 0x007B | continent_province | 1 | 
| 0x007C | continent_pop | 1 | 
| 0x007D | continent_nation_this | 0 | 
| 0x007E | continent_state_this | 0 | 
| 0x007F | continent_province_this | 0 | 
| 0x0080 | continent_pop_this | 0 | 
| 0x0081 | continent_nation_from | 0 | 
| 0x0082 | continent_state_from | 0 | 
| 0x0083 | continent_province_from | 0 | 
| 0x0084 | continent_pop_from | 0 | 
| 0x0085 | casus_belli_tag | 1 | 
| 0x0086 | casus_belli_from | 0 | 
| 0x0087 | casus_belli_this_nation | 0 | 
| 0x0088 | casus_belli_this_state | 0 | 
| 0x0089 | casus_belli_this_province | 0 | 
| 0x008A | casus_belli_this_pop | 0 | 
| 0x008B | military_access_tag | 1 | 
| 0x008C | military_access_from | 0 | 
| 0x008D | military_access_this_nation | 0 | 
| 0x008E | military_access_this_state | 0 | 
| 0x008F | military_access_this_province | 0 | 
| 0x0090 | military_access_this_pop | 0 | 
| 0x0091 | prestige_value | 2 | 
| 0x0092 | prestige_from | 0 | 
| 0x0093 | prestige_this_nation | 0 | 
| 0x0094 | prestige_this_state | 0 | 
| 0x0095 | prestige_this_province | 0 | 
| 0x0096 | prestige_this_pop | 0 | 
| 0x0097 | badboy | 2 | 
| 0x0098 | has_building_state | 1 | 
| 0x0099 | has_building_fort | 0 | 
| 0x009A | has_building_railroad | 0 | 
| 0x009B | has_building_naval_base | 0 | 
| 0x009C | empty | 0 | 
| 0x009D | is_blockaded | 0 | 
| 0x009E | has_country_modifier | 1 | 
| 0x009F | has_province_modifier | 1 | 
| 0x00A0 | region | 1 | 
| 0x00A1 | tag_tag | 1 | 
| 0x00A2 | tag_this_nation | 0 | 
| 0x00A3 | tag_this_province | 0 | 
| 0x00A4 | tag_from_nation | 0 | 
| 0x00A5 | tag_from_province | 0 | 
| 0x00A6 | neighbour_tag | 1 | 
| 0x00A7 | neighbour_this | 0 | 
| 0x00A8 | neighbour_from | 0 | 
| 0x00A9 | units_in_province_value | 1 | 
| 0x00AA | units_in_province_from | 0 | 
| 0x00AB | units_in_province_this_nation | 0 | 
| 0x00AC | units_in_province_this_province | 0 | 
| 0x00AD | units_in_province_this_state | 0 | 
| 0x00AE | units_in_province_this_pop | 0 | 
| 0x00AF | war_with_tag | 1 | 
| 0x00B0 | war_with_from | 0 | 
| 0x00B1 | war_with_this_nation | 0 | 
| 0x00B2 | war_with_this_province | 0 | 
| 0x00B3 | war_with_this_state | 0 | 
| 0x00B4 | war_with_this_pop | 0 | 
| 0x00B5 | unit_in_battle | 0 | 
| 0x00B6 | total_amount_of_divisions | 1 | 
| 0x00B7 | money | 2 | 
| 0x00B8 | lost_national | 2 | 
| 0x00B9 | is_vassal | 0 | 
| 0x00BA | ruling_party_ideology_pop | 1 | 
| 0x00BB | ruling_party_ideology_nation | 1 | 
| 0x00BC | ruling_party | 1 | 
| 0x00BD | is_ideology_enabled | 1 | 
| 0x00BE | political_reform_want_nation | 2 | 
| 0x00BF | political_reform_want_pop | 2 | 
| 0x00C0 | social_reform_want_nation | 2 | 
| 0x00C1 | social_reform_want_pop | 2 | 
| 0x00C2 | total_amount_of_ships | 1 | 
| 0x00C3 | plurality | 2 | 
| 0x00C4 | corruption | 2 | 
| 0x00C5 | is_state_religion_pop | 0 | 
| 0x00C6 | is_state_religion_province | 0 | 
| 0x00C7 | is_state_religion_state | 0 | 
| 0x00C8 | is_primary_culture_pop | 0 | 
| 0x00C9 | is_primary_culture_province | 0 | 
| 0x00CA | is_primary_culture_state | 0 | 
| 0x00CB | is_primary_culture_nation_this_pop | 0 | 
| 0x00CC | is_primary_culture_nation_this_nation | 0 | 
| 0x00CD | is_primary_culture_nation_this_state | 0 | 
| 0x00CE | is_primary_culture_nation_this_province | 0 | 
| 0x00CF | is_primary_culture_state_this_pop | 0 | 
| 0x00D0 | is_primary_culture_state_this_nation | 0 | 
| 0x00D1 | is_primary_culture_state_this_state | 0 | 
| 0x00D2 | is_primary_culture_state_this_province | 0 | 
| 0x00D3 | is_primary_culture_province_this_pop | 0 | 
| 0x00D4 | is_primary_culture_province_this_nation | 0 | 
| 0x00D5 | is_primary_culture_province_this_state | 0 | 
| 0x00D6 | is_primary_culture_province_this_province | 0 | 
| 0x00D7 | is_primary_culture_pop_this_pop | 0 | 
| 0x00D8 | is_primary_culture_pop_this_nation | 0 | 
| 0x00D9 | is_primary_culture_pop_this_state | 0 | 
| 0x00DA | is_primary_culture_pop_this_province | 0 | 
| 0x00DB | is_accepted_culture_pop | 0 | 
| 0x00DC | is_accepted_culture_province | 0 | 
| 0x00DD | is_accepted_culture_state | 0 | 
| 0x00DE | is_coastal | 0 | 
| 0x00DF | in_sphere_tag | 1 | 
| 0x00E0 | in_sphere_from | 0 | 
| 0x00E1 | in_sphere_this_nation | 0 | 
| 0x00E2 | in_sphere_this_province | 0 | 
| 0x00E3 | in_sphere_this_state | 0 | 
| 0x00E4 | in_sphere_this_pop | 0 | 
| 0x00E5 | produces_nation | 1 | 
| 0x00E6 | produces_state | 1 | 
| 0x00E7 | produces_province | 1 | 
| 0x00E8 | produces_pop | 1 | 
| 0x00E9 | average_militancy_nation | 2 | 
| 0x00EA | average_militancy_state | 2 | 
| 0x00EB | average_militancy_province | 2 | 
| 0x00EC | average_consciousness_nation | 2 | 
| 0x00ED | average_consciousness_state | 2 | 
| 0x00EE | average_consciousness_province | 2 | 
| 0x00EF | is_next_reform_nation | 1 | 
| 0x00F0 | is_next_reform_pop | 1 | 
| 0x00F1 | rebel_power_fraction | 2 | 
| 0x00F2 | recruited_percentage_nation | 2 | 
| 0x00F3 | recruited_percentage_pop | 2 | 
| 0x00F4 | has_culture_core | 0 | 
| 0x00F5 | nationalism | 1 | 
| 0x00F6 | is_overseas | 0 | 
| 0x00F7 | controlled_by_rebels | 0 | 
| 0x00F8 | controlled_by_tag | 1 | 
| 0x00F9 | controlled_by_from | 0 | 
| 0x00FA | controlled_by_this_nation | 0 | 
| 0x00FB | controlled_by_this_province | 0 | 
| 0x00FC | controlled_by_this_state | 0 | 
| 0x00FD | controlled_by_this_pop | 0 | 
| 0x00FE | controlled_by_owner | 0 | 
| 0x00FF | controlled_by_reb | 0 | 
| 0x0100 | is_canal_enabled | 1 | 
| 0x0101 | is_state_capital | 0 | 
| 0x0102 | truce_with_tag | 1 | 
| 0x0103 | truce_with_from | 0 | 
| 0x0104 | truce_with_this_nation | 0 | 
| 0x0105 | truce_with_this_province | 0 | 
| 0x0106 | truce_with_this_state | 0 | 
| 0x0107 | truce_with_this_pop | 0 | 
| 0x0108 | total_pops_nation | 2 | 
| 0x0109 | total_pops_state | 2 | 
| 0x010A | total_pops_province | 2 | 
| 0x010B | total_pops_pop | 2 | 
| 0x010C | has_pop_type_nation | 1 | 
| 0x010D | has_pop_type_state | 1 | 
| 0x010E | has_pop_type_province | 1 | 
| 0x010F | has_pop_type_pop | 1 | 
| 0x0110 | has_empty_adjacent_province | 0 | 
| 0x0111 | has_leader | 1 | 
| 0x0112 | ai | 0 | 
| 0x0113 | can_create_vassals | 0 | 
| 0x0114 | is_possible_vassal | 1 | 
| 0x0115 | province_id | 1 | 
| 0x0116 | vassal_of_tag | 1 | 
| 0x0117 | vassal_of_from | 0 | 
| 0x0118 | vassal_of_this_nation | 0 | 
| 0x0119 | vassal_of_this_province | 0 | 
| 0x011A | vassal_of_this_state | 0 | 
| 0x011B | vassal_of_this_pop | 0 | 
| 0x011C | alliance_with_tag | 1 | 
| 0x011D | alliance_with_from | 0 | 
| 0x011E | alliance_with_this_nation | 0 | 
| 0x011F | alliance_with_this_province | 0 | 
| 0x0120 | alliance_with_this_state | 0 | 
| 0x0121 | alliance_with_this_pop | 0 | 
| 0x0122 | has_recently_lost_war | 0 | 
| 0x0123 | is_mobilised | 0 | 
| 0x0124 | mobilisation_size | 2 | 
| 0x0125 | crime_higher_than_education_nation | 0 | 
| 0x0126 | crime_higher_than_education_state | 0 | 
| 0x0127 | crime_higher_than_education_province | 0 | 
| 0x0128 | crime_higher_than_education_pop | 0 | 
| 0x0129 | agree_with_ruling_party | 2 | 
| 0x012A | is_colonial_state | 0 | 
| 0x012B | is_colonial_province | 0 | 
| 0x012C | has_factories | 0 | 
| 0x012D | in_default_tag | 1 | 
| 0x012E | in_default_from | 0 | 
| 0x012F | in_default_this_nation | 0 | 
| 0x0130 | in_default_this_province | 0 | 
| 0x0131 | in_default_this_state | 0 | 
| 0x0132 | in_default_this_pop | 0 | 
| 0x0133 | total_num_of_ports | 1 | 
| 0x0134 | always | 0 | 
| 0x0135 | election | 0 | 
| 0x0136 | has_global_flag | 1 | 
| 0x0137 | is_capital | 0 | 
| 0x0138 | nationalvalue_nation | 1 | 
| 0x0139 | industrial_score_value | 1 | 
| 0x013A | industrial_score_from_nation | 0 | 
| 0x013B | industrial_score_this_nation | 0 | 
| 0x013C | industrial_score_this_pop | 0 | 
| 0x013D | industrial_score_this_state | 0 | 
| 0x013E | industrial_score_this_province | 0 | 
| 0x013F | military_score_value | 1 | 
| 0x0140 | military_score_from_nation | 0 | 
| 0x0141 | military_score_this_nation | 0 | 
| 0x0142 | military_score_this_pop | 0 | 
| 0x0143 | military_score_this_state | 0 | 
| 0x0144 | military_score_this_province | 0 | 
| 0x0145 | civilized_nation | 0 | 
| 0x0146 | civilized_pop | 0 | 
| 0x0147 | civilized_province | 0 | 
| 0x0148 | national_provinces_occupied | 2 | 
| 0x0149 | is_greater_power_nation | 0 | 
| 0x014A | is_greater_power_pop | 0 | 
| 0x014B | rich_tax | 1 | 
| 0x014C | middle_tax | 1 | 
| 0x014D | poor_tax | 1 | 
| 0x014E | social_spending_nation | 1 | 
| 0x014F | social_spending_pop | 1 | 
| 0x0150 | social_spending_province | 1 | 
| 0x0151 | colonial_nation | 0 | 
| 0x0152 | pop_majority_religion_nation | 1 | 
| 0x0153 | pop_majority_religion_state | 1 | 
| 0x0154 | pop_majority_religion_province | 1 | 
| 0x0155 | pop_majority_culture_nation | 1 | 
| 0x0156 | pop_majority_culture_state | 1 | 
| 0x0157 | pop_majority_culture_province | 1 | 
| 0x0158 | pop_majority_issue_nation | 2 | 
| 0x0159 | pop_majority_issue_state | 2 | 
| 0x015A | pop_majority_issue_province | 2 | 
| 0x015B | pop_majority_issue_pop | 2 | 
| 0x015C | pop_majority_ideology_nation | 1 | 
| 0x015D | pop_majority_ideology_state | 1 | 
| 0x015E | pop_majority_ideology_province | 1 | 
| 0x015F | pop_majority_ideology_pop | 1 | 
| 0x0160 | poor_strata_militancy_nation | 2 | 
| 0x0161 | poor_strata_militancy_state | 2 | 
| 0x0162 | poor_strata_militancy_province | 2 | 
| 0x0163 | poor_strata_militancy_pop | 2 | 
| 0x0164 | middle_strata_militancy_nation | 2 | 
| 0x0165 | middle_strata_militancy_state | 2 | 
| 0x0166 | middle_strata_militancy_province | 2 | 
| 0x0167 | middle_strata_militancy_pop | 2 | 
| 0x0168 | rich_strata_militancy_nation | 2 | 
| 0x0169 | rich_strata_militancy_state | 2 | 
| 0x016A | rich_strata_militancy_province | 2 | 
| 0x016B | rich_strata_militancy_pop | 2 | 
| 0x016C | rich_tax_above_poor | 0 | 
| 0x016D | culture_has_union_tag_pop | 0 | 
| 0x016E | culture_has_union_tag_nation | 0 | 
| 0x016F | this_culture_union_tag | 1 | 
| 0x0170 | this_culture_union_from | 0 | 
| 0x0171 | this_culture_union_this_nation | 0 | 
| 0x0172 | this_culture_union_this_province | 0 | 
| 0x0173 | this_culture_union_this_state | 0 | 
| 0x0174 | this_culture_union_this_pop | 0 | 
| 0x0175 | this_culture_union_this_union_nation | 0 | 
| 0x0176 | this_culture_union_this_union_province | 0 | 
| 0x0177 | this_culture_union_this_union_state | 0 | 
| 0x0178 | this_culture_union_this_union_pop | 0 | 
| 0x0179 | minorities_nation | 0 | Primary `dcon::nation_id`
| 0x017A | minorities_state | 0 | 
| 0x017B | minorities_province | 0 | 
| 0x017C | revanchism_nation | 2 | 
| 0x017D | revanchism_pop | 2 | 
| 0x017E | has_crime | 1 | 
| 0x017F | num_of_substates | 1 | 
| 0x0180 | num_of_vassals_no_substates | 1 | 
| 0x0181 | brigades_compare_this | 2 | 
| 0x0182 | brigades_compare_from | 2 | 
| 0x0183 | constructing_cb_tag | 1 | 
| 0x0184 | constructing_cb_from | 0 | 
| 0x0185 | constructing_cb_this_nation | 0 | 
| 0x0186 | constructing_cb_this_province | 0 | 
| 0x0187 | constructing_cb_this_state | 0 | 
| 0x0188 | constructing_cb_this_pop | 0 | 
| 0x0189 | constructing_cb_discovered | 0 | 
| 0x018A | constructing_cb_progress | 2 | 
| 0x018B | civilization_progress | 2 | 
| 0x018C | constructing_cb_type | 1 | 
| 0x018D | is_our_vassal_tag | 1 | 
| 0x018E | is_our_vassal_from | 0 | 
| 0x018F | is_our_vassal_this_nation | 0 | 
| 0x0190 | is_our_vassal_this_province | 0 | 
| 0x0191 | is_our_vassal_this_state | 0 | 
| 0x0192 | is_our_vassal_this_pop | 0 | 
| 0x0193 | substate_of_tag | 1 | 
| 0x0194 | substate_of_from | 0 | 
| 0x0195 | substate_of_this_nation | 0 | 
| 0x0196 | substate_of_this_province | 0 | 
| 0x0197 | substate_of_this_state | 0 | 
| 0x0198 | substate_of_this_pop | 0 | 
| 0x0199 | is_substate | 0 | 
| 0x019A | great_wars_enabled | 0 | 
| 0x019B | can_nationalize | 0 | 
| 0x019C | part_of_sphere | 0 | 
| 0x019D | is_sphere_leader_of_tag | 1 | 
| 0x019E | is_sphere_leader_of_from | 0 | 
| 0x019F | is_sphere_leader_of_this_nation | 0 | 
| 0x01A0 | is_sphere_leader_of_this_province | 0 | 
| 0x01A1 | is_sphere_leader_of_this_state | 0 | 
| 0x01A2 | is_sphere_leader_of_this_pop | 0 | 
| 0x01A3 | number_of_states | 1 | 
| 0x01A4 | war_score | 2 | 
| 0x01A5 | is_releasable_vassal_from | 0 | 
| 0x01A6 | is_releasable_vassal_other | 0 | 
| 0x01A7 | has_recent_imigration | 1 | 
| 0x01A8 | province_control_days | 1 | 
| 0x01A9 | is_disarmed | 0 | 
| 0x01AA | big_producer | 1 | 
| 0x01AB | someone_can_form_union_tag_from | 0 | 
| 0x01AC | someone_can_form_union_tag_other | 0 | 
| 0x01AD | social_movement_strength | 2 | 
| 0x01AE | political_movement_strength | 2 | 
| 0x01AF | can_build_factory_in_capital_state | 1 | 
| 0x01B0 | social_movement | 0 | 
| 0x01B1 | political_movement | 0 | 
| 0x01B2 | has_cultural_sphere | 0 | 
| 0x01B3 | world_wars_enabled | 0 | 
| 0x01B4 | has_pop_culture_pop_this_pop | 0 | 
| 0x01B5 | has_pop_culture_state_this_pop | 0 | 
| 0x01B6 | has_pop_culture_province_this_pop | 0 | 
| 0x01B7 | has_pop_culture_nation_this_pop | 0 | 
| 0x01B8 | has_pop_culture_pop | 1 | 
| 0x01B9 | has_pop_culture_state | 1 | 
| 0x01BA | has_pop_culture_province | 1 | 
| 0x01BB | has_pop_culture_nation | 1 | 
| 0x01BC | has_pop_religion_pop_this_pop | 0 | 
| 0x01BD | has_pop_religion_state_this_pop | 0 | 
| 0x01BE | has_pop_religion_province_this_pop | 0 | 
| 0x01BF | has_pop_religion_nation_this_pop | 0 | 
| 0x01C0 | has_pop_religion_pop | 1 | 
| 0x01C1 | has_pop_religion_state | 1 | 
| 0x01C2 | has_pop_religion_province | 1 | 
| 0x01C3 | has_pop_religion_nation | 1 | 
| 0x01C4 | life_needs | 2 | Life needs of primary `dcon::pop_id` compared against #1 (`float`) |
| 0x01C5 | everyday_needs | 2 | Everyday needs of primary `dcon::pop_id` compared against #1 (`float`) |
| 0x01C6 | luxury_needs | 2 | Luxury needs of primary `dcon::pop_id` compared against #1 (`float`) |
| 0x01C7 | consciousness_pop | 2 | Consciousness of primary `dcon::pop_id` compared against #1 (`float`) |
| 0x01C8 | consciousness_province | 2 | Average consciousness of primary `dcon::province_id` compared against #1 (`float`) |
| 0x01C9 | consciousness_state | 2 | Average consciousness of primary `dcon::state_instance_id` compared against #1 (`float`) |
| 0x01CA | consciousness_nation | 2 | Average consciousness of primary `dcon::nation_id` compared against #1 (`float`) |
| 0x01CB | literacy_pop | 2 | 
| 0x01CC | literacy_province | 2 | 
| 0x01CD | literacy_state | 2 | 
| 0x01CE | literacy_nation | 2 | 
| 0x01CF | militancy_pop | 2 | Militancy of primary `dcon::pop_id` compared against #1 (`float`) |
| 0x01D0 | militancy_province | 2 | Average militancy of primary `dcon::province_id` compared against #1 (`float`) |
| 0x01D1 | militancy_state | 2 | Average militancy of primary `dcon::state_instance_id` compared against #1 (`float`) |
| 0x01D2 | militancy_nation | 2 | Average militancy of primary `dcon::nation_id` compared against #1 (`float`) |
| 0x01D3 | military_spending_pop | 1 | 
| 0x01D4 | military_spending_province | 1 | 
| 0x01D5 | military_spending_state | 1 | 
| 0x01D6 | military_spending_nation | 1 | 
| 0x01D7 | administration_spending_pop | 1 | 
| 0x01D8 | administration_spending_province | 1 | 
| 0x01D9 | administration_spending_state | 1 | 
| 0x01DA | administration_spending_nation | 1 | 
| 0x01DB | education_spending_pop | 1 | 
| 0x01DC | education_spending_province | 1 | 
| 0x01DD | education_spending_state | 1 | 
| 0x01DE | education_spending_nation | 1 | 
| 0x01DF | trade_goods_in_state_state | 1 | 
| 0x01E0 | trade_goods_in_state_province | 1 | 
| 0x01E1 | has_flashpoint | 0 | 
| 0x01E2 | flashpoint_tension | 2 | 
| 0x01E3 | crisis_exist | 0 | Is there an active crisis right now? |
| 0x01E4 | is_liberation_crisis | 0 | 
| 0x01E5 | is_claim_crisis | 0 | 
| 0x01E6 | crisis_temperature | 2 | Crisis temperature compared with #1 (`float`) |
| 0x01E7 | involved_in_crisis_pop | 0 | 
| 0x01E8 | involved_in_crisis_nation | 0 | 
| 0x01E9 | rich_strata_life_needs_nation | 2 | 
| 0x01EA | rich_strata_life_needs_state | 2 | 
| 0x01EB | rich_strata_life_needs_province | 2 | 
| 0x01EC | rich_strata_life_needs_pop | 2 | 
| 0x01ED | rich_strata_everyday_needs_nation | 2 | 
| 0x01EE | rich_strata_everyday_needs_state | 2 | 
| 0x01EF | rich_strata_everyday_needs_province | 2 | 
| 0x01F0 | rich_strata_everyday_needs_pop | 2 | 
| 0x01F1 | rich_strata_luxury_needs_nation | 2 | 
| 0x01F2 | rich_strata_luxury_needs_state | 2 | 
| 0x01F3 | rich_strata_luxury_needs_province | 2 | 
| 0x01F4 | rich_strata_luxury_needs_pop | 2 | 
| 0x01F5 | middle_strata_life_needs_nation | 2 | 
| 0x01F6 | middle_strata_life_needs_state | 2 | 
| 0x01F7 | middle_strata_life_needs_province | 2 | 
| 0x01F8 | middle_strata_life_needs_pop | 2 | 
| 0x01F9 | middle_strata_everyday_needs_nation | 2 | 
| 0x01FA | middle_strata_everyday_needs_state | 2 | 
| 0x01FB | middle_strata_everyday_needs_province | 2 | 
| 0x01FC | middle_strata_everyday_needs_pop | 2 | 
| 0x01FD | middle_strata_luxury_needs_nation | 2 | 
| 0x01FE | middle_strata_luxury_needs_state | 2 | 
| 0x01FF | middle_strata_luxury_needs_province | 2 | 
| 0x0200 | middle_strata_luxury_needs_pop | 2 | 
| 0x0201 | poor_strata_life_needs_nation | 2 | 
| 0x0202 | poor_strata_life_needs_state | 2 | 
| 0x0203 | poor_strata_life_needs_province | 2 | 
| 0x0204 | poor_strata_life_needs_pop | 2 | 
| 0x0205 | poor_strata_everyday_needs_nation | 2 | 
| 0x0206 | poor_strata_everyday_needs_state | 2 | 
| 0x0207 | poor_strata_everyday_needs_province | 2 | 
| 0x0208 | poor_strata_everyday_needs_pop | 2 | 
| 0x0209 | poor_strata_luxury_needs_nation | 2 | 
| 0x020A | poor_strata_luxury_needs_state | 2 | 
| 0x020B | poor_strata_luxury_needs_province | 2 | 
| 0x020C | poor_strata_luxury_needs_pop | 2 | 
| 0x020D | diplomatic_influence_tag | 2 | 
| 0x020E | diplomatic_influence_this_nation | 1 | 
| 0x020F | diplomatic_influence_this_province | 1 | 
| 0x0210 | diplomatic_influence_from_nation | 1 | 
| 0x0211 | diplomatic_influence_from_province | 1 | 
| 0x0212 | pop_unemployment_nation | 3 | 
| 0x0213 | pop_unemployment_state | 3 | 
| 0x0214 | pop_unemployment_province | 3 | 
| 0x0215 | pop_unemployment_pop | 3 | 
| 0x0216 | pop_unemployment_nation_this_pop | 2 | 
| 0x0217 | pop_unemployment_state_this_pop | 2 | 
| 0x0218 | pop_unemployment_province_this_pop | 2 | 
| 0x0219 | relation_tag | 2 | 
| 0x021A | relation_this_nation | 1 | 
| 0x021B | relation_this_province | 1 | 
| 0x021C | relation_from_nation | 1 | 
| 0x021D | relation_from_province | 1 | 
| 0x021E | check_variable | 3 | 
| 0x021F | upper_house | 3 | 
| 0x0220 | unemployment_by_type_nation | 3 | 
| 0x0221 | unemployment_by_type_state | 3 | 
| 0x0222 | unemployment_by_type_province | 3 | 
| 0x0223 | unemployment_by_type_pop | 3 | 
| 0x0224 | party_loyalty_nation_province_id | 3 | 
| 0x0225 | party_loyalty_from_nation_province_id | 3 | 
| 0x0226 | party_loyalty_province_province_id | 3 | 
| 0x0227 | party_loyalty_from_province_province_id | 3 | 
| 0x0228 | party_loyalty_nation_from_province | 2 | 
| 0x0229 | party_loyalty_from_nation_scope_province | 2 | 
| 0x022A | can_build_in_province_railroad_no_limit_from_nation | 0 | 
| 0x022B | can_build_in_province_railroad_yes_limit_from_nation | 0 | 
| 0x022C | can_build_in_province_railroad_no_limit_this_nation | 0 | 
| 0x022D | can_build_in_province_railroad_yes_limit_this_nation | 0 | 
| 0x022E | can_build_in_province_fort_no_limit_from_nation | 0 | 
| 0x022F | can_build_in_province_fort_yes_limit_from_nation | 0 | 
| 0x0230 | can_build_in_province_fort_no_limit_this_nation | 0 | 
| 0x0231 | can_build_in_province_fort_yes_limit_this_nation | 0 | 
| 0x0232 | can_build_in_province_naval_base_no_limit_from_nation | 0 | 
| 0x0233 | can_build_in_province_naval_base_yes_limit_from_nation | 0 | 
| 0x0234 | can_build_in_province_naval_base_no_limit_this_nation | 0 | 
| 0x0235 | can_build_in_province_naval_base_yes_limit_this_nation | 0 | 
| 0x0236 | can_build_railway_in_capital_yes_whole_state_yes_limit | 0 | 
| 0x0237 | can_build_railway_in_capital_yes_whole_state_no_limit | 0 | 
| 0x0238 | can_build_railway_in_capital_no_whole_state_yes_limit | 0 | 
| 0x0239 | can_build_railway_in_capital_no_whole_state_no_limit | 0 | 
| 0x023A | can_build_fort_in_capital_yes_whole_state_yes_limit | 0 | 
| 0x023B | can_build_fort_in_capital_yes_whole_state_no_limit | 0 | 
| 0x023C | can_build_fort_in_capital_no_whole_state_yes_limit | 0 | 
| 0x023D | can_build_fort_in_capital_no_whole_state_no_limit | 0 | 
| 0x023E | work_available_nation | 1 | 
| 0x023F | work_available_state | 1 | 
| 0x0240 | work_available_province | 1 | 
| 0x0242 | variable_ideology_name_nation | 3 | 
| 0x0243 | variable_ideology_name_state | 3 | 
| 0x0244 | variable_ideology_name_province | 3 | 
| 0x0245 | variable_ideology_name_pop | 3 | 
| 0x0246 | variable_issue_name_nation | 3 | 
| 0x0247 | variable_issue_name_state | 3 | 
| 0x0248 | variable_issue_name_province | 3 | 
| 0x0249 | variable_issue_name_pop | 3 | 
| 0x024A | variable_issue_group_name_nation | 2 | 
| 0x024B | variable_issue_group_name_state | 2 | 
| 0x024C | variable_issue_group_name_province | 2 | 
| 0x024D | variable_issue_group_name_pop | 2 | 
| 0x024E | variable_pop_type_name_nation | 3 | 
| 0x024F | variable_pop_type_name_state | 3 | 
| 0x0250 | variable_pop_type_name_province | 3 | 
| 0x0251 | variable_pop_type_name_pop | 3 | 
| 0x0252 | variable_good_name | 3 | 
| 0x0253 | strata_middle | 0 | 
| 0x0254 | strata_poor | 0 | 
| 0x0254 | party_loyalty_from_province_scope_province | 2 | 
| 0x0255 | can_build_factory_nation | 0 | 
| 0x0256 | can_build_factory_province | 0 | 
| 0x0257 | nationalvalue_pop | 1 | 
| 0x0258 | nationalvalue_province | 1 | 
| 0x0259 | war_exhaustion_pop | 2 | 
| 0x025A | has_culture_core_province_this_pop | 0 | 
| 0x025B | tag_pop | 1 | 
| 0x025C | has_country_flag_pop | 1 | 
| 0x025D | has_country_flag_province | 1 | 
| 0x025E | has_country_modifier_province | 1 | 
| 0x025F | religion_nation | 1 | 
| 0x0260 | religion_nation_reb | 0 | 
| 0x0261 | religion_nation_from_nation | 0 | 
| 0x0262 | religion_nation_this_nation | 0 | 
| 0x0263 | religion_nation_this_state | 0 | 
| 0x0264 | religion_nation_this_province | 0 | 
| 0x0265 | religion_nation_this_pop | 0 | 
| 0x0266 | war_exhaustion_province | 2 | 
| 0x0267 | is_greater_power_province | 0 | 
| 0x0268 | is_cultural_union_pop_this_pop | 0 | 
| 0x0269 | has_building_factory | 0 | 
| 0x026A | has_building_state_from_province | 1 | 
| 0x026B | has_building_factory_from_province | 0 | 
| 0x026C | party_loyalty_generic | 2 | 
| 0x026D | invention | 1 | Primary `dcon::nation_id` has invention #1 (`dcon::invention_id`) |
| 0x026E | political_movement_from_reb | 0 | 
| 0x026F | social_movement_from_reb | 0 | 
| 0x0270 | is_next_rreform_nation | 1 | 
| 0x0271 | is_next_rreform_pop | 1 | 
| 0x0272 | variable_reform_group_name_nation | 2 | 
| 0x0273 | variable_reform_group_name_state | 2 | 
| 0x0274 | variable_reform_group_name_province | 2 | 
| 0x0275 | variable_reform_group_name_pop | 2 | 

### Optimizations

Alice performs various optimizations and simplifications upon the trigger code right after it's parsed:

- For scopes:
    - If the scope is empty (has nothing to evaluate)
        - Then, Eliminate whole scope.
        - Otherwise, for each member in the scope:
            - Run same optimization/simplification algorithm
    - If a `NOT` scope:
        - Invert everything inside the NOT scope (to eliminate an entire layer of the scope).
    - If scope has only 1 member and it's a generic scope:
        - Eliminate the scope and only leave 1 member
    - Otherwise, if it's not a generic scope (AND/OR)
        - Rearrange inner scope (if any) to take the place of the outer AND/OR scope
    - If it's a generic scope (AND/OR)
        - Fold it (rearrange inner AND/OR into the outer one)

See `trigger::simplify_trigger` for more information.

## Effects

### What's an effect?

Effects describe what "effects" will occur upon the game state - those do modify the game state and allows to perform various different actions. Unlike triggers, they are not conditional and will execute as-is, however, they can be "limited" by embedding triggers alongside their scopes. Those who are familiar with modding may recognize the usefulness of `limit`. This allows conditionals to still act upon effects even after "the main trigger" is triggered.

### List of effects

| Code | Name | Number of arguments | Notes/Arguments |
|---|---|---|---|
| 0x0000 | none | 0 | Use for indicating no effect |
| 0x0001 | capital | 1 | #1 is capital to move to |
| 0x0002 | add_core_tag | 1 | Adds all of the #1 argument (`dcon::nation_id`) as cores |
| 0x0003 | add_core_int | 1 | ? |
| 0x0004 | add_core_this_nation | 0 | Adds all of `THIS` (`dcon::nation_id`) as cores |
| 0x0005 | add_core_this_province | 0 | Adds all of `THIS` (`dcon::province_id`) as cores |
| 0x0006 | add_core_this_state | 0 | Adds all of `THIS` (`dcon::state_instance_id`) as cores |
| 0x0007 | add_core_this_pop | 0 | Adds `THIS` (`dcon::pop_id` - location of) as cores |
| 0x0008 | add_core_from_province | 0 | Adds `FROM` `dcon::province_id` as cores |
| 0x0009 | add_core_from_nation | 0 | Adds all of `FROM` `dcon::nation_id` as cores |
| 0x000A | add_core_reb | 0 | 
| 0x000B | remove_core_tag | 1 | 
| 0x000C | remove_core_int | 1 | 
| 0x000D | remove_core_this_nation | 0 | 
| 0x000E | remove_core_this_province | 0 | 
| 0x000F | remove_core_this_state | 0 | 
| 0x0010 | remove_core_this_pop | 0 | 
| 0x0011 | remove_core_from_province | 0 | 
| 0x0012 | remove_core_from_nation | 0 | 
| 0x0013 | remove_core_reb | 0 | 
| 0x0014 | change_region_name_state | 1 | Sets name of the `dcon::state_instance_id` to #1 (`dcon::text_key_id`) |
| 0x0015 | change_region_name_province | 1 | Sets name of the `dcon::province_id` to #1 (`dcon::text_key_id`) |
| 0x0016 | trade_goods | 1 | Sets the RGO of the `dcon::province_id` to #1 (`dcon::commodity_id`) |
| 0x0017 | add_accepted_culture | 1 | 
| 0x0018 | add_accepted_culture_union | 0 | 
| 0x0019 | primary_culture | 1 | Sets primary culture to #1 (`dcon::culture_id`) |
| 0x001A | primary_culture_this_nation | 0 | 
| 0x001B | primary_culture_this_state | 0 | 
| 0x001C | primary_culture_this_province | 0 | 
| 0x001D | primary_culture_this_pop | 0 | 
| 0x001E | primary_culture_from_nation | 0 | 
| 0x001F | remove_accepted_culture | 1 | 
| 0x0020 | life_rating | 1 | 
| 0x0021 | religion | 1 | 
| 0x0022 | is_slave_state_yes | 0 | 
| 0x0023 | is_slave_pop_yes | 0 | 
| 0x0024 | research_points | 1 | 
| 0x0025 | tech_school | 1 | 
| 0x0026 | government | 1 | 
| 0x0027 | government_reb | 0 | 
| 0x0028 | treasury | 2 | 
| 0x0029 | war_exhaustion | 2 | 
| 0x002A | prestige | 2 | 
| 0x002B | change_tag | 1 | 
| 0x002C | change_tag_culture | 0 | 
| 0x002D | change_tag_no_core_switch | 1 | 
| 0x002E | change_tag_no_core_switch_culture | 0 | 
| 0x002F | set_country_flag | 1 | 
| 0x0030 | clr_country_flag | 1 | 
| 0x0031 | military_access | 1 | 
| 0x0032 | military_access_this_nation | 0 | 
| 0x0033 | military_access_this_province | 0 | 
| 0x0034 | military_access_from_nation | 0 | 
| 0x0035 | military_access_from_province | 0 | 
| 0x0036 | badboy | 2 | 
| 0x0037 | secede_province | 1 | 
| 0x0038 | secede_province_this_nation | 0 | 
| 0x0039 | secede_province_this_state | 0 | 
| 0x003A | secede_province_this_province | 0 | 
| 0x003B | secede_province_this_pop | 0 | 
| 0x003C | secede_province_from_nation | 0 | 
| 0x003D | secede_province_from_province | 0 | 
| 0x003E | secede_province_reb | 0 | 
| 0x003F | inherit | 1 | 
| 0x0040 | inherit_this_nation | 0 | 
| 0x0041 | inherit_this_state | 0 | 
| 0x0042 | inherit_this_province | 0 | 
| 0x0043 | inherit_this_pop | 0 | 
| 0x0044 | inherit_from_nation | 0 | 
| 0x0045 | inherit_from_province | 0 | 
| 0x0046 | annex_to | 1 | 
| 0x0047 | annex_to_this_nation | 0 | 
| 0x0048 | annex_to_this_state | 0 | 
| 0x0049 | annex_to_this_province | 0 | 
| 0x004A | annex_to_this_pop | 0 | 
| 0x004B | annex_to_from_nation | 0 | 
| 0x004C | annex_to_from_province | 0 | 
| 0x004D | release | 1 | 
| 0x004E | release_this_nation | 0 | 
| 0x004F | release_this_state | 0 | 
| 0x0050 | release_this_province | 0 | 
| 0x0051 | release_this_pop | 0 | 
| 0x0052 | release_from_nation | 0 | 
| 0x0053 | release_from_province | 0 | 
| 0x0054 | change_controller | 1 | 
| 0x0055 | change_controller_this_nation | 0 | 
| 0x0056 | change_controller_this_province | 0 | 
| 0x0057 | change_controller_from_nation | 0 | 
| 0x0058 | change_controller_from_province | 0 | 
| 0x0059 | infrastructure | 1 | 
| 0x005A | money | 2 | 
| 0x005B | leadership | 1 | 
| 0x005C | create_vassal | 1 | 
| 0x005D | create_vassal_this_nation | 0 | 
| 0x005E | create_vassal_this_province | 0 | 
| 0x005F | create_vassal_from_nation | 0 | 
| 0x0060 | create_vassal_from_province | 0 | 
| 0x0061 | end_military_access | 1 | 
| 0x0062 | end_military_access_this_nation | 0 | 
| 0x0063 | end_military_access_this_province | 0 | 
| 0x0064 | end_military_access_from_nation | 0 | 
| 0x0065 | end_military_access_from_province | 0 | 
| 0x0066 | leave_alliance | 1 | 
| 0x0067 | leave_alliance_this_nation | 0 | 
| 0x0068 | leave_alliance_this_province | 0 | 
| 0x0069 | leave_alliance_from_nation | 0 | 
| 0x006A | leave_alliance_from_province | 0 | 
| 0x006B | end_war | 1 | 
| 0x006C | end_war_this_nation | 0 | 
| 0x006D | end_war_this_province | 0 | 
| 0x006E | end_war_from_nation | 0 | 
| 0x006F | end_war_from_province | 0 | 
| 0x0070 | enable_ideology | 1 | 
| 0x0071 | ruling_party_ideology | 1 | 
| 0x0072 | plurality | 2 | 
| 0x0073 | remove_province_modifier | 1 | 
| 0x0074 | remove_country_modifier | 1 | 
| 0x0075 | create_alliance | 1 | 
| 0x0076 | create_alliance_this_nation | 0 | 
| 0x0077 | create_alliance_this_province | 0 | 
| 0x0078 | create_alliance_from_nation | 0 | 
| 0x0079 | create_alliance_from_province | 0 | 
| 0x007A | release_vassal | 1 | 
| 0x007B | release_vassal_this_nation | 0 | 
| 0x007C | release_vassal_this_province | 0 | 
| 0x007D | release_vassal_from_nation | 0 | 
| 0x007E | release_vassal_from_province | 0 | 
| 0x007F | release_vassal_reb | 0 | 
| 0x0080 | release_vassal_random | 0 | 
| 0x0081 | change_province_name | 1 | 
| 0x0082 | enable_canal | 1 | 
| 0x0083 | set_global_flag | 1 | 
| 0x0084 | clr_global_flag | 1 | 
| 0x0085 | nationalvalue_province | 1 | 
| 0x0086 | nationalvalue_nation | 1 | 
| 0x0087 | civilized_yes | 0 | 
| 0x0088 | civilized_no | 0 | 
| 0x0089 | is_slave_state_no | 0 | 
| 0x008A | is_slave_pop_no | 0 | 
| 0x008B | election | 0 | 
| 0x008C | social_reform | 1 | 
| 0x008D | political_reform | 1 | 
| 0x008E | add_tax_relative_income | 2 | 
| 0x008F | neutrality | 0 | 
| 0x0090 | reduce_pop | 2 | 
| 0x0091 | move_pop | 1 | 
| 0x0092 | pop_type | 1 | 
| 0x0093 | years_of_research | 2 | 
| 0x0094 | prestige_factor_positive | 2 | 
| 0x0095 | prestige_factor_negative | 2 | 
| 0x0096 | military_reform | 1 | 
| 0x0097 | economic_reform | 1 | 
| 0x0098 | remove_random_military_reforms | 1 | 
| 0x0099 | remove_random_economic_reforms | 1 | 
| 0x009A | add_crime | 1 | 
| 0x009B | add_crime_none | 0 | 
| 0x009C | nationalize | 0 | 
| 0x009D | build_factory_in_capital_state | 1 | 
| 0x009E | activate_technology | 1 | 
| 0x009F | great_wars_enabled_yes | 0 | 
| 0x00A0 | great_wars_enabled_no | 0 | 
| 0x00A1 | world_wars_enabled_yes | 0 | 
| 0x00A2 | world_wars_enabled_no | 0 | 
| 0x00A3 | assimilate_province | 0 | 
| 0x00A4 | assimilate_pop | 0 | 
| 0x00A5 | literacy | 2 | 
| 0x00A6 | add_crisis_interest | 0 | 
| 0x00A7 | flashpoint_tension | 2 | 
| 0x00A8 | add_crisis_temperature | 2 | 
| 0x00A9 | consciousness | 2 | 
| 0x00AA | militancy | 2 | 
| 0x00AB | rgo_size | 1 | 
| 0x00AC | fort | 1 | 
| 0x00AD | naval_base | 1 | 
| 0x00AE | trigger_revolt_nation | 4 | 
| 0x00AF | trigger_revolt_state | 4 | 
| 0x00B0 | trigger_revolt_province | 4 | 
| 0x00B1 | diplomatic_influence | 2 | 
| 0x00B2 | diplomatic_influence_this_nation | 1 | 
| 0x00B3 | diplomatic_influence_this_province | 1 | 
| 0x00B4 | diplomatic_influence_from_nation | 1 | 
| 0x00B5 | diplomatic_influence_from_province | 1 | 
| 0x00B6 | relation | 2 | 
| 0x00B7 | relation_this_nation | 1 | 
| 0x00B8 | relation_this_province | 1 | 
| 0x00B9 | relation_from_nation | 1 | 
| 0x00BA | relation_from_province | 1 | 
| 0x00BB | add_province_modifier | 2 | 
| 0x00BC | add_province_modifier_no_duration | 1 | 
| 0x00BD | add_country_modifier | 2 | 
| 0x00BE | add_country_modifier_no_duration | 1 | 
| 0x00BF | casus_belli_tag | 3 | 
| 0x00C0 | casus_belli_int | 3 | 
| 0x00C1 | casus_belli_this_nation | 2 | 
| 0x00C2 | casus_belli_this_state | 2 | 
| 0x00C3 | casus_belli_this_province | 2 | 
| 0x00C4 | casus_belli_this_pop | 2 | 
| 0x00C5 | casus_belli_from_nation | 2 | 
| 0x00C6 | casus_belli_from_province | 2 | 
| 0x00C7 | add_casus_belli_tag | 3 | 
| 0x00C8 | add_casus_belli_int | 3 | 
| 0x00C9 | add_casus_belli_this_nation | 2 | 
| 0x00CA | add_casus_belli_this_state | 2 | 
| 0x00CB | add_casus_belli_this_province | 2 | 
| 0x00CC | add_casus_belli_this_pop | 2 | 
| 0x00CD | add_casus_belli_from_nation | 2 | 
| 0x00CE | add_casus_belli_from_province | 2 | 
| 0x00CF | remove_casus_belli_tag | 2 | 
| 0x00D0 | remove_casus_belli_int | 2 | 
| 0x00D1 | remove_casus_belli_this_nation | 1 | 
| 0x00D2 | remove_casus_belli_this_state | 1 | 
| 0x00D3 | remove_casus_belli_this_province | 1 | 
| 0x00D4 | remove_casus_belli_this_pop | 1 | 
| 0x00D5 | remove_casus_belli_from_nation | 1 | 
| 0x00D6 | remove_casus_belli_from_province | 1 | 
| 0x00D7 | this_remove_casus_belli_tag | 2 | 
| 0x00D8 | this_remove_casus_belli_int | 2 | 
| 0x00D9 | this_remove_casus_belli_this_nation | 1 | 
| 0x00DA | this_remove_casus_belli_this_state | 1 | 
| 0x00DB | this_remove_casus_belli_this_province | 1 | 
| 0x00DC | this_remove_casus_belli_this_pop | 1 | 
| 0x00DD | this_remove_casus_belli_from_nation | 1 | 
| 0x00DE | this_remove_casus_belli_from_province | 1 | 
| 0x00DF | war_tag | 7 | 
| 0x00E0 | war_this_nation | 6 | 
| 0x00E1 | war_this_state | 6 | 
| 0x00E2 | war_this_province | 6 | 
| 0x00E3 | war_this_pop | 6 | 
| 0x00E4 | war_from_nation | 6 | 
| 0x00E5 | war_from_province | 6 | 
| 0x00E6 | war_no_ally_tag | 7 | 
| 0x00E7 | war_no_ally_this_nation | 6 | 
| 0x00E8 | war_no_ally_this_state | 6 | 
| 0x00E9 | war_no_ally_this_province | 6 | 
| 0x00EA | war_no_ally_this_pop | 6 | 
| 0x00EB | war_no_ally_from_nation | 6 | 
| 0x00EC | war_no_ally_from_province | 6 | 
| 0x00ED | country_event_this_nation | 2 | 
| 0x00EE | country_event_immediate_this_nation | 1 | 
| 0x00EF | province_event_this_nation | 2 | 
| 0x00F0 | province_event_immediate_this_nation | 1 | 
| 0x00F1 | sub_unit_int | 2 | 
| 0x00F2 | sub_unit_this | 1 | 
| 0x00F3 | sub_unit_from | 1 | 
| 0x00F4 | sub_unit_current | 1 | 
| 0x00F5 | set_variable | 3 | 
| 0x00F6 | change_variable | 3 | 
| 0x00F7 | ideology | 3 | 
| 0x00F8 | upper_house | 3 | 
| 0x00F9 | scaled_militancy_issue | 3 | 
| 0x00FA | scaled_militancy_ideology | 3 | 
| 0x00FB | scaled_militancy_unemployment | 2 | 
| 0x00FC | scaled_consciousness_issue | 3 | 
| 0x00FD | scaled_consciousness_ideology | 3 | 
| 0x00FE | scaled_consciousness_unemployment | 2 | 
| 0x00FF | define_general | 3 | 
| 0x0100 | define_admiral | 3 | 
| 0x0101 | dominant_issue | 3 | 
| 0x0102 | add_war_goal | 1 | 
| 0x0103 | move_issue_percentage_nation | 4 | 
| 0x0104 | move_issue_percentage_state | 4 | 
| 0x0105 | move_issue_percentage_province | 4 | 
| 0x0106 | move_issue_percentage_pop | 4 | 
| 0x0107 | party_loyalty | 3 | 
| 0x0108 | party_loyalty_province | 2 | 
| 0x0109 | variable_tech_name_no | 1 | 
| 0x010A | variable_invention_name_yes | 1 | 
| 0x010B | build_railway_in_capital_yes_whole_state_yes_limit | 0 | 
| 0x010C | build_railway_in_capital_yes_whole_state_no_limit | 0 | 
| 0x010D | build_railway_in_capital_no_whole_state_yes_limit | 0 | 
| 0x010E | build_railway_in_capital_no_whole_state_no_limit | 0 | 
| 0x010F | build_fort_in_capital_yes_whole_state_yes_limit | 0 | 
| 0x0110 | build_fort_in_capital_yes_whole_state_no_limit | 0 | 
| 0x0111 | build_fort_in_capital_no_whole_state_yes_limit | 0 | 
| 0x0112 | build_fort_in_capital_no_whole_state_no_limit | 0 | 
| 0x0113 | relation_reb | 1 | 
| 0x0114 | variable_tech_name_yes | 1 | 
| 0x0115 | variable_good_name | 3 | 
| 0x0116 | set_country_flag_province | 1 | 
| 0x0117 | add_country_modifier_province | 2 | 
| 0x0118 | add_country_modifier_province_no_duration | 1 | 
| 0x0119 | dominant_issue_nation | 3 | 
| 0x011A | relation_province | 2 | 
| 0x011B | relation_province_this_nation | 1 | 
| 0x011C | relation_province_this_province | 1 | 
| 0x011D | relation_province_from_nation | 1 | 
| 0x011E | relation_province_from_province | 1 | 
| 0x011F | relation_province_reb | 1 | 
| 0x0120 | scaled_militancy_nation_issue | 3 | 
| 0x0121 | scaled_militancy_nation_ideology | 3 | 
| 0x0122 | scaled_militancy_nation_unemployment | 2 | 
| 0x0123 | scaled_consciousness_nation_issue | 3 | 
| 0x0124 | scaled_consciousness_nation_ideology | 3 | 
| 0x0125 | scaled_consciousness_nation_unemployment | 2 | 
| 0x0126 | scaled_militancy_nation_issue | 3 | 
| 0x0127 | scaled_militancy_nation_ideology | 3 | 
| 0x0128 | scaled_militancy_nation_unemployment | 2 | 
| 0x0129 | scaled_consciousness_nation_issue | 3 | 
| 0x012A | scaled_consciousness_nation_ideology | 3 | 
| 0x012B | scaled_consciousness_nation_unemployment | 2 | 
| 0x012C | scaled_militancy_nation_issue | 3 | 
| 0x012D | scaled_militancy_nation_ideology | 3 | 
| 0x012E | scaled_militancy_nation_unemployment | 2 | 
| 0x012F | scaled_consciousness_nation_issue | 3 | 
| 0x0130 | scaled_consciousness_nation_ideology | 3 | 
| 0x0131 | scaled_consciousness_nation_unemployment | 2 | 
| 0x0132 | variable_good_name_province | 3 | 
| 0x0133 | treasury_province | 2 | 
| 0x0134 | country_event_this_state | 2 | 
| 0x0135 | country_event_immediate_this_state | 1 | 
| 0x0136 | province_event_this_state | 2 | 
| 0x0137 | province_event_immediate_this_state | 1 | 
| 0x0138 | country_event_this_province | 2 | 
| 0x0139 | country_event_immediate_this_province | 1 | 
| 0x013A | province_event_this_province | 2 | 
| 0x013B | province_event_immediate_this_province | 1 | 
| 0x013C | country_event_this_pop | 2 | 
| 0x013D | country_event_immediate_this_pop | 1 | 
| 0x013E | province_event_this_pop | 2 | 
| 0x013F | province_event_immediate_this_pop | 1 | 
| 0x0140 | country_event_province_this_nation | 2 | 
| 0x0141 | country_event_immediate_province_this_nation | 1 | 
| 0x0142 | country_event_province_this_state | 2 | 
| 0x0143 | country_event_immediate_province_this_state | 1 | 
| 0x0144 | country_event_province_this_province | 2 | 
| 0x0145 | country_event_immediate_province_this_province | 1 | 
| 0x0146 | country_event_province_this_pop | 2 | 
| 0x0147 | country_event_immediate_province_this_pop | 1 | 
| 0x0148 | activate_invention | 1 | 
| 0x0149 | variable_invention_name_no | 1 | 
| 0x0014A | add_core_tag_state | 1 | 
| 0x0014B | remove_core_tag_state | 1 | 
| 0x014C | secede_province_state | 1 | 
| 0x014D | assimilate_state | 0 | 
