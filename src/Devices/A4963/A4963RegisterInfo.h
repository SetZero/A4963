//
// Created by sebastian on 23.09.18.
//

#pragma once

#include <chrono>
#include <cmath>
#include "../../utils/scales/UnitScale.h"
#include "../../CustomDataTypes/SIUnit.h"
#include "../../CustomDataTypes/Volt.h"
#include "../../CustomDataTypes/Hertz.h"
#include "../../CustomDataTypes/Percentage.h"

#ifdef debug
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-variable"
#endif


namespace NS_A4963 {

    namespace detail {

        enum class Diagnostic : uint16_t {
            PhaseCLowSideVDS = (1 << 0),
            PhaseCHighSideVDS = (1 << 1),
            PhaseBLowSideVDS = (1 << 2),
            PhaseBHighSideVDS = (1 << 3),
            PhaseALowSideVDS = (1 << 4),
            PhaseAHighSideVDS = (1 << 5),
            UndefinedBit1 = (1 << 6),
            VBBUndervoltage = (1 << 7),
            UndefinedBit2 = (1 << 8),
            LossOfBemfSynchronization = (1 << 9),
            OverTemperature = (1 << 10),
            HighTemperature = (1 << 11),
            UndefinedBit3 = (1 << 12),
            SerialTransferError = (1 << 13),
            PowerOnReset = (1 << 14),
            DiagnosticRegisterFlag = (1 << 15)
        };

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

            /* Config4 Addresses */
            PIControllerProportionalGain        = 0b0000111100000000,
            PWMDutyCycleTorqueStartup           = 0b0000000011110000,
            StartSpeed                          = 0b0000000000001111,

            /* Config5 Addresses */
            PIControllerIntegralGain            = 0b0000111100000000,
            SpeedOutputSelection                = 0b0000000010000000,
            MaximumSpeedSetting                 = 0b0000000001110000,
            PhaseAdvance                        = 0b0000000000001111,

            /* Run register Addresses */
            MotorControlMode                    = 0b0000110000000000,
            EnableStopOnFail                    = 0b0000001000000000,
            DutyCycleControl                    = 0b0000000111110000,
            RestartControl                      = 0b0000000000001000,
            Brake                               = 0b0000000000000100,
            DirectionOfRotation                 = 0b0000000000000010,
            Run                                 = 0b0000000000000001,

            /* Mask Addresses*/
            PhaseCLowSideVDS = (1 << 0),
            PhaseCHighSideVDS = (1 << 1),
            PhaseBLowSideVDS = (1 << 2),
            PhaseBHighSideVDS = (1 << 3),
            PhaseALowSideVDS = (1 << 4),
            PhaseAHighSideVDS = (1 << 5),
            UndefinedBit1 = (1 << 6),
            VBBUndervoltage = (1 << 7),
            UndefinedBit2 = (1 << 8),
            LossOfBemfSynchronization = (1 << 9),
            OverTemperature = (1 << 10),
            TemperatureWarning = (1 << 11)
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
    using namespace CustomDataTypes::Frequency::literals;
    using namespace CustomDataTypes::literals;
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
        OverspeedLimitRatio,
        DegaussCompensation,
        FixedPeriod,
        PositionControllerIntegralGain,
        PWMDutyCycleHoldTorque,
        HoldTime,
        PIControllerProportionalGain,
        PWMDutyCycleTorqueStartup,
        StartSpeed,
        PIControllerIntegralGain,
        SpeedOutputSelection,
        MaximumSpeedSetting,
        PhaseAdvance,
        MotorControlMode,
        EnableStopOnFail,
        DutyCycleControl,
        RestartControl,
        Brake,
        DirectionOfRotation,
        Run
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
        static constexpr std::string_view name = "Blank Time";

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
        static constexpr std::string_view name = "Dead Time";
    };

    template<>
    struct RegisterValues<A4963RegisterNames::CurrentSenseThresholdVoltage> {
        static inline constexpr bool isRanged = true;
        static constexpr auto mask = RegisterMask::CurrentSenseThresholdVoltageAddress;
        static constexpr auto code = RegisterCodes::Config1;
        static constexpr auto min = 12.5_mV;
        static constexpr auto max = 200.0_mV;
        static constexpr auto functor = [](auto t1) { return (t1 + 1) * min; };
        static constexpr auto inverse_functor = [](auto t1) { return static_cast<ssize_t>(t1 / min) - 1; };
        static constexpr NewUnitScale<min, max, functor, inverse_functor> value{};
        static constexpr std::string_view name = "Current Sense Threshold Voltage";
    };

    template<>
    struct RegisterValues<A4963RegisterNames::VDSThreshold> {
        static inline constexpr bool isRanged = true;
        static constexpr auto mask = RegisterMask::VDSThresholdAddress;
        static constexpr auto code = RegisterCodes::Config1;
        static constexpr auto min = 0.0_mV;
        static constexpr auto max = 1.55_V;
        static constexpr auto functor = [](auto t1) { return t1 * 50.0_mV; };
        static constexpr auto inverse_functor = [](auto t1) { return static_cast<ssize_t>(t1 / 50.0_mV); };
        static constexpr NewUnitScale<min, max, functor, inverse_functor> value{};
        static constexpr std::string_view name = "VDS Threshold";
    };

    template<>
    struct RegisterValues<A4963RegisterNames::PositionControllerProportionalGain> {
        static inline constexpr bool isRanged = true;
        static constexpr auto mask = RegisterMask::PositionControllerProportionalGain;
        static constexpr auto code = RegisterCodes::Config2;
        static constexpr auto min = 1.0/128;
        static constexpr auto max = 256.0;
        static constexpr auto functor = [](auto t1) { return std::exp2(t1 - 7); };
        static constexpr auto inverse_functor = [](auto t1) { return static_cast<ssize_t>(std::log2(t1) + 7); };
        static constexpr NewUnitScale<min, max, functor, inverse_functor> value{};
        static constexpr std::string_view name = "Position Controller Proportional Gain";
    };

    template<>
    struct RegisterValues<A4963RegisterNames::RecirculationMode> {
        static inline constexpr bool isRanged = false;
        static constexpr auto mask = RegisterMask::RecirculationModeAddress;
        static constexpr auto code = RegisterCodes::Config0;
        static constexpr std::string_view name = "Recirculation Mode";

        enum class values : uint16_t {
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
        static constexpr std::string_view name = "Bemf Time Qualifier";

        enum class values : uint16_t {
            DebounceTimer = 0,
            WindowTimer = 1
        };
    };

    template<>
    struct RegisterValues<A4963RegisterNames::InvertPWMInput> {
        static inline constexpr bool isRanged = false;
        static constexpr auto mask = RegisterMask::InvertPWMInputAddress;
        static constexpr auto code = RegisterCodes::Config1;
        static constexpr std::string_view name = "Invert PWM Input";

        enum class values : uint16_t {
            NormalTrueLogic = 0,
            InverterLogic = 1
        };
    };

    template<>
    struct RegisterValues<A4963RegisterNames::PercentFastDecay> {
        static inline constexpr bool isRanged = false;
        static constexpr auto mask = RegisterMask::PercentFastDecayAddress;
        static constexpr auto code = RegisterCodes::Config1;
        static constexpr std::string_view name = "Percent Fast Decay";

        enum class values : uint16_t {
            T12_5Percent = 0,
            T25Percent = 1
        };
    };

    template<>
    struct RegisterValues<A4963RegisterNames::PositionControllerIntegralGain> {
        static inline constexpr bool isRanged = true;
        static constexpr auto mask = RegisterMask::PositionControllerIntegralGain;
        static constexpr auto code = RegisterCodes::Config3;
        static constexpr auto min = 1.0/128;
        static constexpr auto max = 256.0;
        static constexpr auto functor = [](auto t1) { return std::exp2(t1 - 7); };
        static constexpr auto inverse_functor = [](auto t1) { return static_cast<ssize_t>(std::log2(t1) + 7); };
        static constexpr NewUnitScale<min, max, functor, inverse_functor> value{};
        static constexpr std::string_view name = "Position Controller Integral Gain";
    };

    template<>
    struct RegisterValues<A4963RegisterNames::OverspeedLimitRatio> {
        static inline constexpr bool isRanged = false;
        static constexpr auto mask = RegisterMask::OverspeedLimitRatio;
        static constexpr auto code = RegisterCodes::Config2;
        static constexpr std::string_view name = "Overspeed Limit Ratio";

        enum class values : uint16_t {
            T100Percent = 0,
            T125Percent = 1,
            T150Percent = 2,
            T200Percent = 3
        };
    };

    template<>
    struct RegisterValues<A4963RegisterNames::DegaussCompensation> {
        static inline constexpr bool isRanged = false;
        static constexpr auto mask = RegisterMask::DegaussCompensation;
        static constexpr auto code = RegisterCodes::Config2;
        static constexpr std::string_view name = "Degauss Compensation";

        enum class values : uint16_t {
            Off = 0,
            Active = 1
        };
    };

    template<>
    struct RegisterValues<A4963RegisterNames::FixedPeriod> {
        static inline constexpr bool isRanged = true;
        static constexpr auto mask = RegisterMask::FixedPeriod;
        static constexpr auto code = RegisterCodes::Config2;
        static constexpr auto min = 20.0us;
        static constexpr auto max = 96.6us;
        static constexpr auto functor = [](auto t1) { return min+(t1*1.6us); };
        static constexpr auto inverse_functor = [](auto t1) { return static_cast<ssize_t>((t1 -min)/1.6us); };
        static constexpr NewUnitScale<min, max, functor, inverse_functor> value{};
        static constexpr std::string_view name = "Fixed Period";
    };

    template<>
    struct RegisterValues<A4963RegisterNames::PWMDutyCycleHoldTorque> {
        static inline constexpr bool isRanged = true;
        static constexpr auto mask = RegisterMask::PWMDutyCycleHoldTorque;
        static constexpr auto code = RegisterCodes::Config3;
        static constexpr auto min = 6.25_perc;
        static constexpr auto max = 100.0_perc;
        static constexpr auto functor = [](auto t1) { return (t1+1)*min; };
        static constexpr auto inverse_functor = [](auto t1) { return static_cast<ssize_t>((t1/min).getPercent()-1); };
        static constexpr NewUnitScale<min, max, functor, inverse_functor> value{};
        static constexpr std::string_view name = "PWM Duty Cycle Hold Torque";
    };

    template<>
    struct RegisterValues<A4963RegisterNames::HoldTime> {
        static inline constexpr bool isRanged = true;
        static constexpr auto mask = RegisterMask::HoldTime;
        static constexpr auto code = RegisterCodes::Config3;
        static constexpr auto min = 0ms;
        static constexpr auto max = 120ms;
        static constexpr auto functor = [](auto t1) { return t1*8ms; };
        static constexpr auto inverse_functor = [](auto t1) { return static_cast<ssize_t>(t1/8ms); };
        static constexpr NewUnitScale<min, max, functor, inverse_functor> value{};
        static constexpr std::string_view name = "Hold Time";
    };

    template<>
    struct RegisterValues<A4963RegisterNames::PIControllerProportionalGain> {
        static inline constexpr bool isRanged = true;
        static constexpr auto mask = RegisterMask::PIControllerProportionalGain;
        static constexpr auto code = RegisterCodes::Config4;
        static constexpr auto min = 1/128.0;
        static constexpr auto max = 256.0;
        static constexpr auto functor = [](auto t1) { return std::exp2(t1 - 7); };
        static constexpr auto inverse_functor = [](auto t1) { return static_cast<ssize_t>(std::log2(t1) + 7); };
        static constexpr NewUnitScale<min, max, functor, inverse_functor> value{};
        static constexpr std::string_view name = "PI Controller Proportional Gain";
    };

    template<>
    struct RegisterValues<A4963RegisterNames::PWMDutyCycleTorqueStartup> {
        static inline constexpr bool isRanged = true;
        static constexpr auto mask = RegisterMask::PWMDutyCycleTorqueStartup;
        static constexpr auto code = RegisterCodes::Config4;
        static constexpr auto min = 6.25_perc;
        static constexpr auto max = 100.0_perc;
        static constexpr auto functor = [](auto t1) { return (t1+1)*min; };
        static constexpr auto inverse_functor = [](auto t1) { return static_cast<ssize_t>((t1/min).getPercent()-1); };
        static constexpr NewUnitScale<min, max, functor, inverse_functor> value{};
        static constexpr std::string_view name = "PWM Duty Cycle Torque Startup";
    };

    template<>
    struct RegisterValues<A4963RegisterNames::StartSpeed> {
        static inline constexpr bool isRanged = true;
        static constexpr auto mask = RegisterMask::StartSpeed;
        static constexpr auto code = RegisterCodes::Config4;
        static constexpr auto min = 2_Hz;
        static constexpr auto max = 32_Hz;
        static constexpr auto functor = [](auto t1) { return CustomDataTypes::Frequency::hertz(t1+1) * min; };
        static constexpr auto inverse_functor = [](auto t1) { return static_cast<ssize_t>((t1/min)-1_Hz); };
        static constexpr NewUnitScale<min, max, functor, inverse_functor> value{};
        static constexpr std::string_view name = "Start Speed";
    };

    template<>
    struct RegisterValues<A4963RegisterNames::PIControllerIntegralGain> {
        static inline constexpr bool isRanged = true;
        static constexpr auto mask = RegisterMask::PIControllerIntegralGain;
        static constexpr auto code = RegisterCodes::Config5;
        static constexpr auto min = 1/128.0;
        static constexpr auto max = 256.0;
        static constexpr auto functor = [](auto t1) { return std::exp2(t1 - 7); };
        static constexpr auto inverse_functor = [](auto t1) { return static_cast<ssize_t>(std::log2(t1) + 7); };
        static constexpr NewUnitScale<min, max, functor, inverse_functor> value{};
        static constexpr std::string_view name = "PI Controller Integral Gain";
    };

    template<>
    struct RegisterValues<A4963RegisterNames::SpeedOutputSelection> {
        static inline constexpr bool isRanged = false;
        static constexpr auto mask = RegisterMask::SpeedOutputSelection;
        static constexpr auto code = RegisterCodes::Config5;
        static constexpr std::string_view name = "Speed Output Selection";

        enum class values : uint16_t {
            ElectricalFrequenzy = 0,
            CommutationFrequenzy = 1
        };
    };

    template<>
    struct RegisterValues<A4963RegisterNames::MaximumSpeedSetting> {
        static inline constexpr bool isRanged = true;
        static constexpr auto mask = RegisterMask::MaximumSpeedSetting;
        static constexpr auto code = RegisterCodes::Config5;
        static constexpr auto min = 25.5_Hz;
        static constexpr auto max = 3.2767_kHz;
        static constexpr auto functor = [](auto t1) { return (std::exp2(8+t1)-1)*0.1; };
        static constexpr auto inverse_functor = [](auto t1) noexcept { return static_cast<ssize_t>(std::log2(static_cast<CustomDataTypes::Frequency::Hertz<long double>>(t1*10+1_Hz).count())-8 ); };
        static constexpr NewUnitScale<min, max, functor, inverse_functor> value{};
        static constexpr std::string_view name = "Maximum Speed Setting";
    };

    template<>
    struct RegisterValues<A4963RegisterNames::PhaseAdvance> {
        static inline constexpr bool isRanged = true;
        static constexpr auto mask = RegisterMask::PhaseAdvance;
        static constexpr auto code = RegisterCodes::Config5;
        static constexpr auto min = 0.0;
        static constexpr auto max = 28.125;
        static constexpr auto functor = [](auto t1) { return t1*1.875; }; //TODO: 1.875 Phase
        static constexpr auto inverse_functor = [](auto t1) { return static_cast<ssize_t>(t1 / 1.875); };
        static constexpr NewUnitScale<min, max, functor, inverse_functor> value{};
        static constexpr std::string_view name = "Phase Advance";
    };

    template<>
    struct RegisterValues<A4963RegisterNames::MotorControlMode> {
        static inline constexpr bool isRanged = false;
        static constexpr auto mask = RegisterMask::MotorControlMode;
        static constexpr auto code = RegisterCodes::Run;
        static constexpr std::string_view name = "Motor Control Mode";

        enum class values : uint16_t {
            IndirectSpeed = 0b00,
            DirectSpeed = 0b01,
            ClosedLoopCurrent = 0b10,
            ClosedLoopSpeed = 0b11
        };
    };

    template<>
    struct RegisterValues<A4963RegisterNames::EnableStopOnFail> {
        static inline constexpr bool isRanged = false;
        static constexpr auto mask = RegisterMask::EnableStopOnFail;
        static constexpr auto code = RegisterCodes::Run;
        static constexpr std::string_view name = "Enable Stop On Fail";

        enum class values : uint16_t {
            NoStopOnFail = 0,
            StopOnFail = 1,
        };
    };

    template<>
    struct RegisterValues<A4963RegisterNames::DutyCycleControl> {
        static inline constexpr bool isRanged = true;
        static constexpr auto mask = RegisterMask::DutyCycleControl;
        static constexpr auto code = RegisterCodes::Run;
        static constexpr auto min = 10.0_perc;
        static constexpr auto max = 100.0_perc;
        static constexpr auto functor = [](auto t1) { return 7.0_perc+CustomDataTypes::Percentage(t1*3.0); };
        static constexpr auto inverse_functor = [](auto t1) { return static_cast<ssize_t>((t1 -7.0_perc)/3.0); };
        static constexpr NewUnitScale<min, max, functor, inverse_functor> value{};
        static constexpr std::string_view name = "Duty Cycle Control";
    };

    template<>
    struct RegisterValues<A4963RegisterNames::RestartControl> {
        static inline constexpr bool isRanged = false;
        static constexpr auto mask = RegisterMask::RestartControl;
        static constexpr auto code = RegisterCodes::Run;
        static constexpr std::string_view name = "Restart Control";

        enum class values : uint16_t {
            NoRestart = 0,
            RestartAfterLossOfSync = 1
        };
    };

    template<>
    struct RegisterValues<A4963RegisterNames::Brake> {
        static inline constexpr bool isRanged = false;
        static constexpr auto mask = RegisterMask::Brake;
        static constexpr auto code = RegisterCodes::Run;
        static constexpr std::string_view name = "Brake";

        enum class values : uint16_t {
            BrakeDisabled = 0,
            EnableIfPWMDisabled = 1
        };
    };

    template<>
    struct RegisterValues<A4963RegisterNames::DirectionOfRotation> {
        static inline constexpr bool isRanged = false;
        static constexpr auto mask = RegisterMask::DirectionOfRotation;
        static constexpr auto code = RegisterCodes::Run;
        static constexpr std::string_view name = "Direction Of Rotation";

        enum class values : uint16_t {
            Forward = 0,
            Reverse = 1
        };
    };

    template<>
    struct RegisterValues<A4963RegisterNames::Run> {
        static inline constexpr bool isRanged = false;
        static constexpr auto mask = RegisterMask::Run;
        static constexpr auto code = RegisterCodes::Run;
        static constexpr std::string_view name = "Run";

        enum class values : uint16_t {
            DisableOutputCoastMotor = 0,
            StartAndRunMotor = 1
        };
    };
}

#ifdef debug
    #pragma GCC diagnostic pop
#endif