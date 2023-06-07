#pragma once

namespace sys {
struct state;
}

namespace rng {

struct random_pair {
	uint64_t high;
	uint64_t low;
};

uint64_t get_random(sys::state const& state, uint32_t value_in);		 // try to populate as many bits of value_in as you can
random_pair get_random_pair(sys::state const& state, uint32_t value_in); // each call natively generates 128 random bits anyways
uint64_t get_random(sys::state const& state, uint32_t value_in_hi, uint32_t value_in_lo);

} // namespace rng
