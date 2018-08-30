//
// Created by sebastian on 30.08.18.
//

#pragma once
#include <cassert>
#include <iostream>
#include <libusb.h>
#include "SPIBridge.h"

class ATmega32u4SPI : public spi::SPIBridge {
public:
    ATmega32u4SPI();

private:
    unsigned char transfer(unsigned char input) override;
    std::vector<unsigned char> transfer(std::vector<unsigned char>& input) override;
    void setGPIODirection(const spi::gpioDirection& direction, spi::GPIOPins pin) override;
    void writeGPIO(const spi::gpioState& state, spi::GPIOPins pin) override;
    void slaveSelect() override;
    void slaveDeselect() override;
};
