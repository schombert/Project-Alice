#include "prng.hpp"
#include "system_state.hpp"

#include "random123/philox.h"

namespace rng {


uint64_t get_random(sys::state const& state, uint32_t value_in) { // try to populate as many bits of value_in as you can
	r123::Philox4x32 rng;
	r123::Philox4x32::ctr_type c = { state.current_date.value , value_in };
	r123::Philox4x32::key_type k = { state.game_seed };

	r123::Philox4x32::ctr_type r = rng(c, k);
	

	return (uint64_t(r[0]) << 32) | uint64_t(r[1]);
}
uint64_t get_random(sys::state const& state, uint32_t value_in_hi, uint32_t value_in_lo) {
	r123::Philox4x32 rng;
	r123::Philox4x32::ctr_type c = { value_in_hi , value_in_lo };
	r123::Philox4x32::key_type k = { state.game_seed };

	r123::Philox4x32::ctr_type r = rng(c, k);


	return (uint64_t(r[0]) << 32) | uint64_t(r[1]);
}
random_pair get_random_pair(sys::state const& state, uint32_t value_in) { // each call natively generates 128 random bits anyways

	r123::Philox4x32 rng;
	r123::Philox4x32::ctr_type c = { state.current_date.value , value_in };
	r123::Philox4x32::key_type k = { state.game_seed };

	r123::Philox4x32::ctr_type r = rng(c, k);

	return random_pair{ (uint64_t(r[0]) << 32) | uint64_t(r[1]), (uint64_t(r[2]) << 32) | uint64_t(r[3]) };
}

}
