//
// Created by sebastian on 23.10.18.
//
#include "DeserializeA4963.h"

namespace NS_A4963 {

    const std::map<std::string_view, A4963RegisterNames> RegisterStrings::values = {
            RegisterStrings::reg_pair<A4963RegisterNames::BlankTime>(),
            RegisterStrings::reg_pair<A4963RegisterNames::DeadTime>(),
            RegisterStrings::reg_pair<A4963RegisterNames::CurrentSenseThresholdVoltage>(),
            RegisterStrings::reg_pair<A4963RegisterNames::VDSThreshold>(),
            RegisterStrings::reg_pair<A4963RegisterNames::PositionControllerProportionalGain>(),
            RegisterStrings::reg_pair<A4963RegisterNames::RecirculationMode>(),
            RegisterStrings::reg_pair<A4963RegisterNames::PercentFastDecay>(),
            RegisterStrings::reg_pair<A4963RegisterNames::InvertPWMInput>(),
            RegisterStrings::reg_pair<A4963RegisterNames::BemfTimeQualifier>(),
            RegisterStrings::reg_pair<A4963RegisterNames::OverspeedLimitRatio>(),
            RegisterStrings::reg_pair<A4963RegisterNames::DegaussCompensation>(),
            RegisterStrings::reg_pair<A4963RegisterNames::FixedPeriod>(),
            RegisterStrings::reg_pair<A4963RegisterNames::PositionControllerIntegralGain>(),
            RegisterStrings::reg_pair<A4963RegisterNames::PWMDutyCycleHoldTorque>(),
            RegisterStrings::reg_pair<A4963RegisterNames::HoldTime>(),
            RegisterStrings::reg_pair<A4963RegisterNames::PIControllerProportionalGain>(),
            RegisterStrings::reg_pair<A4963RegisterNames::PWMDutyCycleTorqueStartup>(),
            RegisterStrings::reg_pair<A4963RegisterNames::StartSpeed>(),
            RegisterStrings::reg_pair<A4963RegisterNames::PIControllerIntegralGain>(),
            RegisterStrings::reg_pair<A4963RegisterNames::SpeedOutputSelection>(),
            RegisterStrings::reg_pair<A4963RegisterNames::MaximumSpeedSetting>(),
            RegisterStrings::reg_pair<A4963RegisterNames::PhaseAdvance>(),
            RegisterStrings::reg_pair<A4963RegisterNames::MotorControlMode>(),
            RegisterStrings::reg_pair<A4963RegisterNames::EnableStopOnFail>(),
            RegisterStrings::reg_pair<A4963RegisterNames::DutyCycleControl>(),
            RegisterStrings::reg_pair<A4963RegisterNames::RestartControl>(),
            RegisterStrings::reg_pair<A4963RegisterNames::Brake>(),
            RegisterStrings::reg_pair<A4963RegisterNames::DirectionOfRotation>(),
            RegisterStrings::reg_pair<A4963RegisterNames::Run>()
    };

    JsonSetter::JsonSetter(const std::string& str) {
        std::ifstream i(str);
        i >> j;

        auto config = j["config"].items();
        for (const auto &element : config) {
            for(const auto &el : element.value().items()) {
                const auto &registerName = el.key();
                auto registerValue =  el.value().get<std::string>();

                auto val = RegisterStrings::get(registerName);
            }
        }
    }
}