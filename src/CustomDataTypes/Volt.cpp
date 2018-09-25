//
// Created by sebastian on 17.09.18.
//

#include "Volt.h"
namespace CustomDataTypes::Electricity {

    namespace literals {
        nanovolt operator ""_nV(unsigned long long element) {
            return nanovolt(static_cast<intmax_t>(element));
        }

        Volt<long double, std::nano> operator ""_nV(long double element) {
            return Volt<long double, std::nano>(element);
        }

        microvolt operator ""_uV(unsigned long long element) {
            return microvolt(static_cast<intmax_t>(element));
        }

        Volt<long double, std::micro> operator ""_uV(long double element) {
            return Volt<long double, std::micro>(element);
        }

        millivolt operator ""_mV(unsigned long long element) {
            return millivolt(static_cast<intmax_t>(element));
        }

        Volt<long double, std::milli> operator ""_mV(long double element) {
            return Volt<long double, std::milli>(element);
        }

        volt operator ""_V(unsigned long long element) {
            return volt(static_cast<intmax_t>(element));
        }

        Volt<long double> operator ""_V(long double element) {
            return Volt<long double>(element);
        }

        kilovolt operator ""_kV(unsigned long long element) {
            return kilovolt(static_cast<intmax_t>(element));
        }

        Volt<long double, std::kilo> operator ""_kV(long double element) {
            return Volt<long double, std::kilo>(element);
        }

        megavolt operator ""_MV(unsigned long long element) {
            return megavolt(static_cast<intmax_t>(element));
        }

        Volt<long double, std::mega> operator ""_MV(long double element) {
            return Volt<long double, std::mega>(element);
        }
    }
}