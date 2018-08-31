#pragma once
#include <vector>

namespace spi {

    enum gpioState { on, off };
    enum gpioDirection { in, out };
    class GPIOPin final {
    private:
        int value;
    public:
        GPIOPin() : value{0} {}
        constexpr explicit GPIOPin(int value): value{value} {}
        constexpr operator int() const { return value; }
    };

    class SPIBridge {
    protected:
        std::vector<GPIOPin> gpiopins;
    public:
        virtual ~SPIBridge() = default;
        virtual unsigned char transfer(unsigned char input) = 0;
        virtual std::vector<unsigned char> transfer(std::vector<unsigned char>& input) = 0;
        virtual std::vector<GPIOPin> pins() const = 0;
        virtual void setGPIODirection(const gpioDirection& direction, GPIOPin pin) = 0;
        virtual void writeGPIO(const gpioState& state, GPIOPin pin) = 0;
        virtual void slaveSelect() = 0;
        virtual void slaveDeselect() = 0;
    };
}