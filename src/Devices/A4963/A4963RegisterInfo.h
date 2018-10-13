//
// Created by sebastian on 23.09.18.
//

#pragma once

#include <chrono>
#include<functional>
#include "../../utils/scales/UnitScale.h"
#include "../../CustomDataTypes/Volt.h"

namespace NS_A4963 {

    using namespace std::literals::chrono_literals;
    using namespace CustomDataTypes::Electricity::literals;

    enum class A4963RegisterNames {
        BlankTime,
        DeadTime,
        CurrentSenseThresholdVoltage,
        VDSThreshold
    };

    template<A4963RegisterNames reg>
    struct RegisterValues;

    template<>
    struct RegisterValues<A4963RegisterNames::BlankTime> {
        using type = UnitScale<std::chrono::duration<long double, std::nano>,  uint16_t >;
        static constexpr type value{400ns, 6us, 0us};
		template<typename T1, typename T2>
		static const auto normalizer(T1 input, T2 precision) {
			return static_cast<type::value_type>(input / precision);
		}
			; //= [](auto input, auto precision) { return static_cast<type::value_type>(input / precision); };
    };

    template<>
    struct RegisterValues<A4963RegisterNames::DeadTime> {
        using type = UnitScale<std::chrono::duration<long double, std::nano>, uint16_t>;
        static constexpr type value{50ns, 3.15us, 100ns};
		template<typename T1, typename T2>
		static const auto normalizer(T1 input, T2 precision) {
			return static_cast<type::value_type>(input / precision);
		} //= [](auto input, auto precision) { return static_cast<type::value_type>(input / precision); };
    };

    template<>
    struct RegisterValues<A4963RegisterNames::CurrentSenseThresholdVoltage> {
        using type = UnitScale<CustomDataTypes::Electricity::Volt<long double, std::milli>, uint16_t>;
        static constexpr type value{12.5_mV, 200.0_mV, 12.5_mV};
		template<typename T1, typename T2>
		static const auto normalizer(T1 input, T2 precision) {
			return static_cast<type::value_type>(input / precision) - 1;
		} //= [](auto input, auto precision) { return (static_cast<type::value_type>(input / precision) - 1); };
    };

    template<>
    struct RegisterValues<A4963RegisterNames::VDSThreshold> {
        using type = UnitScale<CustomDataTypes::Electricity::Volt<long double, std::milli>, uint16_t>;
        static constexpr type value{50.0_mV, 1.55_V, 0.0_mV};
		template<typename T1, typename T2>
		static const auto normalizer(T1 input, T2 precision) {
			return static_cast<type::value_type>(input / precision) - 1;
		} //= [](auto input, auto precision) { return static_cast<type::value_type>(input / precision); };
    };
}