//
// Created by keven on 31.08.18.
//

#pragma once

#include <stdint.h>

namespace gpio {


    enum class gpioState : uint8_t {
        on, off
    };
    enum class gpioDirection : uint8_t {
        in, out
    };

    class GPIOPin final {
    private:
        int value;
    public:
        constexpr GPIOPin() : value{0} {}

        constexpr explicit GPIOPin(int value) : value{value} {}

        constexpr operator int() const { return value; }
    };

    class GPIOBridge {
        virtual void setGPIODirection(const gpioDirection& direction, GPIOPin pin) = 0;
        virtual void writeGPIO(const gpioState& state, GPIOPin pin) = 0;
        virtual gpioState readGPIO(GPIOPin pin) const = 0;
    };

}