//
// Created by sebastian on 19.10.18.
//

#pragma once

#include <ratio>
#include <limits>
#include <stdexcept>
#include "../utils/RatioLookup.h"

namespace CustomDataTypes::Electricity {
    template<typename D, typename Rep, typename Period>
    class SIUnit;

    template<template<typename, typename> typename D, typename Rep, std::intmax_t Num, std::intmax_t Denom>
    class SIUnit<D<Rep, std::ratio<Num, Denom>>, Rep, std::ratio<Num, Denom>> {
        using Derived = D<Rep, std::ratio<Num, Denom>>;
        using value_type = Rep;
    public:
        //constexpr SIUnit() noexcept = default;

        //template<typename oC, typename = std::enable_if_t<is_template_same_v<C, oC>>>
        //constexpr SIUnit(const oC& si) noexcept : internalRepresentation(si.internalRepresentation) {}

        //template<typename oC, typename = std::enable_if_t<is_template_same_v<C, oC>>>
        //constexpr SIUnit(oC&& si) noexcept : internalRepresentation(std::move(si.internalRepresentation)) { }

        constexpr explicit SIUnit(Rep input) noexcept : internalRepresentation{input} {}

        //template<template<typename, typename> typename oC, typename oRep, std::intmax_t oNum, std::intmax_t oDenom>
        //constexpr explicit SIUnit(const SIUnit<oC<oRep, std::ratio<oNum, oDenom>>> &si) noexcept : internalRepresentation(convert_value(si)){ }

        //~SIUnit() = default;

        template<typename oRep, std::intmax_t oNum, std::intmax_t oDenom>
        constexpr Derived &operator=(D<oRep, std::ratio<oNum, oDenom>> other) {
            std::swap(internalRepresentation, other.internalRepresentation);
            return derived_this();
        }

        constexpr Rep count() const { return internalRepresentation; }

        constexpr Derived operator+() const {
            return derived_this();
        }

        template<typename oRep, std::intmax_t oNum, std::intmax_t oDenom>
        constexpr operator D<oRep, std::ratio<oNum, oDenom>>() const {
            static_assert((std::is_floating_point_v<oRep> && std::is_floating_point_v<Rep>) ||
                          (std::is_integral_v<oRep> && std::is_integral_v<Rep>));
            /*if(Num == oNum && oDenom == Denom) {
                Derived *p = static_cast<Derived *>(this);
                return *p;
            }*/
            return D<oRep, std::ratio<oNum, oDenom>>{(internalRepresentation * Num * oDenom) / (oNum * Denom)};
        }

        template<typename Number, typename = std::enable_if_t<std::is_arithmetic<Number>::value>>
        constexpr explicit operator Number() const {
            if (internalRepresentation < std::numeric_limits<Number>::max()) {
                return count();
            } else {
                throw std::overflow_error("Invalid cast of Volt type!");
            }
        }

        template<typename oRep, std::intmax_t oNum, std::intmax_t oDenom>
        constexpr bool operator==(const D<oRep, std::ratio<oNum, oDenom>> &rhs) const {
            return internalRepresentation == convert_value(rhs);
        }

        template<typename oRep, std::intmax_t oNum, std::intmax_t oDenom>
        constexpr bool operator<(const D<oRep, std::ratio<oNum, oDenom>> &rhs) const {
            return internalRepresentation < convert_value(rhs);
        }


        template<typename oRep, std::intmax_t oNum, std::intmax_t oDenom>
        D<Rep, std::ratio<Num, Denom>> &operator+=(const D<oRep, std::ratio<oNum, oDenom>> &rhs) {
            internalRepresentation += convert_value(rhs);
            Derived *p = static_cast<Derived *>(this);
            return *p;
        }

        template<typename oRep, std::intmax_t oNum, std::intmax_t oDenom>
        D<Rep, std::ratio<Num, Denom>> &operator-=(const D<oRep, std::ratio<oNum, oDenom>> &rhs) {
            internalRepresentation -= convert_value(rhs);
            Derived *p = static_cast<Derived *>(this);
            return *p;
        }

        template<typename oRep, std::intmax_t oNum, std::intmax_t oDenom>
        D<Rep, std::ratio<Num, Denom>> &operator*=(const D<oRep, std::ratio<oNum, oDenom>> &rhs) {
            internalRepresentation *= convert_value(rhs);
            Derived *p = static_cast<Derived *>(this);
            return *p;
        }

        template<typename T>
        D<Rep, std::ratio<Num, Denom>> &operator*=(const T &rhs) {
            internalRepresentation *= rhs;
            Derived *p = static_cast<Derived *>(this);
            return *p;
        }

        template<typename oRep, std::intmax_t oNum, std::intmax_t oDenom>
        D<Rep, std::ratio<Num, Denom>> &operator/=(const D<oRep, std::ratio<oNum, oDenom>> &rhs) {
            internalRepresentation /= convert_value(rhs);
            Derived *p = static_cast<Derived *>(this);
            return *p;
        }

    private:
        template<typename oRep, std::intmax_t oNum, std::intmax_t oDenom>
        constexpr Rep convert_value(const D<oRep, std::ratio<oNum, oDenom>> &rhs) const {
            return ((rhs.count() * oNum * Denom) / (Num * oDenom));
        }

        constexpr Derived derived_this() {
            Derived *p = static_cast<Derived *>(this);
            return *p;
        }

        template<typename oC, typename oRep, typename oPeriod>
        friend
        class SIUnit;

        Rep internalRepresentation;
    };

    template<template<typename, typename> typename C, typename Rep, typename Period, typename oRep, typename oPeriod>
    C<Rep, Period> operator+(C<Rep, Period> lhs, const C<oRep, oPeriod> &rhs) {
        lhs += rhs;
        return lhs;
    }

    template<template<typename, typename> typename C, typename Rep, typename Period, typename oRep, typename oPeriod>
    C<Rep, Period> operator-(C<Rep, Period> lhs, const C<oRep, oPeriod> &rhs) {
        lhs -= rhs;
        return lhs;
    }

    template<template<typename, typename> typename C, typename Rep, typename Period, typename oRep, typename oPeriod>
    C<Rep, Period> operator*(C<Rep, Period> lhs, const C<oRep, oPeriod> &rhs) {
        lhs *= rhs;
        return lhs;
    }

    template<template<typename, typename> typename C, typename Rep, typename Period, typename T>
    C<Rep, Period> operator*(C<Rep, Period> lhs, const T &rhs) {
        lhs *= rhs;
        return lhs;
    }

    template<template<typename, typename> typename C, typename Rep, typename Period, typename T>
    C<Rep, Period> operator*(const T &rhs, C<Rep, Period> lhs) {
        lhs *= rhs;
        return lhs;
    }

    template<template<typename, typename> typename C, typename Rep, typename Period, typename oRep, typename oPeriod>
    C<Rep, Period> operator/(C<Rep, Period> lhs, const C<oRep, oPeriod> &rhs) {
        lhs /= rhs;
        return lhs;
    }

    template<template<typename, typename> typename C, typename Rep, typename Period, typename oRep, typename oPeriod>
    constexpr bool operator!=(const C<Rep, Period> &lhs, const C<oRep, oPeriod> &rhs) {
        return !(lhs == rhs);
    }

    template<template<typename, typename> typename C, typename Rep, typename Period, typename oRep, typename oPeriod>
    constexpr bool operator>(const C<Rep, Period> &lhs, const C<oRep, oPeriod> &rhs) {
        return rhs < lhs;
    }

    template<template<typename, typename> typename C, typename Rep, typename Period, typename oRep, typename oPeriod>
    constexpr bool operator<=(const C<Rep, Period> &lhs, const C<oRep, oPeriod> &rhs) {
        return !(rhs < lhs);
    }

    template<template<typename, typename> typename C, typename Rep, typename Period, typename oRep, typename oPeriod>
    constexpr bool operator>=(const C<Rep, Period> &lhs, const C<oRep, oPeriod> &rhs) {
        return !(lhs< rhs);
    }

    template<template<typename, typename> typename C, typename Rep, std::intmax_t Num, std::intmax_t Denom>
    std::ostream &operator<<(std::ostream &os, const C<Rep, std::ratio<Num, Denom>> &other) {
        os << other.count() << " " << utils::ratio_lookup<std::ratio<Num, Denom>>::abr_value << C<Rep, std::ratio<Num, Denom>>::abr_value;
        return os;
    }

    template<typename Rep, typename Period = std::ratio<1> >
    class Volt : public SIUnit<Volt<Rep, Period>, Rep, Period> {
        using parent_type = SIUnit<Volt<Rep, Period>, Rep, Period>;
    public:
        static constexpr std::string_view abr_value = "V";
        constexpr explicit Volt(Rep input) : parent_type{input} {}
    };

    template<typename Rep, typename Period = std::ratio<1> >
    class Herz : public SIUnit<Herz<Rep, Period>, Rep, Period> {
        using parent_type = SIUnit<Herz<Rep, Period>, Rep, Period>;
    public:
        static constexpr std::string_view abr_value = "Hz";
        constexpr explicit Herz(Rep input) : parent_type{input} {}
        constexpr explicit Herz(parent_type &input) {}
    };

    using nanovolt  = Volt<std::intmax_t, std::nano>;
    using microvolt = Volt<std::intmax_t, std::micro>;
    using millivolt = Volt<std::intmax_t, std::milli>;
    using volt      = Volt<std::intmax_t>;
    using kilovolt  = Volt<std::intmax_t, std::kilo>;
    using megavolt  = Volt<std::intmax_t, std::mega>;

    namespace literals {
        constexpr nanovolt operator ""_nV(unsigned long long element) {
            return nanovolt(static_cast<std::intmax_t>(element));
        }

        constexpr Volt<long double, std::nano> operator ""_nV(long double element) {
            return Volt<long double, std::nano>(element);
        }

        constexpr microvolt operator ""_uV(unsigned long long element) {
            return microvolt(static_cast<std::intmax_t>(element));
        }

        constexpr Volt<long double, std::micro> operator ""_uV(long double element) {
            return Volt<long double, std::micro>(element);
        }

        constexpr millivolt operator ""_mV(unsigned long long element) {
            return millivolt(static_cast<std::intmax_t>(element));
        }

        constexpr Volt<long double, std::milli> operator ""_mV(long double element) {
            return Volt<long double, std::milli>(element);
        }

        constexpr volt operator ""_V(unsigned long long element) {
            return volt(static_cast<std::intmax_t>(element));
        }

        constexpr Volt<long double> operator ""_V(long double element) {
            return Volt<long double>(element);
        }

        constexpr kilovolt operator ""_kV(unsigned long long element) {
            return kilovolt(static_cast<std::intmax_t>(element));
        }

        constexpr Volt<long double, std::kilo> operator ""_kV(long double element) {
            return Volt<long double, std::kilo>(element);
        }

        constexpr megavolt operator ""_MV(unsigned long long element) {
            return megavolt(static_cast<std::intmax_t>(element));
        }

        constexpr Volt<long double, std::mega> operator ""_MV(long double element) {
            return Volt<long double, std::mega>(element);
        }
    }

}



