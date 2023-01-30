#pragma once
#include <stdint.h>

namespace nations {
uint32_t tag_to_int(char first, char second, char third) {
	return (uint32_t(first) << 16) | (uint32_t(first) << 8) | (uint32_t(first) << 0);
}
}

