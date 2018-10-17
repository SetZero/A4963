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

template<auto& min, auto& max, auto& functor, auto& inverse_functor, typename TValueType = uint16_t, TValueType stepsize = 1>
class NewUnitScale {
    static_assert(std::is_invocable_v<decltype(functor), TValueType>);
    static_assert(std::is_invocable_v<decltype(inverse_functor), decltype(min)>);
    static_assert(min <= max);
    static_assert(std::is_arithmetic_v<TValueType>);
    static_assert(stepsize > 0);

private:
    using non_ref_type = std::remove_const_t<std::remove_reference_t<decltype(min)>>;
public:
    class const_iterator: public std::iterator<std::input_iterator_tag,
            non_ref_type,
            non_ref_type,
            const non_ref_type*,
            non_ref_type
    > {
    private:
        size_t num;
    public:
        explicit const_iterator(unsigned long _num = 0) : num(_num) {}
        const const_iterator& operator++() const {num = num + stepsize; return *this;}
        const const_iterator operator++(int) const {const_iterator retval = *this; ++(*this); return retval;}
        bool operator==(const_iterator other) const {return num == other.num;}
        bool operator!=(const_iterator other) const {return !(*this == other);}
        non_ref_type operator*() const {return functor(num);}
    };
    const const_iterator begin() const { return const_iterator(0); }
    const const_iterator end() const { return const_iterator(inverse_functor(max)+1); }

    template<typename T>
    constexpr std::optional<TValueType> convertValue(const T& value) const {
        //TODO: round value up/down
        if(value >= min && value <= max) {
            return {inverse_functor(static_cast<non_ref_type>(value))};
        } else {
            std::cerr << "Duration not in Range!" << std::endl;
            return std::nullopt;
        }
    }

    template<typename Rep, typename Period>
    constexpr std::optional<TValueType> convertValue(const std::chrono::duration<Rep, Period>& value) const {
        //TODO: round value up/down
        if(value >= min && value <= max) {
            auto steps = std::chrono::duration_cast<non_ref_type>(value);
            return inverse_functor(steps);
        } else {
            std::cerr << "Maximum: " << max.count() << ", Minimum: " << min.count() << ", Given: " << value.count() << std::endl;
            std::cerr << "Duration not in Range!" << std::endl;
            return std::nullopt;
        }
    }

    constexpr non_ref_type getActualValue(TValueType value) const {
        return non_ref_type{functor(value)};
    }


    constexpr non_ref_type getMaxValue() const {
        return max;
    }

    constexpr non_ref_type getMinValue() const {
        return min;
    }
};


