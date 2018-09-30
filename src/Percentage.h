//
// Created by keven on 30.09.18.
//

#pragma once
#include "utils.h"

namespace math {
    struct Accuracy {
        struct Bit32 {
        };
        struct Bit64 {
        };
    };

    template<typename accuracy>
    class Percentage {
        using acc = typename utils::conditional<utils::isEqual<accuracy, Accuracy::Bit32>::value, float,
                typename utils::conditional<utils::isEqual<accuracy, Accuracy::Bit64>::value, double, void>::type>::type;
        static_assert(!utils::isEqual<void, acc>::value, "wrong type, only Accuracy::Bit32||Bit64 allowed");
    public:
        inline Percentage() = default;
    };
}

