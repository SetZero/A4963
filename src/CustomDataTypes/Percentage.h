//
// Created by keven on 30.09.18.
//

#pragma once
#include <type_traits>
#include <stdint.h>
#include <math.h>
#include <ostream>

namespace CustomDataTypes {

    template<typename accuracy>
    class Percentage {
        static_assert(std::is_floating_point<accuracy>(), "wrong type: only floating point types allowed");
        accuracy mPercentage = 0;
    public:
        using value_type = accuracy;
        constexpr inline Percentage() = default;
        constexpr inline explicit Percentage(accuracy percent) : mPercentage(percent){};
        constexpr Percentage(Percentage&& other) noexcept : Percentage() { this->swap(other);};
        constexpr inline Percentage(const Percentage& other) : mPercentage(other.mPercentage) {};
        constexpr inline accuracy getPercent() const { return mPercentage; };
        constexpr explicit operator accuracy() const {
            return mPercentage;
        }



        template<typename T>
        constexpr explicit operator T() const {
            static_assert(std::is_arithmetic<T>(), "cannot cast to a non arithmetic type");
            return (T)mPercentage;
        }

        constexpr Percentage& set(accuracy percent){mPercentage = percent; return *this;};
        constexpr Percentage& operator+=(const Percentage& other){
            this->mPercentage += other.mPercentage;
            return *this;
        }
        constexpr Percentage& operator*=(const Percentage& other){
            this->mPercentage = (this->mPercentage*other.mPercentage)/100;
            return *this;
        }
        template<typename Skalar>
        constexpr Percentage& operator*=(const Skalar skalar){
            static_assert(std::is_arithmetic<Skalar>(), "cannot multiplicate from a non arithmetic type");
            if(skalar >= 0)
                this->mPercentage = mPercentage/100 * skalar;
            else
                this->mPercentage = mPercentage/100 * -skalar;
            return *this;
        }
        constexpr Percentage& operator-=(const Percentage& other){
            if(other.mPercentage <= this->mPercentage)
                this->mPercentage -= other.mPercentage;
            else this->mPercentage = 0;
            return *this;
        }
        constexpr Percentage& operator/=(const Percentage& other){
            this->mPercentage = (this->mPercentage/other.mPercentage)/100;
            return *this;
        }
        template<typename Skalar>
        constexpr Percentage& operator/=(const Skalar skalar){
            static_assert(std::is_arithmetic<Skalar>(), "cannot divide by a non arithmetic type");
            this->mPercentage = mPercentage/100/skalar;
            return *this;
        }

        constexpr Percentage& operator~()=delete;

        constexpr inline Percentage& swap(Percentage& other){
            std::swap(other.mPercentage, this->mPercentage);
            return *this;
        }
    };

    namespace literals {
        constexpr auto operator"" _percLd(long double percent){
            return Percentage<long double>(percent);
        }
        constexpr auto operator"" _perc(long double percent){
            return Percentage<double>(percent);
        }
    }

    template<typename accuracy>
    constexpr Percentage<accuracy> operator+(const Percentage<accuracy>& lhs, const Percentage<accuracy>& rhs){
        Percentage<accuracy> temp{lhs};
        temp+=rhs;
        return temp;
    }
    template<typename accuracy>
    constexpr Percentage<accuracy> operator*(const Percentage<accuracy>& lhs, const Percentage<accuracy>& rhs){
        Percentage<accuracy> temp{lhs};
        temp*=rhs;
        return temp;
    }
    template<typename Skalar, typename accuracy>
    constexpr Percentage<accuracy> operator*(const Percentage<accuracy>& lhs, const Skalar& rhs){
        Percentage<accuracy> temp{lhs};
        temp*=rhs;
        return temp;
    }

    template<typename Skalar, typename accuracy>
    constexpr Percentage<accuracy> operator*(const Skalar& lhs, const Percentage<accuracy>& rhs){
        Percentage<accuracy> temp{rhs};
        temp*=lhs;
        return temp;
    }

    template<typename accuracy>
    constexpr bool operator==(const Percentage<accuracy>& lhs, const Percentage<accuracy>& rhs){
        return std::abs(lhs.getPercent() - rhs.getPercent()) < std::numeric_limits<accuracy>::epsilon();
    }

    template<typename accuracy>
    constexpr bool operator!=(const Percentage<accuracy>& lhs, const Percentage<accuracy>& rhs){
        return !(lhs == rhs);
    }

    template<typename accuracy>
    constexpr bool operator<=(const Percentage<accuracy>& lhs, const Percentage<accuracy>& rhs){
        return lhs.getPercent() < rhs.getPercent() || lhs == rhs;
    }

    template<typename accuracy>
    constexpr bool operator>=(const Percentage<accuracy>& lhs, const Percentage<accuracy>& rhs){
        return lhs.getPercent() > rhs.getPercent() || lhs == rhs;
    }
    template<typename accuracy>
    constexpr Percentage<accuracy> operator/(const Percentage<accuracy>& lhs, const Percentage<accuracy>& rhs){
        Percentage<accuracy> temp{lhs};
        temp/=rhs;
        return temp;
    }
    template<typename Skalar, typename accuracy>
    constexpr Percentage<accuracy> operator/(const Percentage<accuracy>& lhs, const Skalar rhs){
        Percentage<accuracy> temp{lhs};
        temp/=rhs;
        return temp;
    }
    template<typename Skalar, typename accuracy>
    constexpr Percentage<accuracy> operator/(const Skalar lhs, const Percentage<accuracy>& rhs){
        Percentage<accuracy> temp{lhs};
        temp/=(accuracy) rhs;
        return temp;
    }
    template<typename accuracy>
    constexpr Percentage<accuracy> operator-(const Percentage<accuracy>& lhs, const Percentage<accuracy>& rhs){
        Percentage<accuracy> temp{lhs};
        temp-=rhs;
        return temp;
    }

    template<typename accuracy>
    constexpr void swap(const Percentage<accuracy>& lhs, const Percentage<accuracy>& rhs){
        lhs.swap(rhs);
    }

    template<typename accuracy>
    constexpr std::ostream &operator<<(std::ostream &os, const Percentage<accuracy>& percentage) {
        os << " Percentage: " << percentage.getPercent();
        return os;
    }
}

