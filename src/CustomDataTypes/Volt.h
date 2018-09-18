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
            internalRepresentation = (volt.count() * oNum * Denom) / (Num * oDenom);
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
            return internalRepresentation == (rhs.count() * oNum * Denom) / (Num * oDenom);
        }

        template<typename oRep, std::intmax_t oNum, std::intmax_t oDenom>
        bool operator!=(const Volt<oRep, std::ratio<oNum, oDenom>> &rhs) const {
            return !(rhs == *this);
        }

        template<typename oRep, std::intmax_t oNum, std::intmax_t oDenom>
        bool operator<(const Volt<oRep, std::ratio<oNum, oDenom>> &rhs) const {
            return internalRepresentation < (rhs.count() * oNum * Denom) / (Num * oDenom);
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

    private:
        Rep internalRepresentation;
    };

    using nanovolt  = Volt<std::intmax_t, std::nano>;
    using microvolt = Volt<std::intmax_t, std::micro>;
    using millivolt = Volt<std::intmax_t, std::milli>;
    using volt      = Volt<std::intmax_t>;
    using kilovolt  = Volt<std::intmax_t, std::kilo>;
    using megavolt  = Volt<std::intmax_t, std::mega>;
}

