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

namespace CustomDataTypes::Frequency {

    template<typename Rep, typename Period = std::ratio<1> >
    class Hertz : public SIUnit<Hertz<Rep, Period>, Rep, Period> {
        using parent_type = SIUnit<Hertz<Rep, Period>, Rep, Period>;
    public:
        static constexpr std::string_view abr_value = "Hz";
        constexpr explicit Hertz(Rep input) : parent_type{input} {}
    };

    using nanohertz  = Hertz<std::intmax_t, std::nano>;
    using microhertz = Hertz<std::intmax_t, std::micro>;
    using millihertz = Hertz<std::intmax_t, std::milli>;
    using hertz      = Hertz<std::intmax_t>;
    using kilohertz  = Hertz<std::intmax_t, std::kilo>;
    using megahertz  = Hertz<std::intmax_t, std::mega>;
    using gigahertz  = Hertz<std::intmax_t, std::giga>;
    using terahertz  = Hertz<std::intmax_t, std::tera>;
    using petahertz  = Hertz<std::intmax_t, std::peta>;
    using exahertz  = Hertz<std::intmax_t, std::exa>;

    namespace literals {
        constexpr nanohertz operator ""_nHz(unsigned long long element) {
            return nanohertz(static_cast<std::intmax_t>(element));
        }

        constexpr Hertz<long double, std::nano> operator ""_nHz(long double element) {
            return Hertz<long double, std::nano>(element);
        }

        constexpr microhertz operator ""_uHz(unsigned long long element) {
            return microhertz(static_cast<std::intmax_t>(element));
        }

        constexpr Hertz<long double, std::micro> operator ""_uHz(long double element) {
            return Hertz<long double, std::micro>(element);
        }

        constexpr millihertz operator ""_mHz(unsigned long long element) {
            return millihertz(static_cast<std::intmax_t>(element));
        }

        constexpr Hertz<long double, std::milli> operator ""_mHz(long double element) {
            return Hertz<long double, std::milli>(element);
        }

        constexpr hertz operator ""_Hz(unsigned long long element) {
            return hertz(static_cast<std::intmax_t>(element));
        }

        constexpr Hertz<long double> operator ""_Hz(long double element) {
            return Hertz<long double>(element);
        }

        constexpr kilohertz operator ""_kHz(unsigned long long element) {
            return kilohertz(static_cast<std::intmax_t>(element));
        }

        constexpr Hertz<long double, std::kilo> operator ""_kHz(long double element) {
            return Hertz<long double, std::kilo>(element);
        }

        constexpr megahertz operator ""_MHz(unsigned long long element) {
            return megahertz(static_cast<std::intmax_t>(element));
        }

        constexpr Hertz<long double, std::mega> operator ""_MHz(long double element) {
            return Hertz<long double, std::mega>(element);
        }
    }
}






