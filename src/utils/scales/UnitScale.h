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
    const const_iterator end() const { return const_iterator(inverse_functor(max)+stepsize); }

    template<typename T>
    [[nodiscard]] constexpr std::optional<TValueType> convertValue(const T& value) const noexcept {
        using namespace utils::printable;
        //TODO: round value up/down
        if(utils::approximately_greater_or_equal(value, min) &&
            utils::approximately_less_or_equal(value, max))
        {
            auto converted_value = inverse_functor(static_cast<non_ref_type>(value));
            auto actual_value = functor(converted_value);
            if(!utils::approximately_same(actual_value, value)) {
                auto next_possible_value = functor(converted_value + 1);
                if(utils::approximately_less_or_equal(next_possible_value, max) && utils::approximately_same(next_possible_value, value)) {
                    converted_value = converted_value + 1;
                } else {
                    std::cerr << "Warning! Converted value not equal to actual value ("
                              << static_cast<non_ref_type>(actual_value)
                              << " vs. " << static_cast<non_ref_type>(value) << "), next possible upper bound value: "
                              << next_possible_value << std::endl;
                }
            }
            return {converted_value};
        } else {
            std::cerr << "Maximum: " << max << ", Minimum: " << min << ", Given: " << value << std::endl;
            return std::nullopt;
        }
    }

    [[nodiscard]] constexpr non_ref_type getActualValue(TValueType value) const noexcept {
        return non_ref_type{functor(value)};
    }


    [[nodiscard]] constexpr non_ref_type getMaxValue() const noexcept {
        return max;
    }

    [[nodiscard]] constexpr non_ref_type getMinValue() const noexcept {
        return min;
    }
};


