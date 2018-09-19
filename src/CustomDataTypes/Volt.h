//
// Created by sebastian on 17.09.18.
//

#pragma once

#include <ratio>

namespace CustomDataTypes {
    template<typename Rep, typename Period = std::ratio<1> >
    class Volt;

    template<typename Rep, std::intmax_t Num, std::intmax_t Denom>
    class Volt<Rep, std::ratio<Num, Denom>> {
    public:
        constexpr Volt() = default;

        Volt(const Volt &) = default;

        explicit Volt(Rep input) {
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

    inline namespace literals {
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


