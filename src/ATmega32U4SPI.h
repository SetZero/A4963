//
// Created by sebastian on 30.08.18.
//

#pragma once
#include <cassert>
#include <iostream>
#include <libusb.h>
#include <memory>
#include "SPIBridge.h"
#include "GPIOBridge.h"
#include "LibUSBDevices.h"

class ATmega32u4SPI : public spi::SPIBridge, public gpio::GPIOBridge {
public:
    static const uint16_t deviceID = 0x204f;
    static const uint16_t vendorID = 0x03eb;

    explicit ATmega32u4SPI(const std::shared_ptr<LibUSBDevice>& device);
    void setGPIODirection(const gpio::gpioDirection& direction, gpio::GPIOPin pin) override;
    void writeGPIO(const gpio::gpioState& state, gpio::GPIOPin pin) override;
    gpio::gpioState readGPIO(gpio::GPIOPin pin) const override;
    spi::SPIData transfer(const spi::SPIData& spiData) const override;
    void slaveRegister(const SPIDevice& device, const gpio::GPIOPin& pin) override;
    void slaveSelect(const SPIDevice& slave) override;
    void slaveDeselect(const SPIDevice& slave) override;
private:
    std::shared_ptr<LibUSBDevice> mDevice;
};
