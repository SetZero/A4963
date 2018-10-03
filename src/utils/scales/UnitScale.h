//
// Created by sebastian on 13.09.18.
//

#pragma once

#include <chrono>
#include <type_traits>
#include <iostream>
#include <optional>
#include "../../CustomDataTypes/Volt.h"
#include "utils.h"

template<typename TDuration>
struct DurationData {
    TDuration precision;
    TDuration maxValue;
    TDuration minValue;
};

template<typename TUnitType, typename TValueType>
class UnitScale {
public:
    /*template <typename... T>
    explicit UnitScale(T... ts)
    : UnitScale(DurationData<TUnitType>{ts...})
    { }*/

    explicit UnitScale(const DurationData<TUnitType>& data) : UnitScale(data.precision, data.maxValue,
                                                                           data.minValue) {
    }

    UnitScale(TUnitType precision, TUnitType maxValue, TUnitType minValue) : mPrecision(precision), mMaxValue(maxValue),
                                                                              mMinValue(minValue) {
        static_assert(utils::is_duration<TUnitType>::value || utils::is_volt<TUnitType>::value, "TUnitType must be of type std::chrono::duration" \
                                                                                  "or CustomDataTypes::Electricity::Volt");
    }

    //TODO: return ScaleOptional and return if it is too big or too small
    template<typename Rep, typename Period>
    std::optional<TValueType> convertValue(const CustomDataTypes::Electricity::Volt<Rep, Period>& value) {
        //TODO: round value up/down
        if(value >= mMinValue && value <= mMaxValue) {
            auto steps = static_cast<TUnitType>(value);
            return steps.count() / mPrecision.count();
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

    TUnitType getActualValue(TValueType value) {
        return TUnitType{value * mPrecision.count()};
    }

    template<template<typename, typename> typename E, typename Rep, typename Period>
    E<Rep, Period> getActualValue(TValueType value) {
        if constexpr (utils::is_duration<E<Rep, Period>>::value) {
            return std::chrono::duration_cast(TUnitType{value * mPrecision.count()});
        } else {
            return static_cast<E<Rep, Period>>(TUnitType{value * mPrecision.count()});
        }
    }

    TUnitType getPrecision() const {
        return mPrecision;
    }

    TUnitType getMaxValue() const {
        return mMaxValue;
    }

    TUnitType getMinValue() const {
        return mMinValue;
    }


private:
    TUnitType mPrecision;
    TUnitType mMaxValue;
    TUnitType mMinValue;
};



