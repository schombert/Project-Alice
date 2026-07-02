#include "economy/economy_pops.hpp"

TEST_CASE("safe spending ratio returns fallback when required is zero", "[economy][pops]") {
	auto scalar = economy::pops::safe_spending_ratio(true, 0.0f, 0.0f, 1.0f);
	REQUIRE(scalar == 1.0f);

	auto vector = economy::pops::safe_spending_ratio(
		ve::mask_vector(true, false, true, false),
		ve::fp_vector(0.0f, 2.0f, 0.0f, 6.0f),
		ve::fp_vector(0.0f, 4.0f, 0.0f, 3.0f),
		ve::fp_vector(1.0f)
	);
	REQUIRE(vector[0] == 1.0f);
	REQUIRE(vector[1] == 0.5f);
	REQUIRE(vector[2] == 1.0f);
	REQUIRE(vector[3] == 2.0f);
}
