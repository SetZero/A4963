//
// Created by sebastian on 17.09.18.
//

#pragma once

#include <ratio>
#include <limits>
#include <stdexcept>
#include <iostream>
#include "../utils/RatioLookup.h"
#include "SIUnit.h"

namespace CustomDataTypes::Electricity {

    template<typename Rep, typename Period = std::ratio<1> >
    class Volt : public SIUnit<Volt<Rep, Period>, Rep, Period> {
        using parent_type = SIUnit<Volt<Rep, Period>, Rep, Period>;
    public:
        static constexpr std::string_view abr_value = "V";
        constexpr explicit Volt(Rep input) noexcept : parent_type{input} {}
    };

    using nanovolt  = Volt<std::intmax_t, std::nano>;
    using microvolt = Volt<std::intmax_t, std::micro>;
    using millivolt = Volt<std::intmax_t, std::milli>;
    using volt      = Volt<std::intmax_t>;
    using kilovolt  = Volt<std::intmax_t, std::kilo>;
    using megavolt  = Volt<std::intmax_t, std::mega>;

    namespace literals {
        constexpr nanovolt operator ""_nV(unsigned long long element) noexcept {
            return nanovolt(static_cast<std::intmax_t>(element));
        }

        constexpr Volt<long double, std::nano> operator ""_nV(long double element) noexcept {
            return Volt<long double, std::nano>(element);
        }

        constexpr microvolt operator ""_uV(unsigned long long element) noexcept {
            return microvolt(static_cast<std::intmax_t>(element));
        }

        constexpr Volt<long double, std::micro> operator ""_uV(long double element) noexcept {
            return Volt<long double, std::micro>(element);
        }

        constexpr millivolt operator ""_mV(unsigned long long element) noexcept {
            return millivolt(static_cast<std::intmax_t>(element));
        }

        constexpr Volt<long double, std::milli> operator ""_mV(long double element) noexcept {
            return Volt<long double, std::milli>(element);
        }

        constexpr volt operator ""_V(unsigned long long element) noexcept {
            return volt(static_cast<std::intmax_t>(element));
        }

        constexpr Volt<long double> operator ""_V(long double element) noexcept {
            return Volt<long double>(element);
        }

        constexpr kilovolt operator ""_kV(unsigned long long element) noexcept {
            return kilovolt(static_cast<std::intmax_t>(element));
        }

        constexpr Volt<long double, std::kilo> operator ""_kV(long double element) noexcept {
            return Volt<long double, std::kilo>(element);
        }

        constexpr megavolt operator ""_MV(unsigned long long element) noexcept {
            return megavolt(static_cast<std::intmax_t>(element));
        }

        constexpr Volt<long double, std::mega> operator ""_MV(long double element) noexcept {
            return Volt<long double, std::mega>(element);
        }
    }
}


