#pragma once
#include <stdint.h>
#include <vector>
#include "dcon_generated.hpp"

namespace nations {
uint32_t tag_to_int(char first, char second, char third) {
	return (uint32_t(first) << 16) | (uint32_t(second) << 8) | (uint32_t(third) << 0);
}

struct triggered_modifier {
	dcon::modifier_id linked_modifier;
	dcon::trigger_key trigger_condition;
};

struct fixed_event {
	int16_t chance;
	dcon::national_event_id id;
};
struct fixed_province_event {
	int16_t chance;
	dcon::provincial_event_id id;
};

struct global_national_state {
	std::vector<triggered_modifier> triggered_modifiers;

	dcon::modifier_id very_easy_player;
	dcon::modifier_id easy_player;
	dcon::modifier_id hard_player;
	dcon::modifier_id very_hard_player;
	dcon::modifier_id very_easy_ai;
	dcon::modifier_id easy_ai;
	dcon::modifier_id hard_ai;
	dcon::modifier_id very_hard_ai;

	//provincial
	dcon::modifier_id overseas;
	dcon::modifier_id coastal;
	dcon::modifier_id non_coastal;
	dcon::modifier_id coastal_sea;
	dcon::modifier_id sea_zone;
	dcon::modifier_id land_province;
	dcon::modifier_id blockaded;
	dcon::modifier_id no_adjacent_controlled;
	dcon::modifier_id core;
	dcon::modifier_id has_siege;
	dcon::modifier_id occupied;
	dcon::modifier_id nationalism;
	dcon::modifier_id infrastructure;

	// national
	dcon::modifier_id base_values;
	dcon::modifier_id war;
	dcon::modifier_id peace;
	dcon::modifier_id disarming;
	dcon::modifier_id war_exhaustion;
	dcon::modifier_id badboy;
	dcon::modifier_id debt_default_to;
	dcon::modifier_id bad_debter;
	dcon::modifier_id great_power;
	dcon::modifier_id second_power;
	dcon::modifier_id civ_nation;
	dcon::modifier_id unciv_nation;
	dcon::modifier_id average_literacy;
	dcon::modifier_id plurality;
	dcon::modifier_id generalised_debt_default;
	dcon::modifier_id total_occupation;
	dcon::modifier_id total_blockaded;
	dcon::modifier_id in_bankrupcy;

	int32_t num_allocated_national_variables = 0;
	int32_t num_allocated_national_flags = 0;
	int32_t num_allocated_global_flags = 0;

	dcon::national_focus_id flashpoint_focus;
	float flashpoint_amount = 0.15f;

	std::vector<fixed_event> on_yearly_pulse;
	std::vector<fixed_event> on_quarterly_pulse;
	std::vector<fixed_province_event> on_battle_won;
	std::vector<fixed_province_event> on_battle_lost;
	std::vector<fixed_event> on_surrender;
	std::vector<fixed_event> on_new_great_nation;
	std::vector<fixed_event> on_lost_great_nation;
	std::vector<fixed_event> on_election_tick;
	std::vector<fixed_event> on_colony_to_state;
	std::vector<fixed_event> on_state_conquest;
	std::vector<fixed_event> on_colony_to_state_free_slaves;
	std::vector<fixed_event> on_debtor_default;
	std::vector<fixed_event> on_debtor_default_small;
	std::vector<fixed_event> on_debtor_default_second;
	std::vector<fixed_event> on_civilize;
	std::vector<fixed_event> on_my_factories_nationalized;
	std::vector<fixed_event> on_crisis_declare_interest;
};

namespace influence {

constexpr inline uint8_t level_mask = uint8_t(0x07);
constexpr inline uint8_t level_neutral = uint8_t(0x00);
constexpr inline uint8_t level_opposed = uint8_t(0x01);
constexpr inline uint8_t level_hostile = uint8_t(0x02);
constexpr inline uint8_t level_cordial = uint8_t(0x03);
constexpr inline uint8_t level_friendly = uint8_t(0x04);
constexpr inline uint8_t level_in_sphere = uint8_t(0x05);

constexpr inline uint8_t priority_mask = uint8_t(0x18);
constexpr inline uint8_t priority_zero = uint8_t(0x00);
constexpr inline uint8_t priority_one = uint8_t(0x08);
constexpr inline uint8_t priority_two = uint8_t(0x10);
constexpr inline uint8_t priority_three = uint8_t(0x18);

constexpr inline uint8_t is_expelled = uint8_t(0x20);
constexpr inline uint8_t is_discredited = uint8_t(0x40);
constexpr inline uint8_t is_banned = uint8_t(0x80);

inline uint8_t increase_level(uint8_t v) {
	switch(v & level_mask) {
		case level_neutral:
			return uint8_t((v & ~level_mask) | level_cordial);
		case level_opposed:
			return uint8_t((v & ~level_mask) | level_neutral);
		case level_hostile:
			return uint8_t((v & ~level_mask) | level_opposed);
		case level_cordial:
			return uint8_t((v & ~level_mask) | level_friendly);
		case level_friendly:
			return uint8_t((v & ~level_mask) | level_in_sphere);
		case level_in_sphere:
			return uint8_t((v & ~level_mask) | level_in_sphere);
		default:
			return v;
	}
}
inline uint8_t decrease_level(uint8_t v) {
	switch(v & level_mask) {
		case level_neutral:
			return uint8_t((v & ~level_mask) | level_opposed);
		case level_opposed:
			return uint8_t((v & ~level_mask) | level_hostile);
		case level_hostile:
			return uint8_t((v & ~level_mask) | level_hostile);
		case level_cordial:
			return uint8_t((v & ~level_mask) | level_neutral);
		case level_friendly:
			return uint8_t((v & ~level_mask) | level_cordial);
		case level_in_sphere:
			return uint8_t((v & ~level_mask) | level_friendly);
		default:
			return v;
	}
}
inline uint8_t increase_priority(uint8_t v) {
	if((v & priority_mask) != priority_three) {
		return uint8_t(v + priority_one);
	} else {
		return v;
	}
}
inline uint8_t decrease_priority(uint8_t v) {
	if((v & priority_mask) != priority_zero) {
		return uint8_t(v - priority_one);
	} else {
		return v;
	}
}

}

}

