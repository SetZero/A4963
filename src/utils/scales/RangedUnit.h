//
// Created by sebastian on 21.09.18.
//

#pragma once

#include <type_traits>

template<auto minimum, auto maximum, auto precision>
class RangedUnit {
    static constexpr decltype(precision) value {};
    static_assert(std::is_convertible<decltype(minimum), decltype(maximum)>::value && std::is_convertible<decltype(maximum), decltype(precision)>::value);
    static_assert(minimum <= maximum);
    static_assert((maximum - minimum) % precision == 0);
    static_assert(value >= minimum && value <= maximum && value % precision == 0);
};



