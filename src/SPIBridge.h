#pragma once
#include <vector>

namespace spi {

    enum gpioState { on, off };
    enum gpioDirection { in, out };
    class GPIOPins final {
    private:
        int value;
    public:
        GPIOPins() : value{0} {}
        constexpr explicit GPIOPins(int value): value{value} {}
        constexpr operator int() const { return value; }
    };

    static constexpr GPIOPins pin0(1 << 0);
    static constexpr GPIOPins pin1(1 << 1);
    static constexpr GPIOPins pin2(1 << 2);
    static constexpr GPIOPins pin3(1 << 3);
    static constexpr GPIOPins pin4(1 << 4);
    static constexpr GPIOPins pin5(1 << 5);
    static constexpr GPIOPins pin6(1 << 6);
    static constexpr GPIOPins pin7(1 << 7);
    static constexpr GPIOPins pin8(1 << 8);

    class SPIBridge {
    public:
        virtual ~SPIBridge() = default;
        virtual unsigned char transfer(unsigned char input) = 0;
        virtual std::vector<unsigned char> transfer(std::vector<unsigned char>& input) = 0;
        virtual void setGPIODirection(const gpioDirection& direction, GPIOPins pin) = 0;
        virtual void writeGPIO(const gpioState& state, GPIOPins pin) = 0;
        virtual void slaveSelect() = 0;
        virtual void slaveDeselect() = 0;
    };
}