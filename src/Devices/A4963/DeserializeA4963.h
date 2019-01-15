//
// Created by sebastian on 23.10.18.
//

#pragma once

#include <chrono>
#include <variant>
#include <fstream>
#include <regex>
#include <iomanip>
#include "../../CustomDataTypes/SIUnit.h"
#include "../../CustomDataTypes/Volt.h"
#include "../../CustomDataTypes/Hertz.h"
#include "../../CustomDataTypes/Percentage.h"
#include "../../../inc/json.h"
#include "A4963.h"
#include "A4963RegisterInfo.h"

namespace NS_A4963 {
    using nlohmann::json;

    static inline const json defaultValues{
            {"config", 
                {
                    json{{RegisterValues<A4963RegisterNames::BlankTime>::name,"1200ns"}},
                    json{{RegisterValues<A4963RegisterNames::DeadTime>::name,"1000ns"}},
                    json{{RegisterValues<A4963RegisterNames::RecirculationMode>::name,"Off"}},
                    json{{RegisterValues<A4963RegisterNames::PercentFastDecay>::name,"12,5%"}},
                    json{{RegisterValues<A4963RegisterNames::InvertPWMInput>::name,"Normal True Logic"}},
                    json{{RegisterValues<A4963RegisterNames::CurrentSenseThresholdVoltage>::name,"200mV"}},
                    json{{RegisterValues<A4963RegisterNames::BemfTimeQualifier>::name,"Debounce Timer"}},
                    json{{RegisterValues<A4963RegisterNames::VDSThreshold>::name,"1.55V"}},
                    json{{RegisterValues<A4963RegisterNames::PositionControllerProportionalGain>::name, "1"}},
                    json{{RegisterValues<A4963RegisterNames::OverspeedLimitRatio>::name,"150%"}},
                    json{{RegisterValues<A4963RegisterNames::DegaussCompensation>::name,"Off"}},
                    json{{RegisterValues<A4963RegisterNames::FixedPeriod>::name,"50.4us"}},
                    json{{RegisterValues<A4963RegisterNames::PositionControllerIntegralGain>::name,"1"}},
                    json{{RegisterValues<A4963RegisterNames::PWMDutyCycleHoldTorque>::name,"37.5%"}},
                    json{{RegisterValues<A4963RegisterNames::HoldTime>::name,"16ms"}},
                    json{{RegisterValues<A4963RegisterNames::PIControllerProportionalGain>::name,"1"}},
                    json{{RegisterValues<A4963RegisterNames::PWMDutyCycleTorqueStartup>::name,"50%"}},
                    json{{RegisterValues<A4963RegisterNames::StartSpeed>::name,"8Hz"}},
                    json{{RegisterValues<A4963RegisterNames::PIControllerIntegralGain>::name,"1"}},
                    json{{RegisterValues<A4963RegisterNames::SpeedOutputSelection>::name,"Electrical Frequenzy"}},
                    json{{RegisterValues<A4963RegisterNames::MaximumSpeedSetting>::name,"819.1Hz"}},
                    json{{RegisterValues<A4963RegisterNames::PhaseAdvance>::name,"15"}},
                    json{{RegisterValues<A4963RegisterNames::MotorControlMode>::name,"Indirect Speed"}},
                    json{{RegisterValues<A4963RegisterNames::EnableStopOnFail>::name,"No Stop On Fail"}},
                    json{{RegisterValues<A4963RegisterNames::DutyCycleControl>::name,"0"}},
                    json{{RegisterValues<A4963RegisterNames::RestartControl>::name,"Restart After Loss Of Sync"}},
                    json{{RegisterValues<A4963RegisterNames::Brake>::name,"Brake Disabled"}},
                    json{{RegisterValues<A4963RegisterNames::DirectionOfRotation>::name,"Forward"}},
                    json{{RegisterValues<A4963RegisterNames::Run>::name,"Start And Run Motor"}}
                }
            },
            {"mask", 
                {
                    json{{"Temperature Warning",false}},
                    json{{"Overtemperature",false}},
                    json{{"Loss Of Bemf Synchronization",false}},
                    json{{"Phase A High Side VDS",false}},
                    json{{"Phase B High Side VDS",false}},
                    json{{"Phase C High Side VDS",false}},
                    json{{"VBB Undervoltage",false}},
                }
            }
    };

    static auto A4963MasksMap = std::map<std::string_view, Masks>{
            {"Temperature Warning", Masks::TemperatureWarning},
            {"Overtemperature", Masks::OverTemperature},
            {"Loss Of Bemf Synchronization", Masks::LossOfBemfSynchronization},
            {"Phase A High Side VDS", Masks::PhaseAHighSideVDS},
            {"Phase B High Side VDS", Masks::PhaseBHighSideVDS},
            {"Phase C High Side VDS", Masks::PhaseCHighSideVDS},
            {"VBB Undervoltage", Masks::VBBUndervoltage},
    };


    class JsonSetter {

    private:
        json j;
    public:
        struct UnitInfo {
            long double data;
            char prefix;
            std::string unit;
            bool succsess;
        };

        static UnitInfo parseData(const std::string& str);
        JsonSetter(A4963 &device, const std::string& str);

    };


    template<typename Rep, typename Period = std::ratio<1, 1>>
    std::variant<CustomDataTypes::Frequency::Hertz<Rep, Period>, CustomDataTypes::Electricity::Volt<Rep, Period>, std::chrono::duration<Rep, Period>>
    getType(const char prefix, const std::string &unit, Rep value) {
        auto prefixRatio = utils::getRatio(prefix);
        auto newValue = (value * prefixRatio.first * Period::den) / (prefixRatio.second * Period::num);
        if (unit == CustomDataTypes::Electricity::Volt<Rep, Period>::abr_value) {
            return CustomDataTypes::Electricity::Volt<Rep, Period>{newValue};
        } else if (unit == CustomDataTypes::Frequency::Hertz<Rep, Period>::abr_value) {
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
    static auto setIfPeriodic(A4963 &device, double data, const char prefix, const std::string &unit) {
        static_assert(utils::is_periodic<T>::value);
        using Rep = typename utils::periodic_info<T>::rep;
        using Period = typename utils::periodic_info<T>::period;
        try {
            auto d = T{std::get<T>(
                    getType<Rep, typename utils::periodic_info<T>::period>(prefix, unit, static_cast<Rep>(data)))};
            auto return_value = device.set<N>(d);
            if(!return_value) {
                std::cerr << "This error occured in register " << RegisterValues<N>::name << std::endl;
            }
            return return_value;
        }
        catch(std::runtime_error& e){
            std::cerr << "Invalid Unit \"" << prefix << unit << "\" in register \"" << RegisterValues<N>::name <<
                        "\", did you mean \"" << utils::ratio_lookup<Period>::abr_value << utils::periodic_printable<T>::name << "\" ?" << std::endl;
            return std::optional<const T>{};
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

    template<A4963RegisterNames N = static_cast<A4963RegisterNames>(0)>
    void setRuntime(A4963 &device, A4963RegisterNames toSet, const std::string& registerData) {
        using namespace utils::printable;
        if (toSet == N) {
            if constexpr(RegisterValues<N>::isRanged) {
                JsonSetter::UnitInfo unitInfo = JsonSetter::parseData(registerData);
                if(unitInfo.succsess) {
                    auto prefix = unitInfo.prefix;
                    auto unit = unitInfo.unit;
                    auto data = unitInfo.data;

                    using type = std::remove_const_t<decltype(RegisterValues<N>::min)>;
                    if constexpr(utils::is_periodic<type>::value) {
                        if (auto value = setIfPeriodic<type, N>(device, static_cast<double>(data), prefix, unit)) {
                            std::cout << "Set the register " << std::setw(40) << RegisterValues<N>::name << " to "
                                    << std::setw(20) << *value << std::endl;
                        }
                    } else {
                        if (prefix != '\0') {
                            std::cerr << "This register (" << RegisterValues<N>::name
                                      << ") doesn't expect a prefix. This might be an error!" << std::endl;
                        }
                        if constexpr (std::is_arithmetic_v<type>) {
                            if (!unit.empty()) {
                                std::cerr << "This register (" << RegisterValues<N>::name
                                          << ") doesn't expect a unit. This might be an error!" << std::endl;
                            }
                        } else {
                            if(unit != type::abr_value) {
                                std::cerr << "This register (" << RegisterValues<N>::name << ") excpects the type to be \""
                                          << type::abr_value << "\", but is set to "
                                          << "\"" << unit << "\"" << std::endl;
                            }
                        }

                        setIfNotPeriodic<type, N>(device, static_cast<double>(data));
                        std::cout << "Set the register " << std::setw(40) << RegisterValues<N>::name << " to " << std::setw(20)
                                  << data << prefix << unit << std::endl;

                    }
                } else {
                    std::cerr << "There was an error parsing your data at register " << RegisterValues<N>::name << std::endl;
                }
            } else {
                try {
                    auto type = RegisterValues<N>::map.at(registerData.data());
                    device.set<N>(type);
                    std::cout << "Set the register " << std::setw(40) << RegisterValues<N>::name << " to " << std::setw(20) << registerData << std::endl;
                } catch (std::exception& e) {
                    std::cerr << "Unknown value " << registerData << " in register " << RegisterValues<N>::name << std::endl;
                    std::cerr << "Possible values: " << std::endl;
                    for(auto possibleValues : RegisterValues<N>::map) {
                        std::cerr << "\t - " << possibleValues.first << std::endl;
                    }
                }
            }
        } else {
            setRuntime<static_cast<A4963RegisterNames>(static_cast<uint8_t>(N) + 1)>(device, toSet, registerData);
        }
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

    template<>
    inline void setRuntime<A4963RegisterNames::Run>(A4963 &device, A4963RegisterNames toSet, const std::string& registerData) {
        std::cout << "Set the register " << RegisterValues<A4963RegisterNames::Run>::name << " to " << registerData << std::endl;
        auto d = static_cast<typename RegisterValues<A4963RegisterNames::Run>::values>(std::atof(registerData.data()));
        device.set<A4963RegisterNames::Run>(d);
    }

#pragma GCC diagnostic pop

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

}
