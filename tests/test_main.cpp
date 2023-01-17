#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

TEST_CASE("Dummy test", "[dummy test instance]") {
    REQUIRE(1 + 1 == 2);
}