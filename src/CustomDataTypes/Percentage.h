//
// Created by keven on 30.09.18.
//

#pragma once
#include <type_traits>
#include <cstdint>
#include <cmath>
#include <ostream>

namespace CustomDataTypes {

    template<typename accuracy>
    class Percentage {
        static_assert(std::is_floating_point<accuracy>(), "wrong type: only floating point types allowed");
        accuracy mPercentage = 0;
    public:
        static constexpr std::string_view abr_value = "%";
        using value_type = accuracy;
        constexpr inline Percentage() noexcept = default;
        constexpr inline explicit Percentage(accuracy percent) noexcept : mPercentage(percent){};
        constexpr Percentage(Percentage&& other) noexcept : Percentage() { this->swap(other);};
        constexpr inline Percentage(const Percentage& other) noexcept : mPercentage(other.mPercentage) {};
        constexpr inline accuracy getPercent() const noexcept { return mPercentage; };

        [[nodiscard]] constexpr value_type get() const noexcept {
            return mPercentage;
        }

        template<typename T>
        constexpr explicit operator T() const noexcept {
            static_assert(std::is_arithmetic<T>(), "cannot cast to a non arithmetic type");
            return (T)mPercentage;
        }

        constexpr Percentage& set(accuracy percent) noexcept {mPercentage = percent; return *this;};
        constexpr Percentage& operator+=(const Percentage& other) noexcept {
            this->mPercentage += other.mPercentage;
            return *this;
        }
        constexpr Percentage& operator*=(const Percentage& other) noexcept {
            this->mPercentage = (this->mPercentage*other.mPercentage)/100;
            return *this;
        }
        template<typename Skalar>
        constexpr Percentage& operator*=(const Skalar skalar) noexcept {
            static_assert(std::is_arithmetic<Skalar>(), "cannot multiplicate from a non arithmetic type");
            if(skalar >= 0)
                this->mPercentage = mPercentage * skalar;
            else
                this->mPercentage = mPercentage * -skalar;
            return *this;
        }
        constexpr Percentage& operator-=(const Percentage& other) noexcept {
            if(other.mPercentage <= this->mPercentage)
                this->mPercentage -= other.mPercentage;
            else this->mPercentage = 0;
            return *this;
        }
        constexpr Percentage& operator/=(const Percentage& other) noexcept {
            this->mPercentage = (this->mPercentage/other.mPercentage)/100;
            return *this;
        }
        template<typename Skalar>
        constexpr Percentage& operator/=(const Skalar skalar) noexcept {
            static_assert(std::is_arithmetic<Skalar>(), "cannot divide by a non arithmetic type");
            this->mPercentage /= skalar;
            return *this;
        }

        constexpr Percentage& operator~()=delete;

        constexpr inline Percentage& swap(Percentage& other) noexcept {
            std::swap(other.mPercentage, this->mPercentage);
            return *this;
        }
    };

    namespace literals {
        [[nodiscard]] constexpr auto operator"" _percLd(long double percent){
            return Percentage<long double>(percent);
        }
        constexpr auto operator"" _perc(long double percent){
            return Percentage<double>(static_cast<double>(percent));
        }
        constexpr auto operator"" _perc(unsigned long long percent){
            return Percentage<double>(static_cast<double>(percent));
        }
        constexpr auto operator"" _percLd(unsigned long long percent){
            return Percentage<long double>(percent);
        }
    }

    template<typename accuracy>
    [[nodiscard]] constexpr Percentage<accuracy> operator+(const Percentage<accuracy>& lhs, const Percentage<accuracy>& rhs) noexcept {
        Percentage<accuracy> temp{lhs};
        temp+=rhs;
        return temp;
    }
    template<typename accuracy>
    [[nodiscard]] constexpr Percentage<accuracy> operator*(const Percentage<accuracy>& lhs, const Percentage<accuracy>& rhs) noexcept {
        Percentage<accuracy> temp{lhs};
        temp*=rhs;
        return temp;
    }
    template<typename Skalar, typename accuracy>
    [[nodiscard]] constexpr Percentage<accuracy> operator*(const Percentage<accuracy>& lhs, const Skalar& rhs) noexcept {
        Percentage<accuracy> temp{lhs};
        temp*=rhs;
        return temp;
    }

    template<typename Skalar, typename accuracy>
    [[nodiscard]] constexpr Percentage<accuracy> operator*(const Skalar& lhs, const Percentage<accuracy>& rhs) noexcept {
        Percentage<accuracy> temp{rhs};
        temp*=lhs;
        return temp;
    }

    template<typename accuracy>
    [[nodiscard]] constexpr bool operator==(const Percentage<accuracy>& lhs, const Percentage<accuracy>& rhs) noexcept {
        return std::abs(lhs.getPercent() - rhs.getPercent()) < std::numeric_limits<accuracy>::epsilon();
    }

    template<typename accuracy>
    [[nodiscard]] constexpr bool operator!=(const Percentage<accuracy>& lhs, const Percentage<accuracy>& rhs) noexcept {
        return !(lhs == rhs);
    }

    template<typename accuracy>
    [[nodiscard]] constexpr bool operator<=(const Percentage<accuracy>& lhs, const Percentage<accuracy>& rhs) noexcept {
        return lhs.getPercent() < rhs.getPercent() || lhs == rhs;
    }

    template<typename accuracy>
    [[nodiscard]] constexpr bool operator>=(const Percentage<accuracy>& lhs, const Percentage<accuracy>& rhs) noexcept {
        return lhs.getPercent() > rhs.getPercent() || lhs == rhs;
    }
    template<typename accuracy>
    [[nodiscard]] constexpr Percentage<accuracy> operator/(const Percentage<accuracy>& lhs, const Percentage<accuracy>& rhs) noexcept {
        Percentage<accuracy> temp{lhs};
        temp/=rhs;
        return temp;
    }
    template<typename Skalar, typename accuracy>
    [[nodiscard]] constexpr Percentage<accuracy> operator/(const Percentage<accuracy>& lhs, const Skalar rhs) noexcept {
        Percentage<accuracy> temp{lhs};
        temp/=rhs;
        return temp;
    }
    template<typename Skalar, typename accuracy>
    [[nodiscard]] constexpr Percentage<accuracy> operator/(const Skalar lhs, const Percentage<accuracy>& rhs) noexcept {
        Percentage<accuracy> temp{static_cast<accuracy>(lhs)};
        temp/=(accuracy) rhs;
        return temp;
    }
    template<typename accuracy>
    [[nodiscard]] constexpr Percentage<accuracy> operator-(const Percentage<accuracy>& lhs, const Percentage<accuracy>& rhs) noexcept {
        Percentage<accuracy> temp{lhs};
        temp-=rhs;
        return temp;
    }

    template<typename accuracy>
    constexpr void swap(Percentage<accuracy>& lhs, Percentage<accuracy>& rhs) noexcept {
        lhs.swap(rhs);
    }

    template<typename accuracy>
    [[nodiscard]] constexpr std::ostream &operator<<(std::ostream &os, const Percentage<accuracy>& percentage) noexcept {
        os << percentage.getPercent() << "%";
        return os;
    }
}

