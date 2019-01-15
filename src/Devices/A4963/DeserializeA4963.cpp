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

    JsonSetter::JsonSetter(A4963 &device, const std::string& str) {
        std::ifstream i(str);
        i >> j;

        auto config = j["config"].items();
        for (const auto &element : config) {
            for(const auto &el : element.value().items()) {
                const auto &registerName = el.key();
                auto registerValue = el.value().get<std::string>();

                if(registerName != "Duty Cycle Control" || (registerName != "Duty Cycle Control" && registerValue == "0")) {
                    A4963RegisterNames val;
                    try {
                        val = RegisterStrings::get(registerName);
                    } catch (std::exception &e) {
                        std::cerr << "Unknown Register Value: " << registerName << std::endl;
                        continue;
                    }

                    setRuntime(device, val, registerValue);
                } else {
                    device.turnOffDutyCycle();
                }
            }
        }

        auto mask = j["mask"];
        for (auto &it : mask) {
            for (auto it1 = it.begin(); it1 != it.end(); ++it1) {
                device.configDiagnostic(NS_A4963::A4963MasksMap.at(it1.key()),it1.value());
                std::cout << "set: " << it1.key() << " to: " << (it1.value() ? " On " : " Off ") << std::endl;
            }
        }
        device.commit();
    }

    JsonSetter::UnitInfo JsonSetter::parseData(const std::string &registerValue) {
        static std::regex unit_regex(
                R"(([-+]?[0-9]*\.?[0-9]+)\s?(a|f|p|n|u|m|c|d|D|h|k|M|T|P|E)?([A-Z0-9a-z!"#$%&'()*+,.\/:;<=>?@\[\] ^_`{|}~-]+)?)",
                                    std::regex_constants::ECMAScript);

        std::smatch unit_match;
        long double unit_val = 0.0;
        char prefix = '\0';
        bool success = true;
        std::string unit;

        if (std::regex_search(registerValue, unit_match, unit_regex)) {
            for (size_t i = 0; i < unit_match.size(); ++i) {
                switch (i) {
                    case 1:
                        if (unit_match[i].length() > 0) {
                            unit_val = std::atof(unit_match[i].str().data());
                        } else {
                            success = false;
                        }
                        break;
                    case 2:
                        if (unit_match[i].length() > 0) {
                            prefix = unit_match[i].str().at(0);
                        }
                        break;
                    case 3:
                        if (unit_match[i].length() > 0) {
                            unit = unit_match[i].str();
                        }
                        break;
                    default:
                        break;
                }
            }
            return UnitInfo{unit_val, prefix, unit, success};
        } else {
            UnitInfo failed;
            failed.succsess = false;
            return failed;
        }
    }
}