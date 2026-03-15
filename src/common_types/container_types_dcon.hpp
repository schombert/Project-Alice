#pragma once
#include "dcon_generated_ids.hpp"
#include "date_interface.hpp"
#include "glm/glm.hpp"
#include "commands_containers.hpp"
#ifdef _WIN64 // WINDOWS
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#ifndef WINSOCK2_IMPORTED
#define WINSOCK2_IMPORTED
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#else // NIX
#include <netinet/in.h>
#include <sys/socket.h>
#endif


// THIS FILE IS SUPPOSED TO STORE ONLY CONTAINERS USED IN DCON DIRECTLY


namespace sys {

struct gamerule_option {
	dcon::text_key name;

	dcon::text_key on_select_lua_function;
	dcon::text_key on_deselect_lua_function;


	bool operator==(const gamerule_option& other) const = default;
	bool operator!=(const gamerule_option& other) const = default;
};

struct value_modifier_segment {
	float factor = 0.0f;
	dcon::trigger_key condition;
	uint16_t padding = 0;
};
static_assert(sizeof(value_modifier_segment) ==
	sizeof(value_modifier_segment::factor)
	+ sizeof(value_modifier_segment::condition)
	+ sizeof(value_modifier_segment::padding));

struct value_modifier_description {
	float factor = 0.0f;
	float base = 0.0f;
	uint16_t first_segment_offset = 0;
	uint16_t segments_count = 0;
};
static_assert(sizeof(value_modifier_description) ==
	sizeof(value_modifier_description::factor)
	+ sizeof(value_modifier_description::base)
	+ sizeof(value_modifier_description::first_segment_offset)
	+ sizeof(value_modifier_description::segments_count));

struct event_option {
	dcon::text_key name;
	dcon::value_modifier_key ai_chance;
	dcon::effect_key effect;

	bool operator==(const event_option& other) const = default;
	bool operator!=(const event_option& other) const = default;
};

static_assert(sizeof(event_option) ==
	sizeof(event_option::name)
	+ sizeof(event_option::ai_chance)
	+ sizeof(event_option::effect));


struct commodity_modifier {
	float amount = 0.0f;
	dcon::commodity_id type;
	uint8_t padding[3] = { 0 };
	bool operator==(const commodity_modifier& other) const {
		return other.type == type && other.amount == amount;
	}
	bool operator!=(const commodity_modifier& other) const {
		return !(other == *this);
	}

};
static_assert(sizeof(commodity_modifier) ==
	sizeof(commodity_modifier::amount)
	+ sizeof(commodity_modifier::type)
	+ sizeof(commodity_modifier::padding));

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
	float maneuver = 0.0f;

	void operator+=(unit_variable_stats const& other) {
		build_time += other.build_time;
		default_organisation += other.default_organisation;
		maximum_speed += other.maximum_speed;
		defence_or_hull += other.defence_or_hull;
		attack_or_gun_power += other.attack_or_gun_power;
		supply_consumption += other.supply_consumption;
		support += other.support;
		maneuver += other.maneuver;
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
		maneuver -= other.maneuver;
		siege_or_torpedo_attack -= other.siege_or_torpedo_attack;
		reconnaissance_or_fire_range -= other.reconnaissance_or_fire_range;
		discipline_or_evasion -= other.discipline_or_evasion;
	}
	bool operator==(const unit_variable_stats& other) const = default;
	bool operator!=(const unit_variable_stats& other) const = default;
};
static_assert(sizeof(unit_variable_stats) ==
	sizeof(unit_variable_stats::build_time)
	+ sizeof(unit_variable_stats::default_organisation)
	+ sizeof(unit_variable_stats::maximum_speed)
	+ sizeof(unit_variable_stats::defence_or_hull)
	+ sizeof(unit_variable_stats::attack_or_gun_power)
	+ sizeof(unit_variable_stats::supply_consumption)
	+ sizeof(unit_variable_stats::support)
	+ sizeof(unit_variable_stats::maneuver)
	+ sizeof(unit_variable_stats::siege_or_torpedo_attack)
	+ sizeof(unit_variable_stats::reconnaissance_or_fire_range)
	+ sizeof(unit_variable_stats::discipline_or_evasion));

struct provincial_modifier_definition {
	static constexpr uint32_t modifier_definition_size = 22;

	float values[modifier_definition_size] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	dcon::provincial_modifier_value offsets[modifier_definition_size] = { dcon::provincial_modifier_value{} };
	uint16_t padding = 0;

	bool operator==(const provincial_modifier_definition& other) const {
		return std::memcmp(this->values, other.values, sizeof(values)) == 0 && std::memcmp(this->offsets, other.offsets, sizeof(offsets)) == 0;
	}
	bool operator!=(const provincial_modifier_definition& other) const {
		return !(other == *this);
	}
};
static_assert(sizeof(provincial_modifier_definition) ==
	sizeof(provincial_modifier_definition::values)
	+ sizeof(provincial_modifier_definition::offsets)
	+ sizeof(provincial_modifier_definition::padding));


struct national_modifier_definition {
	static constexpr uint32_t modifier_definition_size = 22;

	float values[modifier_definition_size] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	dcon::national_modifier_value offsets[modifier_definition_size] = { dcon::national_modifier_value{} };
	uint16_t padding = 0;

	bool operator==(const national_modifier_definition& other) const {
		return std::memcmp(this->values, other.values, sizeof(values)) == 0 && std::memcmp(this->offsets, other.offsets, sizeof(offsets)) == 0;
	}
	bool operator!=(const national_modifier_definition& other) const {
		return !(other == *this);
	}
};
static_assert(sizeof(national_modifier_definition) ==
	sizeof(national_modifier_definition::values)
	+ sizeof(national_modifier_definition::offsets)
	+ sizeof(national_modifier_definition::padding));

struct unit_modifier : public unit_variable_stats {
	dcon::unit_type_id type;
	uint8_t padding[3] = { 0 };
	bool operator==(const unit_modifier& other) const {
		return dynamic_cast<const unit_variable_stats&>(other) == dynamic_cast<const unit_variable_stats&>(*this) && other.type == type;
	}
	bool operator!=(const unit_modifier& other) const {
		return !(other == *this);
	}
};
static_assert(sizeof(unit_modifier) ==
	sizeof(unit_variable_stats)
	+ sizeof(unit_modifier::type)
	+ sizeof(unit_modifier::padding));

struct rebel_org_modifier {
	float amount = 0.0f; //4
	dcon::rebel_type_id type; //1 - no type set = all rebels
	uint8_t padding[2] = { 0, 0 };
	bool operator==(const rebel_org_modifier& other) const {
		return other.amount == amount && other.type == type;
	}
	bool operator!=(const rebel_org_modifier& other) const {
		return !(other == *this);
	}
};
static_assert(sizeof(rebel_org_modifier) ==
	sizeof(rebel_org_modifier::amount)
	+ sizeof(rebel_org_modifier::type)
	+ sizeof(rebel_org_modifier::padding));

struct dated_modifier {
	sys::date expiration;
	dcon::modifier_id mod_id;
	bool operator==(const dated_modifier& other) const = default;
	bool operator!=(const dated_modifier& other) const = default;
};
static_assert(sizeof(dated_modifier) ==
	sizeof(dated_modifier::expiration)
	+ sizeof(dated_modifier::mod_id));





template<size_t _Size>
struct player_value {
	std::array<uint8_t, _Size> data = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	std::string_view to_string_view() const noexcept {
		for(uint32_t i = 0; i < sizeof(data); i++) {
			if(data[i] == ' ' || data[i] == '\0') {
				return std::string_view{ reinterpret_cast<const char*>(&data[0]), uint32_t(i) };
			}
		}
		return std::string_view{ reinterpret_cast<const char*>(&data[0]), sizeof(data) };
	}

	player_value<_Size> from_string_view(std::string_view sv) noexcept {
		size_t length_to_copy = std::min(sv.size(), data.size());
		sv.copy(reinterpret_cast<char*>(data.data()), length_to_copy);
		return *this;
	}

	bool is_equal(const player_value<_Size>& other) const {
		return other.data == data;
	}

	bool operator==(const player_value<_Size>& other) const {
		return is_equal(other);
	}
	bool operator!=(const player_value<_Size>& other) const {
		return !(*this == other);
	}

	std::string to_string() const noexcept {
		return std::string(to_string_view());
	}

	bool empty() noexcept {
		return data[0] == ' ' || data[0] == '\0';
	}

	void append(char c) noexcept {
		for(uint32_t i = 0; i < sizeof(data); i++) {
			if(data[i] == ' ' || data[i] == '\0') {
				data[i] = c;
				return;
			}
		}
	}

	char pop() noexcept {
		for(uint32_t i = 1; i < sizeof(data); i++) {
			if(data[i] == ' ' || data[i] == '\0') {
				auto pop = data[i - 1];
				data[i - 1] = ' ';
				return pop;
			}
		}
		return ' ';
	}
};

using player_name = player_value<24>;
using player_password_salt = player_value<24>;
using player_password_hash = player_value<64>;
using player_password_raw = player_value<24>;

static_assert(sizeof(player_name) == sizeof(player_name::data));
static_assert(sizeof(player_password_salt) == sizeof(player_password_salt::data));
static_assert(sizeof(player_password_hash) == sizeof(player_password_hash::data));
static_assert(sizeof(player_password_raw) == sizeof(player_password_raw::data));





}



namespace economy {

struct commodity_set {
	static constexpr uint32_t set_size = 8;

	float commodity_amounts[set_size] = { 0.0f };
	dcon::commodity_id commodity_type[set_size] = { dcon::commodity_id{} };

	bool operator==(const commodity_set& other) const {
		return std::memcmp(this->commodity_amounts, other.commodity_amounts, sizeof(commodity_amounts)) == 0 && std::memcmp(this->commodity_type, other.commodity_type, sizeof(commodity_type)) == 0;
	}
	bool operator!=(const commodity_set& other) const {
		return !(other == *this);
	}

};
static_assert(sizeof(commodity_set) ==
	sizeof(commodity_set::commodity_amounts)
	+ sizeof(commodity_set::commodity_type));

struct small_commodity_set {
	static constexpr uint32_t set_size = 6;

	float commodity_amounts[set_size] = { 0.0f };
	dcon::commodity_id commodity_type[set_size] = { dcon::commodity_id{} };
	uint16_t padding = 0;

	bool operator==(const small_commodity_set& other) const {
		return std::memcmp(this->commodity_amounts, other.commodity_amounts, sizeof(commodity_amounts)) == 0 && std::memcmp(this->commodity_type, other.commodity_type, sizeof(commodity_type)) == 0;
	}
	bool operator!=(const small_commodity_set& other) const {
		return !(other == *this);
	}



};
static_assert(sizeof(small_commodity_set) ==
	sizeof(small_commodity_set::commodity_amounts)
	+ sizeof(small_commodity_set::commodity_type)
	+ sizeof(small_commodity_set::padding));

} // namespace economy


namespace military {


struct ship_in_battle {
	static constexpr uint16_t distance_mask = 0x03FF;

	static constexpr uint16_t mode_mask = 0x1C00;
	static constexpr uint16_t mode_seeking = 0x0400;
	static constexpr uint16_t mode_approaching = 0x0800;
	static constexpr uint16_t mode_engaged = 0x0C00;
	static constexpr uint16_t mode_retreating = 0x1000;
	static constexpr uint16_t mode_retreated = 0x1400;
	static constexpr uint16_t mode_sunk = 0x0000;

	static constexpr uint16_t is_attacking = 0x2000;

	static constexpr uint16_t type_mask = 0xC000;
	static constexpr uint16_t type_big = 0x4000;
	static constexpr uint16_t type_small = 0x8000;
	static constexpr uint16_t type_transport = 0x0000;

	dcon::ship_id ship;
	int16_t target_slot = -1;
	uint16_t flags = 0;
	uint16_t ships_targeting_this = 0;
	bool operator == (const ship_in_battle&) const = default;
	bool operator != (const ship_in_battle&) const = default;

	uint16_t get_distance() const {
		return flags & distance_mask;
	}
	void set_distance(uint16_t distance) {
		flags &= ~distance_mask;
		flags |= distance_mask & (distance);

	}
};
static_assert(sizeof(ship_in_battle) ==
	sizeof(ship_in_battle::ship)
	+ sizeof(ship_in_battle::ships_targeting_this)
	+ sizeof(ship_in_battle::target_slot)
	+ sizeof(ship_in_battle::flags));

struct mobilization_order {
	sys::date when; //2
	dcon::province_id where; //2
	bool operator==(const mobilization_order& other) const = default;
	bool operator!=(const mobilization_order& other) const = default;
};
static_assert(sizeof(mobilization_order) ==
	sizeof(mobilization_order::where)
	+ sizeof(mobilization_order::when));

struct reserve_regiment {
	static constexpr uint16_t is_attacking = 0x0001;

	static constexpr uint16_t type_mask = 0x0006;
	static constexpr uint16_t type_infantry = 0x0000;
	static constexpr uint16_t type_cavalry = 0x0002;
	static constexpr uint16_t type_support = 0x0004;

	dcon::regiment_id regiment;
	uint16_t flags = 0;

	bool operator==(const reserve_regiment& other) const = default;
	bool operator!=(const reserve_regiment& other) const = default;
};
static_assert(sizeof(reserve_regiment) ==
	sizeof(reserve_regiment::regiment)
	+ sizeof(reserve_regiment::flags));


struct available_cb {
	sys::date expiration; //2
	dcon::nation_id target; //2
	dcon::cb_type_id cb_type; //2
	dcon::state_definition_id target_state;
	bool operator==(const available_cb& other) const = default;
	bool operator!=(const available_cb& other) const = default;
};
static_assert(sizeof(available_cb) ==
	+sizeof(available_cb::target)
	+ sizeof(available_cb::expiration)
	+ sizeof(available_cb::cb_type) +
	sizeof(available_cb::target_state));

}

namespace network {
struct client_handshake_data {
	sys::player_name nickname;
	sys::player_password_raw player_password;
	uint8_t lobby_password[16] = { 0 };
};

enum class client_state : uint8_t {
	normal = 0,
	flushing = 1,
	shutting_down = 2
};
#ifdef _WIN64
typedef SOCKET socket_t;
#else
typedef int socket_t;
#endif

}
