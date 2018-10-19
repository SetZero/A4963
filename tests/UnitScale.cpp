//
// Created by sebastian on 15.10.18.
//

#include "catch.hpp"
#include "../src/utils/scales/UnitScale.h"
#include "SIUnit.h"

using namespace CustomDataTypes::Electricity::literals;
struct Test {
    static constexpr auto min = 0.0_mV;
    static constexpr auto max = 1.55_V;
    static constexpr auto functor = [](auto t1) { return t1 * 50.0_mV; };
    static constexpr auto reverse_functor = [](auto t1) { return static_cast<ssize_t>(t1 / 50.0_mV); };
    static constexpr NewUnitScale<min, max, functor, reverse_functor> value{};
};

TEST_CASE("Simple Unit Scale Test", "[UnitScale Test]") {
    REQUIRE(Test::value.getMaxValue() == 1.55_V);
    REQUIRE(Test::value.getMinValue() == 0.0_V);
    REQUIRE(Test::value.convertValue(50.0_mV) == 1);
    REQUIRE(Test::value.convertValue(1.55_V) == 31);
    REQUIRE(Test::value.getActualValue(1) == 50.0_mV);
    REQUIRE(Test::value.getActualValue(31) == 1.55_V);
}