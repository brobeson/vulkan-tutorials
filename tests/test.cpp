#include <catch2/catch_test_macros.hpp>

SCENARIO("A developer wants a working test tree.") {
  constexpr auto sum{9};
  CHECK(sum == 9);
}
