//
// Created by sebastian on 19.09.18.
//

#pragma once

#include <cstdint>
#include <type_traits>
#include <optional>

namespace utils {
    enum class RangeCheck : uint8_t {
        TooBig,
        TooSmall,
        InRange
    };

    template<typename T>
    struct ScaleOptional {
        RangeCheck init;
        std::optional<T> data;
    };
}


