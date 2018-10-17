//
// Created by sebastian on 23.09.18.
//

#pragma once

#include <chrono>
#include <cmath>
#include "../../utils/scales/UnitScale.h"
#include "../../CustomDataTypes/Volt.h"

#ifdef debug
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-variable"
#endif


namespace NS_A4963 {

    namespace detail {

        enum class RegisterMask : uint16_t {
            /* Common Addresses */
            RegisterAddress                     = 0b1110000000000000,
            WriteAddress                        = 0b0001000000000000,
            RegisterAndWriteAddress             = 0b1111000000000000,
            GeneralData                         = 0b0000111111111111,

            /* Config0 Addresses */
            RecirculationModeAddress            = 0b0000110000000000,
            BlankTimeAddress                    = 0b0000001111000000,
            DeadTimeAddress                     = 0b0000000000111111,

            /* Config1 Addresses */
            PercentFastDecayAddress             = 0b0000100000000000,
            InvertPWMInputAddress               = 0b0000010000000000,
            CurrentSenseThresholdVoltageAddress = 0b0000001111000000,
            BemfTimeQualifierAddress            = 0b0000000000100000,
            VDSThresholdAddress                 = 0b0000000000011111,

            /* Config2 Addresses */
            PositionControllerProportionalGain  = 0b0000111100000000,
            OverspeedLimitRatio                 = 0b0000000011000000,
            DegaussCompensation                 = 0b0000000000100000,
            FixedPeriod                         = 0b0000000000011111,

            /* Config3 Addresses */
            PositionControllerIntegralGain      = 0b0000111100000000,
            PWMDutyCycleHoldTorque              = 0b0000000011110000,
            HoldTime                            = 0b0000000000001111,
        };
        enum class RegisterCodes : uint8_t {
            Config0     = 0b000,
            Config1     = 0b001,
            Config2     = 0b010,
            Config3     = 0b011,
            Config4     = 0b100,
            Config5     = 0b101,
            Mask        = 0b110,
            Run         = 0b111,
            Diagnostic  = 0b110
        };
    }
    using namespace std::literals::chrono_literals;
    using namespace CustomDataTypes::Electricity::literals;
    using namespace detail;

    enum class A4963RegisterNames {
        BlankTime,
        DeadTime,
        CurrentSenseThresholdVoltage,
        VDSThreshold,
        PositionControllerProportionalGain,
        RecirculationMode,
        PercentFastDecay,
        InvertPWMInput,
        BemfTimeQualifier,
    };

    template<A4963RegisterNames reg>
    struct RegisterValues;

    template<>
    struct RegisterValues<A4963RegisterNames::BlankTime> {
        static inline constexpr bool isRanged = true;
        static constexpr auto mask = RegisterMask::BlankTimeAddress;
        static constexpr auto code = RegisterCodes::Config0;
        static constexpr auto min = 0ns;
        static constexpr auto max = 6us;
        static constexpr auto functor = [](auto t1) { return t1 * 400ns; };
        static constexpr auto inverse_functor = [](auto t1) { return static_cast<ssize_t>(t1 / 400ns); };
        static constexpr NewUnitScale<min, max, functor, inverse_functor> value{};

    };

    template<>
    struct RegisterValues<A4963RegisterNames::DeadTime> {
        static inline constexpr bool isRanged = true;
        static constexpr auto mask = RegisterMask::DeadTimeAddress;
        static constexpr auto code = RegisterCodes::Config0;
        static constexpr auto min = 100.0ns;
        static constexpr auto max = 3.15us;
        static constexpr auto functor = [](auto t1) { return t1 * 50.0ns; };
        static constexpr auto inverse_functor = [](auto t1) { return static_cast<ssize_t>(t1 / 50.0ns); };
        static constexpr NewUnitScale<min, max, functor, inverse_functor> value{};
    };

    template<>
    struct RegisterValues<A4963RegisterNames::CurrentSenseThresholdVoltage> {
        static inline constexpr bool isRanged = true;
        static constexpr auto min = 12.5_mV;
        static constexpr auto max = 200.0_mV;
        static constexpr auto functor = [](auto t1) { return (t1 + 1) * 12.5_mV; };
        static constexpr auto inverse_functor = [](auto t1) { return static_cast<ssize_t>(t1 / 12.5_mV) - 1; };
        static constexpr NewUnitScale<min, max, functor, inverse_functor> value{};
    };

    template<>
    struct RegisterValues<A4963RegisterNames::VDSThreshold> {
        static inline constexpr bool isRanged = true;
        static constexpr auto min = 0.0_mV;
        static constexpr auto max = 1.55_V;
        static constexpr auto functor = [](auto t1) { return t1 * 50.0_mV; };
        static constexpr auto inverse_functor = [](auto t1) { return static_cast<ssize_t>(t1 / 50.0_mV); };
        static constexpr NewUnitScale<min, max, functor, inverse_functor> value{};
    };

    template<>
    struct RegisterValues<A4963RegisterNames::PositionControllerProportionalGain> {
        static inline constexpr bool isRanged = true;
        static constexpr auto min = 1.0/128;
        static constexpr auto max = 256.0;
        static constexpr auto functor = [](auto t1) { return std::exp(2, t1 + 7); };
        static constexpr auto inverse_functor = [](auto t1) { return static_cast<ssize_t>(std::log2(t1) + 7); };
        static constexpr NewUnitScale<min, max, functor, inverse_functor> value{};
    };

    template<>
    struct RegisterValues<A4963RegisterNames::RecirculationMode> {
        static inline constexpr bool isRanged = false;
        static constexpr auto mask = RegisterMask::RecirculationModeAddress;
        static constexpr auto code = RegisterCodes::Config0;

        enum class values : uint8_t {
            Auto = 0b00,
            High = 0b01,
            Low = 0b10,
            Off = 0b11
        };
    };

    template<>
    struct RegisterValues<A4963RegisterNames::BemfTimeQualifier> {
        static inline constexpr bool isRanged = false;
        static constexpr auto mask = RegisterMask::BemfTimeQualifierAddress;
        static constexpr auto code = RegisterCodes::Config1;

        enum class values : uint8_t {
            DebounceTimer = 0,
            WindowTimer = 1
        };
    };

    template<>
    struct RegisterValues<A4963RegisterNames::InvertPWMInput> {
        static inline constexpr bool isRanged = false;
        static constexpr auto mask = RegisterMask::InvertPWMInputAddress;
        static constexpr auto code = RegisterCodes::Config1;

        enum class values : uint8_t {
            NormalTrueLogic = 0,
            InverterLogic = 1
        };
    };

    template<>
    struct RegisterValues<A4963RegisterNames::PercentFastDecay> {
        static inline constexpr bool isRanged = false;
        static constexpr auto mask = RegisterMask::PercentFastDecayAddress;
        static constexpr auto code = RegisterCodes::Config1;

        enum class values : uint8_t {
            T12_5Percent = 0,
            T25Percent = 1
        };
    };
}

#ifdef debug
    #pragma GCC diagnostic pop
#endif