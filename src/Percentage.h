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
        inline Percentage() = default;
        inline explicit Percentage(accuracy percent) : mPercentage(percent){};
        inline Percentage(Percentage&& other) noexcept : Percentage() { this->swap(other);};
        inline Percentage(const Percentage& other) : mPercentage(other.mPercentage) {};
        inline accuracy getPercent() const { return mPercentage; };
        explicit  operator accuracy(){
            return getPercent();
        }
        Percentage& operator+=(const Percentage& other){
            this->mPercentage += other.mPercentage;
            return *this;
        }
        Percentage& operator*=(const Percentage& other){
            this->mPercentage = (this->mPercentage*other.mPercentage)/100;
            return *this;
        }
        Percentage& operator*=(const accuracy skalar){
            this->mPercentage *= skalar;
            return *this;
        }
        Percentage& operator-=(const Percentage& other){
            if(other.mPercentage <= this->mPercentage)
                this->mPercentage -= other.mPercentage;
            else this->mPercentage = 0;
            return *this;
        }
        Percentage& operator/=(const Percentage& other){
            this->mPercentage = (this->mPercentage/other.mPercentage)/100;
            return *this;
        }
        Percentage& operator/=(const accuracy skalar){
            this->mPercentage /= skalar;
            return *this;
        }

        Percentage& operator~()=delete;

        inline Percentage& swap(Percentage& other){
            using namespace std;
            swap(other.mPercentage, this->mPercentage);
            return *this;
        }



        friend std::ostream &operator<<(std::ostream &os, const Percentage &percentage);
    };

    namespace literals {
        auto operator"" _perc(long double percent){
            return Percentage<long double>(percent);
        }
    }

    template<typename accuracy>
    Percentage<accuracy> operator+(const Percentage<accuracy>& lhs, const Percentage<accuracy>& rhs){
        Percentage<accuracy> temp{lhs};
        temp+=rhs;
        return temp;
    }
    template<typename accuracy>
    Percentage<accuracy> operator*(const Percentage<accuracy>& lhs, const Percentage<accuracy>& rhs){
        Percentage<accuracy> temp{lhs};
        temp*=rhs;
        return temp;
    }
    template<typename accuracy>
    Percentage<accuracy> operator*(const Percentage<accuracy>& lhs, const accuracy& rhs){
        Percentage<accuracy> temp{lhs};
        temp*=rhs;
        return temp;
    }
    template<typename accuracy>
    Percentage<accuracy> operator/(const Percentage<accuracy>& lhs, const Percentage<accuracy>& rhs){
        Percentage<accuracy> temp{lhs};
        temp/=rhs;
        return temp;
    }
    template<typename accuracy>
    Percentage<accuracy> operator-(const Percentage<accuracy>& lhs, const Percentage<accuracy>& rhs){
        Percentage<accuracy> temp{lhs};
        temp-=rhs;
        return temp;
    }

    template<typename accuracy>
    void swap(const Percentage<accuracy>& lhs, const Percentage<accuracy>& rhs){
        lhs.swap(rhs);
    }

    template<typename accuracy>
    std::ostream &operator<<(std::ostream &os, const Percentage<accuracy>& percentage) {
        os << " Percentage: " << percentage.getPercent();
        return os;
    }
}

