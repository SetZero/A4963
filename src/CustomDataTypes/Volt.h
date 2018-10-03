//
// Created by sebastian on 17.09.18.
//

#pragma once

#include <ratio>

namespace CustomDataTypes::Electricity {
    template<typename Rep, typename Period = std::ratio<1> >
    class Volt;

    template<typename Rep, std::intmax_t Num, std::intmax_t Denom>
    class Volt<Rep, std::ratio<Num, Denom>> {
    public:
        constexpr Volt() = default;

        Volt(const Volt &) = default;

        constexpr explicit Volt(Rep input) {
            internalRepresentation = input;
        }

        template<typename oRep, std::intmax_t oNum, std::intmax_t oDenom>
        explicit Volt(Volt<oRep, std::ratio<oNum, oDenom>> &volt) {
            internalRepresentation = convert_value(volt);
        }

        ~Volt() = default;

        Volt &operator=(const Volt &) = default;

        constexpr Rep count() const { return internalRepresentation; }

        constexpr Volt operator+() const { return *this; }

        template<typename oRep, std::intmax_t oNum, std::intmax_t oDenom>
        explicit operator Volt<oRep, std::ratio<oNum, oDenom>>() {
            internalRepresentation = (internalRepresentation * oNum * Denom) / (Num * oDenom);
        }

        template<typename oRep, std::intmax_t oNum, std::intmax_t oDenom>
        bool operator==(const Volt<oRep, std::ratio<oNum, oDenom>> &rhs) const {
            return internalRepresentation == convert_value(rhs);
        }

        template<typename oRep, std::intmax_t oNum, std::intmax_t oDenom>
        bool operator!=(const Volt<oRep, std::ratio<oNum, oDenom>> &rhs) const {
            return !(rhs == *this);
        }

        template<typename oRep, std::intmax_t oNum, std::intmax_t oDenom>
        bool operator<(const Volt<oRep, std::ratio<oNum, oDenom>> &rhs) const {
            return internalRepresentation < convert_value(rhs);
        }

        template<typename oRep, std::intmax_t oNum, std::intmax_t oDenom>
        bool operator>(const Volt<oRep, std::ratio<oNum, oDenom>> &rhs) const {
            return rhs < *this;
        }

        template<typename oRep, std::intmax_t oNum, std::intmax_t oDenom>
        bool operator<=(const Volt<oRep, std::ratio<oNum, oDenom>> &rhs) const {
            return !(rhs < *this);
        }

        template<typename oRep, std::intmax_t oNum, std::intmax_t oDenom>
        bool operator>=(const Volt<oRep, std::ratio<oNum, oDenom>> &rhs) const {
            return !(*this < rhs);
        }

        template<typename oRep, std::intmax_t oNum, std::intmax_t oDenom>
        Volt<Rep, std::ratio<Num, Denom>>& operator+=(const Volt<oRep, std::ratio<oNum, oDenom>>& rhs) {
            internalRepresentation += convert_value(rhs);
            return *this;
        }

        template<typename oRep, std::intmax_t oNum, std::intmax_t oDenom>
        Volt<Rep, std::ratio<Num, Denom>>& operator-=(const Volt<oRep, std::ratio<oNum, oDenom>>& rhs) {
            internalRepresentation -= convert_value(rhs);
            return *this;
        }

        template<typename oRep, std::intmax_t oNum, std::intmax_t oDenom>
        Volt<Rep, std::ratio<Num, Denom>>& operator*=(const Volt<oRep, std::ratio<oNum, oDenom>>& rhs) {
            internalRepresentation *= convert_value(rhs);
            return *this;
        }

        template<typename oRep, std::intmax_t oNum, std::intmax_t oDenom>
        Volt<Rep, std::ratio<Num, Denom>>& operator/=(const Volt<oRep, std::ratio<oNum, oDenom>>& rhs) {
            internalRepresentation /= convert_value(rhs);
            return *this;
        }
    private:
        template<typename oRep, std::intmax_t oNum, std::intmax_t oDenom>
        Rep convert_value(const Volt<oRep, std::ratio<oNum, oDenom>>& rhs) const {
            return ((rhs.count() * oNum * Denom) / (Num * oDenom));
        }
        template<typename oRep, typename oPeriod>
        friend class Volt;
        Rep internalRepresentation;
    };

    template<typename Rep, typename Period, typename oRep, typename oPeriod>
    Volt<Rep, Period> operator+( Volt<Rep, Period> lhs, const Volt<oRep, oPeriod>& rhs ) {
        lhs += rhs;
        return lhs;
    }

    template<typename Rep, typename Period, typename oRep, typename oPeriod>
    Volt<Rep, Period> operator-( Volt<Rep, Period> lhs, const Volt<oRep, oPeriod>& rhs ) {
        lhs -= rhs;
        return lhs;
    }

    template<typename Rep, typename Period, typename oRep, typename oPeriod>
    Volt<Rep, Period> operator*( Volt<Rep, Period> lhs, const Volt<oRep, oPeriod>& rhs ) {
        lhs *= rhs;
        return lhs;
    }

    template<typename Rep, typename Period, typename oRep, typename oPeriod>
    Volt<Rep, Period> operator/( Volt<Rep, Period> lhs, const Volt<oRep, oPeriod>& rhs ) {
        lhs /= rhs;
        return lhs;
    }
    using nanovolt  = Volt<std::intmax_t, std::nano>;
    using microvolt = Volt<std::intmax_t, std::micro>;
    using millivolt = Volt<std::intmax_t, std::milli>;
    using volt      = Volt<std::intmax_t>;
    using kilovolt  = Volt<std::intmax_t, std::kilo>;
    using megavolt  = Volt<std::intmax_t, std::mega>;

    namespace literals {
        nanovolt operator ""_nV(unsigned long long element);
        Volt<long double, std::nano> operator ""_nV(long double element);

        microvolt operator ""_uV(unsigned long long element);
        Volt<long double, std::micro> operator ""_uV(long double element);

        millivolt operator ""_mV(unsigned long long element);
        Volt<long double, std::milli> operator ""_mV(long double element);

        volt operator ""_V(unsigned long long element);
        Volt<long double> operator ""_V(long double element);

        kilovolt operator ""_kV(unsigned long long element);
        Volt<long double, std::kilo> operator ""_kV(long double element);

        megavolt operator ""_MV(unsigned long long element);
        Volt<long double, std::mega> operator ""_MV(long double element);
    }
}


