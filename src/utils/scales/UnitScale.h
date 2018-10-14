//
// Created by sebastian on 13.09.18.
//

#pragma once

#include <chrono>
#include <type_traits>
#include <iostream>
#include <optional>
#include <stdexcept>
#include "../../CustomDataTypes/Volt.h"
#include "utils.h"

template<typename TDuration>
struct DurationData {
    TDuration precision;
    TDuration maxValue;
    TDuration minValue;
};

//TODO: maybe make min, etc... as a template parameter?
template<typename TUnitType, typename TValueType>
class UnitScale {
public:
    using value_type = TValueType;
    /*template <typename... T>
    explicit UnitScale(T... ts)
    : UnitScale(DurationData<TUnitType>{ts...})
    { }*/

    constexpr explicit UnitScale(const DurationData<TUnitType>& data) noexcept : UnitScale(data.precision, data.maxValue,
                                                                           data.minValue) {
    }

    constexpr UnitScale(const TUnitType& precision, const TUnitType& maxValue, const TUnitType& minValue) noexcept : mPrecision(precision), mMaxValue(maxValue),
                                                                              mMinValue(minValue) {
        //static_assert(maxValue <= minValue);
        //static_assert((maxValue - minValue) % precision == 0);
    }

    //TODO: return ScaleOptional and return if it is too big or too small
    template<typename T, typename Functor>
    std::optional<TValueType> convertValue(const T& value, Functor functor) {
        //TODO: round value up/down
        if(value >= mMinValue && value <= mMaxValue) {
            return {functor(static_cast<TUnitType>(value))};
        } else {
            std::cerr << "Duration not in Range!" << std::endl;
            return std::nullopt;
        }
    }

    template<typename Rep, typename Period, typename Functor>
    std::optional<TValueType> convertValue(const std::chrono::duration<Rep, Period>& value, Functor functor) {
        //TODO: round value up/down
        if(value >= mMinValue && value <= mMaxValue) {
            auto steps = std::chrono::duration_cast<TUnitType>(value);
            return functor(steps);
        } else {
            std::cerr << "Duration not in Range!" << std::endl;
            return std::nullopt;
        }
    }

    //TODO: This will not work for non linear curves! (unknown precision)
    //template<typename = std::enable_if_t<std::is_arithmetic<TValueType>::value>>
    constexpr TUnitType getActualValue(TValueType value) {
        static_assert(utils::is_volt<TUnitType>::value || utils::is_duration<TUnitType>::value || std::is_arithmetic<TUnitType>::value, "this type is not allowed");
        if constexpr (utils::is_volt<TUnitType>::value || utils::is_duration<TUnitType>::value) {
            return TUnitType{value * mPrecision.count()};
        } else {
            return TUnitType{value * mPrecision};
        }
    }

    //TODO Precision might not be needed at all...
    constexpr TUnitType getPrecision() const {
        return mPrecision;
    }

    constexpr TUnitType getMaxValue() const {
        return mMaxValue;
    }

    constexpr TUnitType getMinValue() const {
        return mMinValue;
    }


private:
    TUnitType mPrecision;
    TUnitType mMaxValue;
    TUnitType mMinValue;
};

template<auto& min, auto& max, auto& functor, auto& reverse_functor, typename TValueType = uint16_t>
class NewUnitScale {
private:
    using non_ref_type = std::remove_reference_t<decltype(min)>;
    static_assert(min <= max);
public:
    class const_iterator: public std::iterator<std::input_iterator_tag,
            non_ref_type,
            non_ref_type,
            const non_ref_type*,
            non_ref_type
    > {
    private:
        ssize_t num;
    public:
        constexpr explicit const_iterator(long _num = 0) : num(_num) {}
        const const_iterator& operator++() const {num = num + 1; return *this;}
        const const_iterator operator++(int) const {const_iterator retval = *this; ++(*this); return retval;}
        bool operator==(const_iterator other) const {return num == other.num;}
        bool operator!=(const_iterator other) const {return !(*this == other);}
        non_ref_type operator*() const {return functor(num);}
    };
    const const_iterator begin() const { return const_iterator(0); }
    const const_iterator end() const { return const_iterator(reverse_functor(max + min)); }

    template<typename T, typename Functor>
    std::optional<TValueType> convertValue(const T& value, Functor _functor) {
        //TODO: round value up/down
        if(value >= min && value <= max) {
            return {_functor(static_cast<non_ref_type>(value))};
        } else {
            std::cerr << "Duration not in Range!" << std::endl;
            return std::nullopt;
        }
    }

    template<typename Rep, typename Period, typename Functor>
    std::optional<TValueType> convertValue(const std::chrono::duration<Rep, Period>& value, Functor _functor) {
        //TODO: round value up/down
        if(value >= min && value <= max) {
            auto steps = std::chrono::duration_cast<non_ref_type>(value);
            return _functor(steps);
        } else {
            std::cerr << "Duration not in Range!" << std::endl;
            return std::nullopt;
        }
    }

    constexpr non_ref_type getActualValue(TValueType value) {
        static_assert(utils::is_volt<non_ref_type>::value || utils::is_duration<non_ref_type>::value || std::is_arithmetic<non_ref_type>::value, "this type is not allowed");
        return non_ref_type{reverse_functor(value)};
    }


    constexpr non_ref_type getMaxValue() const {
        return max;
    }

    constexpr non_ref_type getMinValue() const {
        return min;
    }
};


