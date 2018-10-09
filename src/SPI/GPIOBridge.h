//
// Created by keven on 31.08.18.
//

#pragma once

#include <stdint.h>

namespace gpio {


    enum class gpioState {
        on, off
    };
    enum class gpioDirection {
        in, out
    };

    class GPIOPin final {
    private:
        int value = 0;
        bool isUsed = false;
    public:
        constexpr GPIOPin() = default;

        constexpr explicit GPIOPin(int value) : value{value},isUsed(true) {}

        constexpr explicit operator int() const { return value; }

        constexpr explicit operator uint8_t() const { return static_cast<uint8_t>(value); }

        constexpr explicit operator uint16_t() const { return static_cast<uint16_t>(value); }

        constexpr explicit operator uint32_t() const { return static_cast<uint32_t>(value); }


        constexpr explicit operator bool() const {return isUsed;}
    };

    class GPIOBridge {

    public:
        virtual ~GPIOBridge() = default;
        virtual void setGPIODirection(const gpioDirection& direction,const GPIOPin& pin) = 0;
        virtual void writeGPIO(const gpioState& state,const GPIOPin& pin) = 0;
        virtual gpioState readGPIO(const GPIOPin& pin) const = 0;
    };

}