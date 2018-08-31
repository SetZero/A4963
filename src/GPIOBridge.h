//
// Created by keven on 31.08.18.
//

#pragma once

namespace gpio {


    enum gpioState {
        on, off
    };
    enum gpioDirection {
        in, out
    };

    class GPIOPin final {
    private:
        int value;
    public:
        constexpr GPIOPin() : value{0} {}

        constexpr explicit GPIOPin(int value) : value{value} {}

        constexpr explicit operator int() const { return value; }
    };

    class GPIOBridge {
        virtual void setGPIODirection(const gpioDirection& direction, GPIOPin pin) = 0;
        virtual void writeGPIO(const gpioState& state, GPIOPin pin) = 0;
    };

}