#include <catch2/catch_test_macros.hpp>

TEST_CASE("Smoke test — harness builds and Catch2 runs", "[smoke]") {
    REQUIRE(1 + 1 == 2);
}
