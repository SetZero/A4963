//
// Created by sebastian on 17.09.18.
//

#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "../src/CustomDataTypes/Volt.h"

template<typename Rep, typename Period = std::ratio<1> >
using Volt = CustomDataTypes::Volt<Rep, Period>;

TEST_CASE("Test Volt Convertion Floating", "[Volt Floating]") {
    Volt<double, std::milli> v{1};
    auto convertedNone = static_cast<Volt<double>>(v);
    auto convertedKilo = static_cast<Volt<double, std::kilo>>(v);

    REQUIRE(convertedNone.count()    == Approx(0.001));
    REQUIRE(convertedKilo.count()    == Approx(0.000001));
}

TEST_CASE("Test Volt Convertion", "[Volt]") {
    Volt<std::intmax_t, std::mega> v{1000000};
    auto convertedTera   = static_cast<Volt<std::intmax_t, std::tera>>(v);
    auto convertedGiga   = static_cast<Volt<std::intmax_t, std::giga>>(v);
    auto convertedNone   = static_cast<Volt<std::intmax_t, std::mega>>(v);
    auto convertedKilo   = static_cast<Volt<std::intmax_t, std::kilo>>(v);
    auto convertedNormal = static_cast<Volt<std::intmax_t>>(v);
    auto convertedMilli  = static_cast<Volt<std::intmax_t, std::milli>>(v);
    auto convertedMicro  = static_cast<Volt<std::intmax_t, std::micro>>(v);

    REQUIRE(convertedTera.count()    == 1);
    REQUIRE(convertedGiga.count()    == 1000);
    REQUIRE(convertedNone.count()    == 1000000);
    REQUIRE(convertedKilo.count()    == 1000000000);
    REQUIRE(convertedNormal.count()  == 1000000000000);
    REQUIRE(convertedMilli.count()   == 1000000000000000);
    REQUIRE(convertedMicro.count()   == 1000000000000000000);
}