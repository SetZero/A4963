//
// Created by keven on 26.10.18.
//


#include "catch.hpp"
#include "CustomDataTypes/Percentage.h"
using namespace CustomDataTypes::literals;



TEST_CASE("Percentage operator =="){
        auto every = 42.29382749287349279_perc;
        auto thing = 42.29382749287349279_perc;
        REQUIRE(every == thing);
}

TEST_CASE("Percentage operator+"){
    auto erg = 23_perc + 19_perc;
    REQUIRE(erg == 42_perc);
}

TEST_CASE("Percentage operator-"){
    auto erg = 64_perc - 22_perc;
    REQUIRE(erg == 42_perc);
}

TEST_CASE("Percentage operator*"){
    auto erg = 64_perc * 20_perc;
    auto erg1 = 20_perc * 2.1;
    REQUIRE(erg == 12.8_perc);
    REQUIRE(erg1 == 42_perc);
}

TEST_CASE("Percentage operator/"){
    auto erg = 64_perc / 20_perc;
    auto erg1 = 42_perc / 2;
    auto erg2 = 42 / 2_perc;
    REQUIRE(erg == 0.032_perc);
    REQUIRE(erg1 == 21_perc);
    REQUIRE(erg2 == 21_perc);
}

TEST_CASE("Percentage Lambdas"){
    static constexpr auto min = 6.25_perc;
    static constexpr auto functor = [](auto t1) { return (t1+1)*min; };
    static constexpr auto inverse_functor = [](auto t1) { return static_cast<ssize_t>((t1/min.getPercent())-1.0_perc); };
    REQUIRE(functor(3) ==  25_perc);
    REQUIRE(inverse_functor(25_perc) == 3);
}
