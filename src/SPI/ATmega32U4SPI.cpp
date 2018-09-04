//
// Created by sebastian on 30.08.18.
//

#include "ATmega32U4SPI.h"

namespace spi {
    const usb::DeviceID ATmega32u4SPI::deviceID{0x204f};
    const usb::VendorID ATmega32u4SPI::vendorID{0x03eb};

    ATmega32u4SPI::ATmega32u4SPI(const std::shared_ptr<usb::LibUSBDevice> &device) : mDevice{device} {
        device.get()->openDevice();
    }

    void ATmega32u4SPI::setGPIODirection(const gpio::gpioDirection &direction, gpio::GPIOPin pin) {
        if(direction == gpio::gpioDirection::in) {
            mDevice.get()->sendData(
                    {
                        static_cast<uint8_t >(SPIRequestTypes::SetPortDirection),
                        static_cast<uint8_t>(pin),
                        0
                    });
        } else {
            mDevice.get()->sendData(
                    {
                        static_cast<uint8_t >(SPIRequestTypes::SetPortDirection),
                        static_cast<uint8_t>(pin),
                        1
                    });
        }
    }

    void ATmega32u4SPI::writeGPIO(const gpio::gpioState &state, gpio::GPIOPin pin) {
        if(state == gpio::gpioState::on) {
            std::cout << "on" << std::endl;
            mDevice.get()->sendData(
                    {
                        static_cast<uint8_t >(SPIRequestTypes::SetPinActive),
                        static_cast<uint8_t>(pin),
                        1
                    });
        } else {
            std::cout << "off" << std::endl;
            mDevice.get()->sendData(
                    {
                        static_cast<uint8_t >(SPIRequestTypes::SetPinActive),
                        static_cast<uint8_t>(pin),
                        0
                    });
        }
    }

    gpio::gpioState ATmega32u4SPI::readGPIO(gpio::GPIOPin pin) const {
        gpio::gpioState state = gpio::gpioState::off;
        return state;
    }

    spi::SPIData ATmega32u4SPI::transfer(const spi::SPIData &spiData) const {
        //TODO: Check if SPI data is less than 255 single data
        auto size = static_cast<uint8_t >(spiData.getData().size());
        std::vector<uint8_t> dataVector;

        dataVector.push_back(static_cast<uint8_t >(SPIRequestTypes::SendSPIData));
        dataVector.push_back(size);
        dataVector.insert(std::end(dataVector), std::begin(spiData.getData()), std::end(spiData.getData()));

        auto data = mDevice.get()->sendData(dataVector);
        return data;
    }

    void ATmega32u4SPI::slaveRegister(const SPIDevice &device, const gpio::GPIOPin &pin) {

    }

    void ATmega32u4SPI::slaveSelect(const SPIDevice &slave) {

    }

    void ATmega32u4SPI::slaveDeselect(const SPIDevice &slave) {

    }
}

