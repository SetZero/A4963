//
// Created by keven on 31.08.18.
//

#pragma once

namespace gpio {


    enum class gpioState {
        on, off
    };
    enum class gpioDirection {
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
        virtual void setGPIODirection(const gpioDirection& direction,const GPIOPin& pin) = 0;
        virtual void writeGPIO(const gpioState& state,const GPIOPin& pin) = 0;
        virtual gpioState readGPIO(const GPIOPin& pin) const = 0;
    };

}