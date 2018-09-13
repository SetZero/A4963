//
// Created by sebastian on 13.09.18.
//

#pragma once

#include <chrono>
#include <type_traits>
#include <iostream>
#include <optional>

template<typename TDuration>
struct DurationData {
    TDuration precision;
    TDuration maxValue;
    TDuration minValue;
};

template<typename TDuration, typename TValueType>
class DurationScale {
public:
    DurationScale(const DurationData<TDuration> data) : mPrecision(data.precision), mMaxValue(data.maxValue),
                                                                           mMinValue(data.minValue) {
        static_assert(is_duration<TDuration>::value, "TDuration must be of type std::chrono::duration");
    }

    template<typename Rep, typename Period>
    std::optional<TValueType> checkValue(const std::chrono::duration<Rep, Period> &value) {
        if(value >= mMinValue && value <= mMaxValue) {
            auto steps = std::chrono::duration_cast<TDuration>(value);
            return steps/ mPrecision;
        } else {
            std::cerr << "Duration not in Range!" << std::endl;
            return std::nullopt;
        }
    }

private:
    TDuration mPrecision;
    TDuration mMaxValue;
    TDuration mMinValue;

    template<class T>
    struct is_duration : std::false_type {};

    template<class Rep, class Period>
    struct is_duration<std::chrono::duration<Rep, Period>> : std::true_type {};
};



