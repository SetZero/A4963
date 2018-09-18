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

TEST_CASE("Compare two Volt Representations", "[Volt Comparison]") {
    Volt<std::intmax_t, std::milli>     v1{1000};
    Volt<std::intmax_t>                 v2{1};
    Volt<std::intmax_t, std::nano>      v3{1};

    REQUIRE(v1 == v2);
    REQUIRE(v1 != v3);
}

TEST_CASE("Test Volt Literals", "[Volt Literals]") {
    using namespace CustomDataTypes::literals;
    REQUIRE(1_V == 1000_mV);
    REQUIRE(1000_kV == 1_MV);
    REQUIRE(1000_nV != 1_MV);
}

TEST_CASE("Compare greater/smaller", "[Volt Comparison 2]") {
    Volt<std::intmax_t, std::milli>     v1{1000};
    Volt<std::intmax_t, std::milli>     v2{999};
    Volt<std::intmax_t, std::kilo>      v3{1000};
    Volt<std::intmax_t, std::tera>      v4{1000};
    Volt<std::intmax_t>                 v5{1};
    Volt<std::intmax_t, std::nano>      v6{1};

    REQUIRE(v1 > v2);
    REQUIRE(v2 < v1);
    REQUIRE(v1 < v4);
    REQUIRE(v3 > v2);
    REQUIRE(v5 > v2);
    REQUIRE(v6 < v4);
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