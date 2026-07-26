TEST_CASE("Daily policy budget uses income instead of accumulated treasury", "[economy][budget][stability]") {
	using economy::national_budget::sustainable_daily_budget;
	CHECK(sustainable_daily_budget(418.f, 49.7f) == Approx(49.7f));
	CHECK(sustainable_daily_budget(20.f, 49.7f) == Approx(20.f));
	CHECK(sustainable_daily_budget(418.f, -5.f) == Approx(0.f));
	CHECK(sustainable_daily_budget(-1.f, 49.7f) == Approx(0.f));
	CHECK(sustainable_daily_budget(
		std::numeric_limits<float>::infinity(), 49.7f) == Approx(0.f));
}

TEST_CASE("Factory expansion is limited by upgrade construction time", "[economy][stability]") {
	CHECK(economy::stability::factory_daily_expansion(1'000'000.f, 10'000.f, 365.f)
		== Approx(10'000.f / 365.f));
	CHECK(economy::stability::factory_daily_expansion(5.f, 10'000.f, 365.f)
		== Approx(5.f));
	CHECK(economy::stability::factory_daily_expansion(
		std::numeric_limits<float>::infinity(), 10'000.f, 365.f) == 0.f);
	CHECK(economy::stability::factory_daily_expansion(100.f, 0.f, 365.f) == 0.f);
}

TEST_CASE("RGO expansion cannot consume unbounded local construction labor", "[economy][stability]") {
	CHECK(economy::stability::rgo_daily_expansion(500.f, 500.f, 20.f, 1'000.f)
		== Approx(5.f));
	CHECK(economy::stability::rgo_daily_expansion(500.f, 2.f, 20.f, 1'000.f)
		== Approx(2.f));
	CHECK(economy::stability::rgo_daily_expansion(500.f, 500.f, 20.f, 0.f) == 0.f);
}

TEST_CASE("Industrial score conversion saturates safely", "[economy][stability]") {
	CHECK(nations::saturated_industrial_score(-1.0) == 0);
	CHECK(nations::saturated_industrial_score(std::numeric_limits<double>::quiet_NaN()) == 0);
	CHECK(nations::saturated_industrial_score(1234.9) == 1234);
	CHECK(nations::saturated_industrial_score(1.0e30) == std::numeric_limits<uint16_t>::max());
	CHECK(nations::saturated_industrial_score(std::numeric_limits<double>::infinity())
		== std::numeric_limits<uint16_t>::max());
}
