//
// Created by sebastian on 23.10.18.
//

#pragma once

#include <chrono>
#include <variant>
#include <fstream>
#include <regex>
#include "../../CustomDataTypes/SIUnit.h"
#include "../../CustomDataTypes/Volt.h"
#include "../../CustomDataTypes/Hertz.h"
#include "../../CustomDataTypes/Percentage.h"
#include "../../../inc/json.h"
#include "A4963.h"
#include "A4963RegisterInfo.h"

namespace NS_A4963 {
    using nlohmann::json;

    static auto A4963NamesMap = std::map<std::string_view, A4963RegisterNames>{
            {RegisterValues<A4963RegisterNames::BlankTime>::name, A4963RegisterNames::BlankTime},
            {RegisterValues<A4963RegisterNames::DeadTime>::name, A4963RegisterNames::DeadTime},
            {RegisterValues<A4963RegisterNames::CurrentSenseThresholdVoltage>::name, A4963RegisterNames::CurrentSenseThresholdVoltage},
            {RegisterValues<A4963RegisterNames::VDSThreshold>::name, A4963RegisterNames::VDSThreshold},
            {RegisterValues<A4963RegisterNames::PositionControllerProportionalGain>::name, A4963RegisterNames::PositionControllerProportionalGain},
            {RegisterValues<A4963RegisterNames::RecirculationMode>::name, A4963RegisterNames::RecirculationMode},
            {RegisterValues<A4963RegisterNames::PercentFastDecay>::name, A4963RegisterNames::PercentFastDecay},
            {RegisterValues<A4963RegisterNames::InvertPWMInput>::name, A4963RegisterNames::InvertPWMInput},
            {RegisterValues<A4963RegisterNames::BemfTimeQualifier>::name, A4963RegisterNames::BemfTimeQualifier},
            {RegisterValues<A4963RegisterNames::OverspeedLimitRatio>::name, A4963RegisterNames::OverspeedLimitRatio},
            {RegisterValues<A4963RegisterNames::DegaussCompensation>::name, A4963RegisterNames::DegaussCompensation},
            {RegisterValues<A4963RegisterNames::FixedPeriod>::name, A4963RegisterNames::FixedPeriod},
            {RegisterValues<A4963RegisterNames::PositionControllerIntegralGain>::name, A4963RegisterNames::PositionControllerIntegralGain},
            {RegisterValues<A4963RegisterNames::PWMDutyCycleHoldTorque>::name, A4963RegisterNames::PWMDutyCycleHoldTorque},
            {RegisterValues<A4963RegisterNames::HoldTime>::name, A4963RegisterNames::HoldTime},
            {RegisterValues<A4963RegisterNames::PIControllerProportionalGain>::name, A4963RegisterNames::PIControllerProportionalGain},
            {RegisterValues<A4963RegisterNames::PWMDutyCycleTorqueStartup>::name, A4963RegisterNames::PWMDutyCycleTorqueStartup},
            {RegisterValues<A4963RegisterNames::StartSpeed>::name, A4963RegisterNames::StartSpeed},
            {RegisterValues<A4963RegisterNames::PIControllerIntegralGain>::name, A4963RegisterNames::PIControllerIntegralGain},
            {RegisterValues<A4963RegisterNames::SpeedOutputSelection>::name, A4963RegisterNames::SpeedOutputSelection},
            {RegisterValues<A4963RegisterNames::MaximumSpeedSetting>::name, A4963RegisterNames::MaximumSpeedSetting},
            {RegisterValues<A4963RegisterNames::PhaseAdvance>::name, A4963RegisterNames::PhaseAdvance},
            {RegisterValues<A4963RegisterNames::MotorControlMode>::name, A4963RegisterNames::MotorControlMode},
            {RegisterValues<A4963RegisterNames::EnableStopOnFail>::name, A4963RegisterNames::EnableStopOnFail},
            {RegisterValues<A4963RegisterNames::DutyCycleControl>::name, A4963RegisterNames::DutyCycleControl},
            {RegisterValues<A4963RegisterNames::RestartControl>::name, A4963RegisterNames::RestartControl},
            {RegisterValues<A4963RegisterNames::Brake>::name, A4963RegisterNames::Brake},
            {RegisterValues<A4963RegisterNames::DirectionOfRotation>::name, A4963RegisterNames::DirectionOfRotation},
            {RegisterValues<A4963RegisterNames::Run>::name, A4963RegisterNames::Run}
    };


    static auto A4963MasksMap = std::map<std::string_view, Masks>{
            {"Temperatur Warning", Masks::TemperatureWarning},
            {"Loss Of Bemf Synchronitation", Masks::LossOfBemfSynchronization},
            {"Phase A High Side VDS", Masks::PhaseAHighSideVDS},
            {"Phase B High Side VDS", Masks::PhaseBHighSideVDS},
            {"Phase C High Side VDS", Masks::PhaseCHighSideVDS},
            {"VBB Undervoltage", Masks::VBBUndervoltage},
    };



    std::pair<std::intmax_t, std::intmax_t> getRatio(const char prefix) {
        switch (prefix) {
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
    std::variant<CustomDataTypes::Frequency::Hertz<Rep, Period>, CustomDataTypes::Electricity::Volt<Rep, Period>, std::chrono::duration<Rep, Period>>
    getType(const char prefix, const std::string &unit, Rep value) {
        auto prefixRatio = getRatio(prefix);
        auto newValue = (value * prefixRatio.first * Period::den) / (prefixRatio.second * Period::num);
        if (unit == "V") {
            return CustomDataTypes::Electricity::Volt<Rep, Period>{newValue};
        } else if (unit == "Hz") {
            return CustomDataTypes::Frequency::Hertz<Rep, Period>{newValue};
        } else if (unit == "s") {
            return std::chrono::duration<Rep, Period>{newValue};
        } else {
            throw std::runtime_error("Unknown Type: " + unit);
        }
    }

    template<typename Rep>
    std::variant<Rep, CustomDataTypes::Percentage<Rep>> getType(const std::string &unit, Rep value) {
        if (unit == "%") {
            return CustomDataTypes::Percentage{value};
        } else {
            return value;
        }
    }

    template<typename T, A4963RegisterNames N>
    static bool setIfPeriodic(A4963 &device, double data, const char prefix, const std::string &unit) {
        using Rep = typename utils::periodic_info<T>::rep;
        using Period = typename utils::periodic_info<T>::period;
        try {
            auto d = T{std::get<T>(
                    getType<Rep, typename utils::periodic_info<T>::period>(prefix, unit, static_cast<Rep>(data)))};
            if(device.set<N>(d)) {
                return true;
            }
            return false;
        }
        catch(std::exception& e){
            std::cerr << "Invalid Unit \"" << prefix << unit << "\" in register \"" << RegisterValues<N>::name <<
                        "\", did you mean \"" << utils::ratio_lookup<Period>::abr_value << utils::periodic_printable<T>::name << "\" ?" << std::endl;
            return false;
        }
    }

    template<typename T, A4963RegisterNames N>
    static void setIfNotPeriodic(A4963 &device, double data) {
        if constexpr(!std::is_arithmetic_v<T>) {
            auto d = T{static_cast<typename T::value_type>(data)};
            device.set<N>(d);
        } else {
            device.set<N>(static_cast<T>(data));
        }

    }


    template<A4963RegisterNames N>
    void setRuntime(A4963 &device, A4963RegisterNames toSet, char prefix, const std::string &unit, double data) {
        if (toSet == N) {
            if constexpr(RegisterValues<N>::isRanged) {
                using type = std::remove_const_t<decltype(RegisterValues<N>::min)>;
                if constexpr(utils::is_periodic<type>::value) {
                    if(setIfPeriodic<type, N>(device, data, prefix, unit)) {
                        std::cout << "Set the register " << RegisterValues<N>::name << " to " << data << prefix << unit << std::endl;
                    }
                } else {
                    if(prefix != '\0') {
                        std::cerr << "This register (" << RegisterValues<N>::name << ") doesn't expect a prefix. This might be an error!" << std::endl;
                    }
                    if(std::is_arithmetic_v<type> && !unit.empty()) {
                            std::cerr << "This register (" << RegisterValues<N>::name << ") doesn't expect an unit. This might be an error!" << std::endl;
                    }
                    setIfNotPeriodic<type, N>(device, data);
                    std::cout << "Set the register " << RegisterValues<N>::name << " to " << data << prefix << unit << std::endl;
                }
            } else {
                auto d = static_cast<typename RegisterValues<N>::values>(data);
                device.set<N>(d);
            }
        } else {
            setRuntime<static_cast<A4963RegisterNames>(static_cast<uint8_t>(N) + 1)>(device, toSet, prefix, unit, data);
        }
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

    template<>
    void setRuntime<A4963RegisterNames::Run>(A4963 &device, A4963RegisterNames toSet, const char prefix, const std::string &unit, double data) {
        auto d = static_cast<typename RegisterValues<A4963RegisterNames::Run>::values>(data);
        device.set<A4963RegisterNames::Run>(d);
    }

#pragma GCC diagnostic pop

    template<typename T = void>
    void setRuntime(A4963 &device, A4963RegisterNames toSet, const char prefix, const std::string &unit, double data) {
        setRuntime<static_cast<A4963RegisterNames>(0)>(device, toSet, prefix, unit, data);
    }

    class RegisterStrings {
    private:
        template<A4963RegisterNames reg>
        static constexpr std::pair<std::string_view, A4963RegisterNames> reg_pair() {
            return std::pair<std::string_view, A4963RegisterNames>{RegisterValues<reg>::name, reg};
        }

        static const std::map<std::string_view, A4963RegisterNames> values;
    public:
        static A4963RegisterNames get(const std::string_view& string) {
            return values.at(string);
        }
    };

    class JsonSetter {

    private:
    public:
        json j;
        JsonSetter(A4963 &device, const std::string& str);
    };

}