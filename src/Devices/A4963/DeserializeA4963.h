//
// Created by sebastian on 23.10.18.
//

#pragma once

#include <chrono>
#include <variant>
#include "../../CustomDataTypes/SIUnit.h"
#include "../../CustomDataTypes/Volt.h"
#include "../../CustomDataTypes/Hertz.h"
#include "../../CustomDataTypes/Percentage.h"
#include "A4963.h"
#include "A4963RegisterInfo.h"

namespace NS_A4963 {

    std::pair<std::intmax_t, std::intmax_t> getRatio(const char prefix) {
        switch(prefix) {
            case 'a':
                return {1, 1000000000000000000};
            case 'f':
                return {1, 1000000000000000};
            case 'p':
                return {1, 1000000000000};
            case 'n':
                return {1, 1000000000};
            case 'u':
                return {1, 1000000};
            case 'm':
                return {1, 1000};
            case 'c':
                return {1, 100};
            case 'd':
                return {1, 10};
            case '\0':
                return {1, 1};
            case 'D':
                return {10, 1};
            case 'H':
                return {100, 1};
            case 'k':
                return {1000, 1};
            case 'M':
                return {1000000, 1};
            case 'G':
                return {1000000000, 1};
            case 'T':
                return {1000000000000, 1};
            case 'E':
                return {1000000000000000, 1};
            default:
                throw std::runtime_error("Unknown Type!");
        }
    }

    template<typename Rep, typename Period = std::ratio<1, 1>>
    std::variant<CustomDataTypes::Frequency::Hertz<Rep, Period>, CustomDataTypes::Electricity::Volt<Rep, Period>, std::chrono::duration<Rep, Period>> getType(const char prefix, const std::string& unit, Rep& value) {
        auto prefixRatio = getRatio(prefix);
        auto newValue = (value * prefixRatio.first * Period::num) / (prefixRatio.second * Period::den);
        if(unit == "v") {
            return CustomDataTypes::Electricity::Volt<Rep, Period>{newValue};
        } else if(unit == "hz") {
            return CustomDataTypes::Frequency::Hertz<Rep, Period>{newValue};
        } else if(unit == "s") {
            return std::chrono::duration<Rep, Period>{newValue};
        } else {
            throw std::runtime_error("Unknown Type: " + unit);
        }
    }

    template<typename Rep>
    std::variant<Rep, CustomDataTypes::Percentage<Rep>> getType(const std::string& unit, Rep& value) {
        if(unit == "%") {
            return CustomDataTypes::Percentage{value};
        } else {
            return value;
        }
    }

    void setRuntime(A4963& device, A4963RegisterNames toSet, uint16_t data){
        switch(toSet){
            case A4963RegisterNames::PhaseAdvance: {
                auto d = decltype(RegisterValues<A4963RegisterNames::PhaseAdvance>::min){static_cast<double>(data)};
                device.set<A4963RegisterNames::PhaseAdvance>(d);
                break;
            }
            case A4963RegisterNames::BlankTime:{
                auto d = decltype(RegisterValues<A4963RegisterNames::BlankTime>::min){data};
                device.set<A4963RegisterNames::BlankTime>(d);
                break;
            };
            case A4963RegisterNames::DeadTime:{
                auto d = decltype(RegisterValues<A4963RegisterNames::DeadTime>::min){data};
                device.set<A4963RegisterNames::DeadTime>(d);
                break;
            };
            case A4963RegisterNames::CurrentSenseThresholdVoltage:{
                auto d = decltype(RegisterValues<A4963RegisterNames::CurrentSenseThresholdVoltage>::min){static_cast<double>(data)};
                device.set<A4963RegisterNames::CurrentSenseThresholdVoltage>(d);
                break;
            };
            case A4963RegisterNames::VDSThreshold:{
                auto d = decltype(RegisterValues<A4963RegisterNames::VDSThreshold>::min){static_cast<double>(data)};
                device.set<A4963RegisterNames::VDSThreshold>(d);
                break;
            };
            case A4963RegisterNames::PositionControllerProportionalGain:{
                auto d = decltype(RegisterValues<A4963RegisterNames::PositionControllerProportionalGain>::min){static_cast<double>(data)};
                device.set<A4963RegisterNames::PositionControllerProportionalGain>(d);
                break;
            };
            case A4963RegisterNames::RecirculationMode:{
                auto d = static_cast<RegisterValues<A4963RegisterNames::RecirculationMode>::values>(data);
                device.set<A4963RegisterNames::RecirculationMode>(d);
                break;
            };
            case A4963RegisterNames::PercentFastDecay:{
                auto d = static_cast<RegisterValues<A4963RegisterNames::PercentFastDecay>::values>(data);
                device.set<A4963RegisterNames::PercentFastDecay>(d);
                break;
            };
            case A4963RegisterNames::InvertPWMInput:{
                auto d = static_cast<RegisterValues<A4963RegisterNames::InvertPWMInput>::values>(data);
                device.set<A4963RegisterNames::InvertPWMInput>(d);
                break;
            };
            case A4963RegisterNames::BemfTimeQualifier:{
                auto d = static_cast<RegisterValues<A4963RegisterNames::BemfTimeQualifier>::values>(data);
                device.set<A4963RegisterNames::BemfTimeQualifier>(d);
                break;
            };
            case A4963RegisterNames::OverspeedLimitRatio:{
                auto d = static_cast<RegisterValues<A4963RegisterNames::OverspeedLimitRatio>::values>(data);
                device.set<A4963RegisterNames::OverspeedLimitRatio>(d);
                break;
            };
            case A4963RegisterNames::DegaussCompensation:{
                auto d = static_cast<RegisterValues<A4963RegisterNames::DegaussCompensation>::values>(data);
                device.set<A4963RegisterNames::DegaussCompensation>(d);
                break;
            };
            case A4963RegisterNames::FixedPeriod:{
                auto d = decltype(RegisterValues<A4963RegisterNames::FixedPeriod>::min){data};
                device.set<A4963RegisterNames::FixedPeriod>(d);
                break;
            };
            case A4963RegisterNames::PositionControllerIntegralGain:{
                auto d = decltype(RegisterValues<A4963RegisterNames::PositionControllerIntegralGain>::min){static_cast<double>(data)};
                device.set<A4963RegisterNames::PositionControllerIntegralGain>(d);
                break;
            };
            case A4963RegisterNames::PWMDutyCycleHoldTorque:{
                auto d = decltype(RegisterValues<A4963RegisterNames::PWMDutyCycleHoldTorque>::min){static_cast<double>(data)};
                device.set<A4963RegisterNames::PWMDutyCycleHoldTorque>(d);
                break;
            };
            case A4963RegisterNames::HoldTime:{
                auto d = decltype(RegisterValues<A4963RegisterNames::HoldTime>::min){data};
                device.set<A4963RegisterNames::HoldTime>(d);
                break;
            };
            case A4963RegisterNames::PIControllerProportionalGain:{
                auto d = decltype(RegisterValues<A4963RegisterNames::PIControllerProportionalGain>::min){static_cast<double>(data)};
                device.set<A4963RegisterNames::PIControllerProportionalGain>(d);
                break;
            };
            case A4963RegisterNames::PWMDutyCycleTorqueStartup:{
                auto d = decltype(RegisterValues<A4963RegisterNames::PWMDutyCycleTorqueStartup>::min){static_cast<double>(data)};
                device.set<A4963RegisterNames::PWMDutyCycleTorqueStartup>(d);
                break;
            };
            case A4963RegisterNames::StartSpeed:{
                auto d = decltype(RegisterValues<A4963RegisterNames::StartSpeed>::min){data};
                device.set<A4963RegisterNames::StartSpeed>(d);
                break;
            };
            case A4963RegisterNames::PIControllerIntegralGain:{
                auto d = decltype(RegisterValues<A4963RegisterNames::PIControllerIntegralGain>::min){static_cast<double>(data)};
                device.set<A4963RegisterNames::PIControllerIntegralGain>(d);
                break;
            };
            case A4963RegisterNames::SpeedOutputSelection:{
                auto d = static_cast<RegisterValues<A4963RegisterNames::SpeedOutputSelection>::values>(data);
                device.set<A4963RegisterNames::SpeedOutputSelection>(d);
                break;
            };
            case A4963RegisterNames::MaximumSpeedSetting:{
                auto d = decltype(RegisterValues<A4963RegisterNames::MaximumSpeedSetting>::min){static_cast<double>(data)};
                device.set<A4963RegisterNames::MaximumSpeedSetting>(d);
                break;
            };
            case A4963RegisterNames::MotorControlMode:{
                auto d = static_cast<RegisterValues<A4963RegisterNames::MotorControlMode>::values>(data);
                device.set<A4963RegisterNames::MotorControlMode>(d);
                break;
            };
            case A4963RegisterNames::EnableStopOnFail:{
                auto d = static_cast<RegisterValues<A4963RegisterNames::EnableStopOnFail>::values>(data);
                device.set<A4963RegisterNames::EnableStopOnFail>(d);
                break;
            };
            case A4963RegisterNames::DutyCycleControl:{
                auto d = decltype(RegisterValues<A4963RegisterNames::DutyCycleControl>::min){static_cast<double>(data)};
                device.set<A4963RegisterNames::DutyCycleControl>(d);
                break;
            };
            case A4963RegisterNames::RestartControl:{
                auto d = static_cast<RegisterValues<A4963RegisterNames::RestartControl>::values>(data);
                device.set<A4963RegisterNames::RestartControl>(d);
                break;
            };
            case A4963RegisterNames::Brake:{
                auto d = static_cast<RegisterValues<A4963RegisterNames::Brake>::values>(data);
                device.set<A4963RegisterNames::Brake>(d);
                break;
            };
            case A4963RegisterNames::DirectionOfRotation:{
                auto d = static_cast<RegisterValues<A4963RegisterNames::DirectionOfRotation>::values>(data);
                device.set<A4963RegisterNames::DirectionOfRotation>(d);
                break;
            };
            case A4963RegisterNames::Run:{
                auto d = static_cast<RegisterValues<A4963RegisterNames::Run>::values>(data);
                device.set<A4963RegisterNames::Run>(d);
                break;
            };
        }
    }
}