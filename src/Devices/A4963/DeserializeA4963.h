//
// Created by sebastian on 23.10.18.
//

#pragma once

#include <chrono>
#include <variant>
#include <fstream>
#include "../../CustomDataTypes/SIUnit.h"
#include "../../CustomDataTypes/Volt.h"
#include "../../CustomDataTypes/Hertz.h"
#include "../../CustomDataTypes/Percentage.h"
#include "../../../inc/json.h"
#include "A4963.h"
#include "A4963RegisterInfo.h"

namespace NS_A4963 {
    using nlohmann::json;

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
        if (unit == "v") {
            return CustomDataTypes::Electricity::Volt<Rep, Period>{newValue};
        } else if (unit == "hz") {
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
    static void setIfPeriodic(A4963 &device, double data, const char prefix, const std::string &unit) {
        using Rep = typename utils::periodic_info<T>::rep;
        try {
            auto d = T{std::get<T>(
                    getType<Rep, typename utils::periodic_info<T>::period>(prefix, unit, static_cast<Rep>(data)))};
            device.set<N>(d);
        }
        catch(std::exception& e){
            std::cerr << "Invalid Unit \"" << prefix << unit << "\" in register \"" << RegisterValues<N>::name << "\"" << std::endl;
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
                    setIfPeriodic<type, N>(device, data, prefix, unit);
                } else {
                    if(prefix != '\0') {
                        std::cerr << "This register (" << RegisterValues<N>::name << ") doesn't expect a prefix. This might be an error!" << std::endl;
                    }
                    if(std::is_arithmetic_v<type> && !unit.empty()) {
                            std::cerr << "This register (" << RegisterValues<N>::name << ") doesn't expect an unit. This might be an error!" << std::endl;
                    }
                    setIfNotPeriodic<type, N>(device, data);
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
        JsonSetter(const std::string& str);
    };

}