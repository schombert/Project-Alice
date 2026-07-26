#include "economy/land_ownership.hpp"

#include <limits>

TEST_CASE("land ownership claims form a bounded distribution",
		"[economy][ownership]") {
	auto const result =
		economy::land_ownership::target_from_claims(60.f, 30.f, 10.f);
	CHECK(result.smallholders == Approx(0.60f));
	CHECK(result.landed_elites == Approx(0.30f));
	CHECK(result.capitalists == Approx(0.10f));
	CHECK(result.smallholders + result.landed_elites + result.capitalists
		== Approx(1.f));
}

TEST_CASE("existing land ownership changes gradually",
		"[economy][ownership]") {
	economy::land_ownership::distribution current{0.80f, 0.10f};
	economy::land_ownership::distribution target{0.20f, 0.50f};
	auto const next =
		economy::land_ownership::advance(current, target, 0.01f);

	CHECK(next.landed_elites == Approx(0.794f));
	CHECK(next.capitalists == Approx(0.104f));
	CHECK(next.smallholders == Approx(0.102f));
	CHECK(next.landed_elites > target.landed_elites);
	CHECK(next.capitalists < target.capitalists);
}

TEST_CASE("empty legacy ownership initializes on its first observation",
		"[economy][ownership]") {
	auto const initialized = economy::land_ownership::advance(
		{}, {0.35f, 0.15f});
	CHECK(initialized.landed_elites == Approx(0.35f));
	CHECK(initialized.capitalists == Approx(0.15f));
	CHECK(initialized.smallholders == Approx(0.50f));
}

TEST_CASE("land ownership sanitizes invalid inputs",
		"[economy][ownership]") {
	auto const nan = std::numeric_limits<float>::quiet_NaN();
	auto const infinity = std::numeric_limits<float>::infinity();
	auto const result = economy::land_ownership::advance(
		{nan, infinity}, {2.f, 2.f}, nan);

	CHECK(std::isfinite(result.landed_elites));
	CHECK(std::isfinite(result.capitalists));
	CHECK(std::isfinite(result.smallholders));
	CHECK(result.landed_elites >= 0.f);
	CHECK(result.capitalists >= 0.f);
	CHECK(result.smallholders >= 0.f);
	CHECK(result.landed_elites + result.capitalists + result.smallholders
		== Approx(1.f));
}
