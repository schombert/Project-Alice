#pragma once

// CONSTANTS WHICH ARE USED IN DCON DIRECTLY

namespace culture {
inline constexpr int32_t max_issue_options = 6;
}

namespace network {
constexpr size_t RECV_BUFFER_SIZE = 262144;
}

namespace sys {

constexpr int32_t max_event_options = 8;
constexpr uint32_t max_gamerule_settings = 15;

enum class army_group_regiment_status : uint8_t {
	move_to_target,
	move_to_port,
	standby,
	await_transport,
	is_transported,
	disembark,
	embark
};

enum class army_group_regiment_task : uint8_t {
	idle,
	gather_at_hq,
	defend_position,
	siege,
};

enum class army_group_order : uint8_t {
	siege,
	defend,
	designate_port,
	none
};
}

namespace economy {
enum class province_building_type : uint8_t {
	railroad, fort, naval_base, bank, university, last, factory, province_selector, province_immigrator
};
constexpr inline int32_t max_building_types = 5;
}
// global namespace

// Boolean primitive which is guarrenteed to be 1 byte, whereas normal bool can theoretically wary
typedef uint8_t fixed_bool_t;
