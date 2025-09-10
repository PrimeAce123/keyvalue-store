#include "catch/catch.hpp"
#include "p2/p2_sample.hpp"

TEST_CASE("Random Test 2", "[math]") {
    REQUIRE(2 * 3 == 6);
    REQUIRE(p2::TestFunction() == 2);
}


