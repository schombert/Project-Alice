#pragma once

namespace military {

enum special_army_order {
	none,
	move_to_siege,
	strategic_redeployment,
	pursue_to_engage
};

enum class unit_type : uint8_t {
	support, big_ship, cavalry, transport, light_ship, special, infantry
};

enum class crossing_type : uint8_t {
	none, river, sea
};

enum class apply_attrition_on_arrival {
	no, yes
};

enum class battle_is_ending {
	no, yes
};

enum class retreat_type : bool {
	automatic = 0,
	manual = 1,
};

enum class reinforcement_cap : uint8_t {
	capped_at_max_strength,
	uncapped
};
enum class organization_cap : uint8_t {
	capped_at_max_org,
	uncapped
};


constexpr uint8_t min_combat_width = 2;


// The distance from one side of of the naval battle to the middle. Unit speed is cast to this distance with define:NAVAL_COMBAT_SPEED_TO_DISTANCE_FACTOR and naval_battle_speed_mult.
// The "total" distance for both sides is double this number, as each ship will start at 100 distance from the middle (which equals to 200 distance between them)
// the actual integer is 1000 units, which here means 100.0 with one fixed-point decimal.
constexpr uint16_t naval_battle_distance_to_center = 1000;

constexpr uint16_t naval_battle_total_distance = naval_battle_distance_to_center * 2; // total distance from one end of the battle to another

constexpr uint16_t naval_battle_center_line = 0; // The "center line" of a naval battle. Ships on one side cannot go past this.

constexpr uint16_t naval_battle_speed_mult = 1000; // mult for casting unit speed to battle speed


constexpr inline int32_t river_crossing_modifier = -1;
constexpr inline int32_t strait_crossing_modifier = -2;

constexpr inline int32_t days_before_retreat = 11;

enum class battle_result {
	indecisive, attacker_won, defender_won
};
enum class regiment_dmg_source {
	combat, attrition
};

enum class battle_role : uint8_t {
	attacker = 0,
	defender = 1
};

enum class battle_line : uint8_t {
	frontline = 0,
	backline = 1
};

enum class interval_estimation {
	daily, monthly
};
enum class supply_estimation {
	based_on_satisfaction, full_supply_always
};
enum class unit_consumption_type : uint8_t {
	supply = 0,
	reinforcement = 1
};

// These are diffrent enums because only some operations (when dealing with military supply sets) makes sense with the "both" setting. Such as when having an array conisting of both supply and reinforcement goods. Use the below method for converting the "lighter" enum to the heavier one
enum class commodity_consumption_type : uint8_t {
	supply = 0,
	reinforcement = 1,
	both = 2
};

constexpr commodity_consumption_type to_consumption_type(unit_consumption_type in) {
	return commodity_consumption_type(in);
}



}
