//
// Created by sebastian on 23.09.18.
//

#pragma once

#include <chrono>
#include <cmath>
#include "../../utils/scales/UnitScale.h"
#include "../../CustomDataTypes/Volt.h"

namespace NS_A4963 {

    using namespace std::literals::chrono_literals;
    using namespace CustomDataTypes::Electricity::literals;

    enum class A4963RegisterNames {
        BlankTime,
        DeadTime,
        CurrentSenseThresholdVoltage,
        VDSThreshold,
        PositionControllerProportionalGain
    };

    template<A4963RegisterNames reg>
    struct RegisterValues;

    template<>
    struct RegisterValues<A4963RegisterNames::BlankTime> {
        static constexpr auto min = 0ns;
        static constexpr auto max = 6us;
        static constexpr auto functor = [](auto t1) { return t1 * 400ns; };
        static constexpr auto reverse_functor = [](auto t1) { return static_cast<ssize_t>(t1 / 400ns); };
        static constexpr NewUnitScale<min, max, functor, reverse_functor> value{};

    };

    template<>
    struct RegisterValues<A4963RegisterNames::DeadTime> {
        static constexpr auto min = 100ns;
        static constexpr auto max = 3.15us;
        static constexpr auto functor = [](auto t1) { return t1 * 50ns; };
        static constexpr auto reverse_functor = [](auto t1) { return static_cast<ssize_t>(t1 / 50ns); };
        static constexpr NewUnitScale<min, max, functor, reverse_functor> value{};
    };

    template<>
    struct RegisterValues<A4963RegisterNames::CurrentSenseThresholdVoltage> {
        static constexpr auto min = 12.5_mV;
        static constexpr auto max = 200.0_mV;
        static constexpr auto functor = [](auto t1) { return (t1 + 1) * 12.5_mV; };
        static constexpr auto reverse_functor = [](auto t1) { return static_cast<ssize_t>(t1 / 12.5_mV) - 1; };
        static constexpr NewUnitScale<min, max, functor, reverse_functor> value{};
    };

    template<>
    struct RegisterValues<A4963RegisterNames::VDSThreshold> {
        static constexpr auto min = 0.0_mV;
        static constexpr auto max = 1.55_V;
        static constexpr auto functor = [](auto t1) { return t1 * 50.0_mV; };
        static constexpr auto reverse_functor = [](auto t1) { return static_cast<ssize_t>(t1 / 50.0_mV); };
        static constexpr NewUnitScale<min, max, functor, reverse_functor> value{};
    };

    template<>
    struct RegisterValues<A4963RegisterNames::PositionControllerProportionalGain> {
        static constexpr auto min = 1.0/128;
        static constexpr auto max = 256;
        static constexpr auto functor = [](auto t1) { return std::exp(2, t1 + 7); };
        static constexpr auto reverse_functor = [](auto t1) { return static_cast<ssize_t>(std::log2(t1) + 7); };
        static constexpr NewUnitScale<min, max, functor, reverse_functor> value{};
    };
}