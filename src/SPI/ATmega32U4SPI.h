//
// Created by sebastian on 30.08.18.
//

#pragma once
#include <cassert>
#include <iostream>
#include <libusb.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include "SPIBridge.h"
#include "GPIOBridge.h"
#include "src/USB/LibUsbDevice.h"

namespace spi {
    class ATmega32u4SPI : public spi::SPIBridge, public gpio::GPIOBridge {
    public:
        static const usb::DeviceID deviceID;
        static const usb::VendorID vendorID;

        explicit ATmega32u4SPI(const std::shared_ptr<usb::LibUSBDevice> &device);
        void setGPIODirection(const gpio::gpioDirection &direction,const gpio::GPIOPin& pin) override;
        void writeGPIO(const gpio::gpioState &state,const gpio::GPIOPin& pin) override;
        gpio::gpioState readGPIO(const gpio::GPIOPin& pin) const override;
        std::unique_ptr<Data> transfer(const Data& spiData)  override;
        std::vector<std::unique_ptr<Data>> transfer(const std::initializer_list<std::unique_ptr<Data>>& spiData)  override;
        void slaveRegister(const std::shared_ptr<SPIDevice>& device, const gpio::GPIOPin &pin) override;
        void slaveSelect(const std::shared_ptr<SPIDevice>& slave) override;
        void slaveDeselect(const std::shared_ptr<SPIDevice>& slave) override;

        static constexpr gpio::GPIOPin
                pin0 = gpio::GPIOPin(0), pin1 = gpio::GPIOPin(1<<0), pin2 = gpio::GPIOPin(1<<1), pin3 = gpio::GPIOPin(1<<2),
                pin4 = gpio::GPIOPin(1<<3);
    private:
        std::shared_ptr<usb::LibUSBDevice> mDevice;
        std::map<SPIDevice*, gpio::GPIOPin> mSlaves;

        enum class SPIRequestTypes {
            SendSPIData = 1,
            SetPortDirection = 2,
            SetPinActive = 3
        };

        enum class SPIAnswerypes {
            SPIAnswerWaiting = 0,
            SPIAnswerOK = 1
        };
    };
};