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
        using type = UnitScale<std::chrono::duration<long double, std::nano>,  uint16_t >;
        static constexpr type value{400ns, 6us, 0us};
        static constexpr auto normalizer = [](auto input) { return static_cast<type::value_type>(input / 400ns); };
    };

    template<>
    struct RegisterValues<A4963RegisterNames::DeadTime> {
        using type = UnitScale<std::chrono::duration<long double, std::nano>, uint16_t>;
        static constexpr type value{50ns, 3.15us, 100ns};
        static constexpr auto normalizer = [](auto input) { return static_cast<type::value_type>(input / 50ns); };
    };

    template<>
    struct RegisterValues<A4963RegisterNames::CurrentSenseThresholdVoltage> {
        using type = UnitScale<CustomDataTypes::Electricity::Volt<long double, std::milli>, uint16_t>;
        static constexpr type value{12.5_mV, 200.0_mV, 12.5_mV};
        static constexpr auto normalizer = [](auto input) { return (static_cast<type::value_type>(input / 12.5_mV) - 1); };
    };

    template<>
    struct RegisterValues<A4963RegisterNames::VDSThreshold> {
        using type = UnitScale<CustomDataTypes::Electricity::Volt<long double, std::milli>, uint16_t>;
        static constexpr type value{50.0_mV, 1.55_V, 0.0_mV};
        static constexpr auto normalizer = [](auto input) { return static_cast<type::value_type>(input / 50.0_mV); };
    };

    template<>
    struct RegisterValues<A4963RegisterNames::PositionControllerProportionalGain> {
        using type = UnitScale<double, uint16_t>;
        static constexpr type value{0.0, 1.0/128, 256};
        static constexpr auto normalizer = [](auto input) { return static_cast<type::value_type>(std::log2(input) + 7); };
    };
}