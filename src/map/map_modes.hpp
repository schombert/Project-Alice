#pragma once

// #include "../gamestate/system_state.hpp"

namespace map_mode {

enum class mode : uint8_t {
	terrain = 0x01,
	political = 0x02,
	revolt = 0x03,
	diplomatic = 0x04,
	region = 0x05,
	infrastructure = 0x06,
	colonial = 0x07,
	admin = 0x08,
	recruitment = 0x09,
	national_focus = 0x0A,
	rgo_output = 0x0B,
	population = 0x0C,
	nationality = 0x0D,
	sphere = 0x0E,
	supply = 0x0F,
	party_loyalty = 0x10,
	rank = 0x11,
	migration = 0x12,
	civilization_level = 0x13,
	relation = 0x14,
	crisis = 0x15,
	naval = 0x16,
	religion = 0x17,
	fort = 0x18,
	ideology = 0x19,
	income = 0x1a,
	conciousness = 0x1b,
	militancy = 0x1c,
	literacy = 0x1d,
	employment = 0x1e,
	factories = 0x1f,
	issues = 0x20,
	growth = 0x21,
	players = 0x22,
	life_needs = 0x23,
	everyday_needs = 0x24,
	luxury_needs = 0x25,
	officers = 0x26,
	clerk_to_craftsmen_ratio = 0x27,
	life_rating = 0x28,
	crime = 0x29,
	rally = 0x2a,
	workforce = 0x2b,
	mobilization = 0x2c,
	state_select = 0x2d,
};

const uint8_t PROV_COLOR_LAYERS = 2;

void set_map_mode(sys::state& state, mode mode);
void update_map_mode(sys::state& state);
} // namespace map_mode
