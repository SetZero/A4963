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

    void ATmega32u4SPI::setGPIODirection(const gpio::gpioDirection &direction,const gpio::GPIOPin& pin) {
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

    void ATmega32u4SPI::writeGPIO(const gpio::gpioState &state,const gpio::GPIOPin& pin) {
        if(state == gpio::gpioState::on) {
            mDevice.get()->sendData(
                    {
                        static_cast<uint8_t >(SPIRequestTypes::SetPinActive),
                        static_cast<uint8_t>(pin),
                        1
                    });
        } else {
            mDevice.get()->sendData(
                    {
                        static_cast<uint8_t >(SPIRequestTypes::SetPinActive),
                        static_cast<uint8_t>(pin),
                        0
                    });
        }
    }

    gpio::gpioState ATmega32u4SPI::readGPIO(const gpio::GPIOPin& pin) const {
        auto state = gpio::gpioState::off; //TODO: make this work
        return state;
    }

    std::shared_ptr<SPIData> ATmega32u4SPI::transfer(const SPIData &spiData) {
        /*for(auto data : spiData.getData()) {
            std::cout << "sent: " << static_cast<int >(data) << std::endl;
        }*/
        //TODO: Check if SPI data is less than 255 single data
        //TODO: allow multi byte transfer for spiData!
        //auto size = static_cast<uint8_t >(spiData.getData().size());
        std::vector<uint8_t> dataVector;

        dataVector.push_back(static_cast<uint8_t >(SPIRequestTypes::SendSPIData));
        dataVector.push_back(1);
        //dataVector.push_back((uint8_t) spiData);
        for(auto elem : spiData){
            dataVector.push_back(elem);
        }
        //dataVector.insert(std::end(dataVector), std::begin(spiData.getData()), std::end(spiData.getData()));

        auto data = mDevice.get()->sendData(dataVector);
        if(data.empty()) {
            std::cout << "There was an error with the spi data!" << std::endl;
        } else {
            if (data[0] == static_cast<unsigned char>(SPIAnswerypes::SPIAnswerWaiting)) {
                std::cout << "Failed to send data: device not ready yet..." << std::endl;
            } else {
                data.erase(std::begin(data));
                /*for(auto dat : data) {
                    std::cout << static_cast<int>(dat) << std::endl;
                }*/
            }
        }
        return std::make_shared<spi8>(data[0]);
    }

    void ATmega32u4SPI::slaveRegister(std::shared_ptr<SPIDevice> device, const gpio::GPIOPin &pin) {
        setGPIODirection(gpio::gpioDirection::out, pin);
        mSlaves[device] = pin;
        slaveDeselect(device);
    }

    void ATmega32u4SPI::slaveSelect(std::shared_ptr<SPIDevice> slave) {
        auto value = mSlaves.find(slave);
        if(value != std::end(mSlaves)) {
            writeGPIO(gpio::gpioState::off, value->second);
        } else {
            std::cout << "Error: No such slave found!" << std::endl;
        }
    }

    void ATmega32u4SPI::slaveDeselect(std::shared_ptr<SPIDevice> slave) {
        auto value = mSlaves.find(slave);
        if(value != std::end(mSlaves)) {
            writeGPIO(gpio::gpioState::on, value->second);
        } else {
            std::cout << "Error: No such slave found!" << std::endl;
        }
    }

    std::vector<std::shared_ptr<SPIData>> ATmega32u4SPI::transfer(const std::initializer_list<std::shared_ptr<SPIData>> &spiData) {
        std::vector<std::shared_ptr<SPIData>> temp{};
        for(const auto elem : spiData){
            temp.emplace_back(transfer(*elem));
        }
        return temp;
    }
}

