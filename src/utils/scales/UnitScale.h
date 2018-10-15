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

template<auto& min, auto& max, auto& functor, auto& reverse_functor, typename TValueType = uint16_t>
class NewUnitScale {
private:
    using non_ref_type = std::remove_const_t<std::remove_reference_t<decltype(min)>>;
    static_assert(min <= max);
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
        const const_iterator& operator++() const {num = num + 1; return *this;}
        const const_iterator operator++(int) const {const_iterator retval = *this; ++(*this); return retval;}
        bool operator==(const_iterator other) const {return num == other.num;}
        bool operator!=(const_iterator other) const {return !(*this == other);}
        non_ref_type operator*() const {return functor(num);}
    };
    const const_iterator begin() const { return const_iterator(0); }
    const const_iterator end() const { return const_iterator(reverse_functor(max)+1); }

    template<typename T>
    constexpr std::optional<TValueType> convertValue(const T& value) const {
        //TODO: round value up/down
        if(value >= min && value <= max) {
            return {reverse_functor(static_cast<non_ref_type>(value))};
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
            return reverse_functor(steps);
        } else {
            std::cerr << "Duration not in Range!" << std::endl;
            return std::nullopt;
        }
    }

    constexpr non_ref_type getActualValue(TValueType value) const {
        static_assert(utils::is_volt<non_ref_type>::value || utils::is_duration<non_ref_type>::value || std::is_arithmetic<non_ref_type>::value, "this type is not allowed");
        return non_ref_type{functor(value)};
    }


    constexpr non_ref_type getMaxValue() const {
        return max;
    }

    constexpr non_ref_type getMinValue() const {
        return min;
    }
};


