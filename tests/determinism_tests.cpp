#include <cstring>
#include "catch.hpp"
#include "parsers_declarations.hpp"
#include "dcon_generated.hpp"
#include "nations.hpp"
#include "container_types.hpp"
#include "system_state.hpp"
#include "serialization.hpp"
#include "prng.hpp"

TEST_CASE("prng_simple", "[determinism]") {
	std::unique_ptr<sys::state> game_state = std::make_unique<sys::state>(); // too big for the stack
	game_state->game_seed = 64273;
	game_state->current_date.value = 49963;
	auto r1 = rng::get_random(*game_state, ~0x6a3f);
	auto r2 = rng::get_random(*game_state, ~0x6a3f);
	REQUIRE(r1 == r2);
}

TEST_CASE("prng_low_entropy", "[determinism]") {
	std::unique_ptr<sys::state> game_state = std::make_unique<sys::state>(); // too big for the stack
	game_state->game_seed = 1;
	game_state->current_date.value = 1;
	auto r1 = rng::get_random(*game_state, 1);
	auto r2 = rng::get_random(*game_state, 1);
	REQUIRE(r1 == r2);
}

#define UNOPTIMIZABLE_FLOAT(name, value) \
	char name##_storage[sizeof(float)]; \
	new (&name##_storage) float(value); \
	const float *name##_p = std::launder(reinterpret_cast<const float*>(&name##_storage)); \
	const float name = *name##_p;

TEST_CASE("fp_mul", "[determinism]") {
	UNOPTIMIZABLE_FLOAT(f1, 2.f);
	UNOPTIMIZABLE_FLOAT(f2, 4.f);
	REQUIRE(f1 * f2 == 8.f);
}
TEST_CASE("fp_fmadd", "[determinism]") {
	UNOPTIMIZABLE_FLOAT(f1, 2.f);
	UNOPTIMIZABLE_FLOAT(f2, 4.f);
	UNOPTIMIZABLE_FLOAT(f3, 3.f);
	UNOPTIMIZABLE_FLOAT(f4, 2.f);
	
	float fmadd_1 = f1 * f2 + f3 * f4; //equation
	float fmadd_2 = f3 * f4 + f1 * f2; //reverse products
	float fmadd_3 = f1 * (f2 + (f3 * f4) / f1); //factorization by f1
	float fmadd_4 = f2 * (f1 + (f3 * f4) / f2); //factorization by f2
	float fmadd_5 = f3 * ((f1 * f2) / f3 + f4); //factorization by f3
	float fmadd_6 = f4 * ((f1 * f2) / f4 + f3); //factorization by f4

	REQUIRE(fmadd_1 == fmadd_2);
	REQUIRE(fmadd_1 == fmadd_3);
	REQUIRE(fmadd_1 == fmadd_4);
	REQUIRE(fmadd_1 == fmadd_5);
	REQUIRE(fmadd_1 == fmadd_6);
}

TEST_CASE("fp_fprec", "[determinism]") {
	UNOPTIMIZABLE_FLOAT(f1, 2.f);
	UNOPTIMIZABLE_FLOAT(f2, 4.f);
	UNOPTIMIZABLE_FLOAT(f3, 3.f);
	
	float fprec_1 = f1 * f2 / f3; //equation
	float fprec_2 = (f1 * f2) / f3; //same
	float fprec_3 = f1 * (f2 / f3); //same
	float fprec_4 = (f1 / f3) * f2; //same

	REQUIRE(fprec_1 == fprec_2);
	REQUIRE(fprec_1 == fprec_3);
	REQUIRE(fprec_1 == fprec_4);
}

TEST_CASE("math_fns", "[determinism]") {
	for(float k = 1.f; k <= 128.f; k += 1.f) {
		UNOPTIMIZABLE_FLOAT(f1, k);
		REQUIRE(math::sqrt(f1) == Approx(std::sqrt(f1)).margin(0.01f));
	}
	for(float k = -1.f; k <= 1.f; k += 0.01f) {
		UNOPTIMIZABLE_FLOAT(f1, k);
		REQUIRE(math::acos(f1) == Approx(std::acos(f1)).margin(0.018f));
	}
	for(float k = -math::pi; k <= math::pi; k += 0.01f) {
		UNOPTIMIZABLE_FLOAT(f1, k);
		REQUIRE(math::cos(f1) == Approx(std::cos(f1)).margin(0.01f));
		REQUIRE(math::sin(f1) == Approx(std::sin(f1)).margin(0.01f));
	}
}

#undef UNOPTIMIZABLE_FLOAT

TEST_CASE("sim_0", "[determinism]") {
	// Test that the game states are equal AFTER loading
	std::unique_ptr<sys::state> game_state_1 = load_testing_scenario_file();
	game_state_1->game_seed = 808080;
	std::unique_ptr<sys::state> game_state_2 = load_testing_scenario_file();
	game_state_2->game_seed = 808080;
	// should be equal in memory
	REQUIRE(std::memcmp(&game_state_1->world, &game_state_2->world, sizeof(dcon::data_container)) == 0);
}

TEST_CASE("sim_1", "[determinism]") {
	// Test that the game states are equal after loading and performing 1 tick
	std::unique_ptr<sys::state> game_state_1 = load_testing_scenario_file();
	game_state_1->game_seed = 808080;
	game_state_1->single_game_tick(); // 1 tick
	std::unique_ptr<sys::state> game_state_2 = load_testing_scenario_file();
	game_state_2->game_seed = 808080;
	game_state_2->single_game_tick(); // 1 tick
	// should be equal in memory
	REQUIRE(std::memcmp(&game_state_1->world, &game_state_2->world, sizeof(dcon::data_container)) == 0);
}
