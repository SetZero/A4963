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

    static inline const json defaultValues{
            {"config", 
                {
                    json{{"Blank Time","1200ns"}},
                    json{{"Dead Time","1000ns"}},
                    json{{"Recirculation Mode","Off"}},
                    json{{"Percent Fast Decay","12.5 Percent"}},
                    json{{"Invert PWM Input","Normal True Logic"}},
                    json{{"Current Sense Threshold Voltage","200mV"}},
                    json{{"Bemf Time Qualifier","Debounce Timer"}},
                    json{{"VDS Threshold","1.55V"}},
                    json{{"Position Controller Proportional Gain", 1}},
                    json{{"Overspeed Limit Ratio","150 Percent"}},
                    json{{"Degauss Compensation","Off"}},
                    json{{"Fixed Period","50.4us"}},
                    json{{"Position Controller Integral Gain",1}},
                    json{{"PWM Duty Cycle Hold Torque","37.5%"}},
                    json{{"Hold Time","16ms"}},
                    json{{"PI Controller Proportional Gain",1}},
                    json{{"PWM Duty Cycle Torque Startup","50%"}},
                    json{{"Start Speed","8Hz"}},
                    json{{"PI Controller Integral Gain",1}},
                    json{{"Speed Output Selection","Electrical Frequenzy"}},
                    json{{"Maximum Speed Setting","819.1Hz"}},
                    json{{"Phase Advance",15}},
                    json{{"Motor Control Mode","Indirect Speed"}},
                    json{{"Enable Stop On Fail","No Stop On Fail"}},
                    json{{"Duty Cycle Control",0}},
                    json{{"Restart Control","Restart After Loss Of Sync"}},
                    json{{"Brake","Brake Disabled"}},
                    json{{"Direction Of Rotation","Forward"}},
                    json{{"Run Enable","Start And Run Motor"}},
                }
            },
            {"mask", 
                {
                    json{{"Temperature Warning",false}},
                    json{{"Overtemperature",false}},
                    json{{"Loss Of Bemf Synchronitation",false}},
                    json{{"Phase A High Side VDS",false}},
                    json{{"Phase B High Side VDS",false}},
                    json{{"Phase C High Side VDS",false}},
                    json{{"VBB Undervoltage",false}},
                }
            }
    };

    static auto A4963MasksMap = std::map<std::string_view, Masks>{
            {"Temperatur Warning", Masks::TemperatureWarning},
            {"Loss Of Bemf Synchronitation", Masks::LossOfBemfSynchronization},
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
    void setRuntime(A4963 &device, A4963RegisterNames toSet, const std::string& registerData) {
        if (toSet == N) {
            if constexpr(RegisterValues<N>::isRanged) {
                JsonSetter::UnitInfo unitInfo = JsonSetter::parseData(registerData);
                if(unitInfo.succsess) {
                    auto prefix = unitInfo.prefix;
                    auto unit = unitInfo.unit;
                    auto data = unitInfo.data;

                    using type = std::remove_const_t<decltype(RegisterValues<N>::min)>;
                    if constexpr(utils::is_periodic<type>::value) {
                        if (setIfPeriodic<type, N>(device, static_cast<double>(data), prefix, unit)) {
                            std::cout << "Set the register " << RegisterValues<N>::name << " to " << data << prefix
                                      << unit << std::endl;
                        }
                    } else {
                        if (prefix != '\0') {
                            std::cerr << "This register (" << RegisterValues<N>::name
                                      << ") doesn't expect a prefix. This might be an error!" << std::endl;
                        }
                        if (std::is_arithmetic_v<type> && !unit.empty()) {
                            std::cerr << "This register (" << RegisterValues<N>::name
                                      << ") doesn't expect an unit. This might be an error!" << std::endl;
                        }
                        setIfNotPeriodic<type, N>(device, static_cast<double>(data));
                        std::cout << "Set the register " << RegisterValues<N>::name << " to " << data << prefix << unit
                                  << std::endl;
                    }
                } else {
                    std::cerr << "There was an error parsing your data!" << std::endl;
                }
            } else {
                try {
                    auto type = RegisterValues<N>::map.at(registerData.data());
                    device.set<N>(type);
                    std::cout << "Set the register " << RegisterValues<N>::name << " to " << registerData << std::endl;
                } catch (std::exception& e) {
                    std::cerr << "Unknown value " << registerData << " in register " << RegisterValues<N>::name << std::endl;
                }
            }
        } else {
            setRuntime<static_cast<A4963RegisterNames>(static_cast<uint8_t>(N) + 1)>(device, toSet, registerData);
        }
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

    template<>
    void setRuntime<A4963RegisterNames::Run>(A4963 &device, A4963RegisterNames toSet, const std::string& registerData) {
        auto d = static_cast<typename RegisterValues<A4963RegisterNames::Run>::values>(std::atof(registerData.data()));
        device.set<A4963RegisterNames::Run>(d);
    }

#pragma GCC diagnostic pop

    template<typename T = void>
    void setRuntime(A4963 &device, A4963RegisterNames toSet, const std::string& registerData) {
        setRuntime<static_cast<A4963RegisterNames>(0)>(device, toSet, registerData);
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

}
