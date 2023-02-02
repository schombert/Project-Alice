#pragma once
#include <stdint.h>
#include <vector>
#include "dcon_generated.hpp"

namespace nations {
uint32_t tag_to_int(char first, char second, char third) {
	return (uint32_t(first) << 16) | (uint32_t(first) << 8) | (uint32_t(first) << 0);
}

struct triggered_modifier {
	dcon::modifier_id linked_modifier;
	// trigger id
};

struct global_national_state {
	std::vector<triggered_modifier> triggered_modifiers;
};

}

