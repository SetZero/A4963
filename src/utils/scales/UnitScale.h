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
    /*template <typename... T>
    explicit UnitScale(T... ts)
    : UnitScale(DurationData<TUnitType>{ts...})
    { }*/

    constexpr explicit UnitScale(const DurationData<TUnitType>& data) : UnitScale(data.precision, data.maxValue,
                                                                           data.minValue) {
    }

    constexpr UnitScale(const TUnitType& precision, const TUnitType& maxValue, const TUnitType& minValue) : mPrecision(precision), mMaxValue(maxValue),
                                                                              mMinValue(minValue) {
        //static_assert(maxValue <= minValue);
        //static_assert((maxValue - minValue) % precision == 0);
    }

    //TODO: return ScaleOptional and return if it is too big or too small
    template<typename T>
    std::optional<TValueType> convertValue(const T& value) {
        //TODO: round value up/down
        if(value >= mMinValue && value <= mMaxValue) {
            return {static_cast<TValueType>(static_cast<TUnitType>(value / mPrecision))};
        } else {
            std::cerr << "Duration not in Range!" << std::endl;
            return std::nullopt;
        }
    }

    template<typename Rep, typename Period>
    std::optional<TValueType> convertValue(const std::chrono::duration<Rep, Period>& value) {
        //TODO: round value up/down
        if(value >= mMinValue && value <= mMaxValue) {
            auto steps = std::chrono::duration_cast<TUnitType>(value);
            return steps / mPrecision;
        } else {
            std::cerr << "Duration not in Range!" << std::endl;
            return std::nullopt;
        }
    }

    //template<typename = std::enable_if_t<std::is_arithmetic<TValueType>::value>>
    constexpr TUnitType getActualValue(TValueType value) {
        if constexpr (utils::is_volt<TUnitType>::value || utils::is_duration<TUnitType>::value) {
            return TUnitType{value * mPrecision.count()};
        } else if (std::is_arithmetic<TUnitType>::value) {
            return TUnitType{value * mPrecision};
        }
    }

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



