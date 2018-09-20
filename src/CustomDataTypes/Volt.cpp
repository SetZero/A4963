//
// Created by sebastian on 17.09.18.
//

#include "Volt.h"
namespace CustomDataTypes::Electricity {
    using nanovolt  = Volt<std::intmax_t, std::nano>;
    using microvolt = Volt<std::intmax_t, std::micro>;
    using millivolt = Volt<std::intmax_t, std::milli>;
    using volt      = Volt<std::intmax_t>;
    using kilovolt  = Volt<std::intmax_t, std::kilo>;
    using megavolt  = Volt<std::intmax_t, std::mega>;

    namespace literals {
        auto operator ""_nV(unsigned long long element) {
            return nanovolt(static_cast<intmax_t>(element));
        }

        auto operator ""_nV(long double element) {
            return Volt<long double, std::nano>(element);
        }

        auto operator ""_uV(unsigned long long element) {
            return microvolt(static_cast<intmax_t>(element));
        }

        auto operator ""_uV(long double element) {
            return Volt<long double, std::micro>(element);
        }

        auto operator ""_mV(unsigned long long element) {
            return millivolt(static_cast<intmax_t>(element));
        }

        auto operator ""_mV(long double element) {
            return Volt<long double, std::milli>(element);
        }

        auto operator ""_V(unsigned long long element) {
            return volt(static_cast<intmax_t>(element));
        }

        auto operator ""_V(long double element) {
            return Volt<long double>(element);
        }

        auto operator ""_kV(unsigned long long element) {
            return kilovolt(static_cast<intmax_t>(element));
        }

        auto operator ""_kV(long double element) {
            return Volt<long double, std::kilo>(element);
        }

        auto operator ""_MV(unsigned long long element) {
            return megavolt(static_cast<intmax_t>(element));
        }

        auto operator ""_MV(long double element) {
            return Volt<long double, std::mega>(element);
        }
    }
}