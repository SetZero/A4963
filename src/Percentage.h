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
        constexpr inline Percentage() = default;
        constexpr inline explicit Percentage(accuracy percent) : mPercentage(percent){};
        constexpr inline Percentage(Percentage&& other) noexcept : Percentage() { this->swap(other);};
        constexpr inline Percentage(const Percentage& other) : mPercentage(other.mPercentage) {};
        constexpr inline accuracy getPercent() const { return mPercentage; };
        constexpr explicit operator accuracy(){
            return getPercent();
        }
        constexpr Percentage& set(accuracy percent){mPercentage = percent;};
        constexpr Percentage& operator+=(const Percentage& other){
            this->mPercentage += other.mPercentage;
            return *this;
        }
        constexpr Percentage& operator*=(const Percentage& other){
            this->mPercentage = (this->mPercentage*other.mPercentage)/100;
            return *this;
        }
        constexpr Percentage& operator*=(const accuracy skalar){
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
        constexpr Percentage& operator/=(const accuracy skalar){
            this->mPercentage = mPercentage/100/skalar;
            return *this;
        }

        constexpr Percentage& operator~()=delete;

        constexpr inline Percentage& swap(Percentage& other){
            using namespace std;
            swap(other.mPercentage, this->mPercentage);
            return *this;
        }
    };

    namespace literals {
        constexpr auto operator"" _perc(long double percent){
            return Percentage<long double>(percent);
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
    template<typename accuracy>
    constexpr Percentage<accuracy> operator*(const Percentage<accuracy>& lhs, const accuracy& rhs){
        Percentage<accuracy> temp{lhs};
        temp*=rhs;
        return temp;
    }
    template<typename accuracy>
    constexpr Percentage<accuracy> operator*(const accuracy& lhs, const Percentage<accuracy>& rhs){
        Percentage<accuracy> temp{rhs};
        temp*=lhs;
        return temp;
    }
    template<typename accuracy>
    constexpr Percentage<accuracy> operator/(const Percentage<accuracy>& lhs, const Percentage<accuracy>& rhs){
        Percentage<accuracy> temp{lhs};
        temp/=rhs;
        return temp;
    }
    template<typename accuracy>
    constexpr Percentage<accuracy> operator/(const Percentage<accuracy>& lhs, const accuracy rhs){
        Percentage<accuracy> temp{lhs};
        temp/=rhs;
        return temp;
    }
    template<typename accuracy>
    constexpr Percentage<accuracy> operator/(const accuracy lhs, const Percentage<accuracy>& rhs){
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

