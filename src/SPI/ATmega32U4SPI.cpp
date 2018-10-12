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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
    gpio::gpioState ATmega32u4SPI::readGPIO(const gpio::GPIOPin& pin) const {
#pragma GCC diagnostic pop
         //TODO: make this work
        return gpio::gpioState::off;
    }

    std::unique_ptr<Data> ATmega32u4SPI::transfer(const Data &spiData) {
        std::unique_ptr<Data> tmp = spiData.create();
        for(auto elem : spiData){
            std::vector<uint8_t> dataVector;
            dataVector.push_back(static_cast<uint8_t >(SPIRequestTypes::SendSPIData));
            dataVector.push_back(1);
            //TODO: maybe put in LibUsbDevice implentions
            //dataVector.push_back((uint8_t) spiData);
                dataVector.push_back(elem);

            auto data = mDevice.get()->sendData(dataVector);
            if(data.empty()) {
                std::cout << "There was an error with the spi data!" << std::endl;
            } else {
                if (data[0] == static_cast<unsigned char>(SPIAnswerypes::SPIAnswerWaiting)) {
                    std::cout << "Failed to send data: device not ready yet..." << std::endl;
                } else {
                    data.erase(std::begin(data));
                    for(auto element : data){
                        *tmp += element;
                    }
                }
            }
        }
        return tmp;
    }

    void ATmega32u4SPI::slaveRegister(const std::shared_ptr<SPIDevice>& device, const gpio::GPIOPin &pin) {
        setGPIODirection(gpio::gpioDirection::out, pin);
        mSlaves[device.get()] = pin;
        slaveDeselect(device);
    }

    void ATmega32u4SPI::slaveSelect(const std::shared_ptr<SPIDevice>& slave) {
        auto value = mSlaves.find(slave.get());
        if(value != std::end(mSlaves)) {
            writeGPIO(gpio::gpioState::off, value->second);
        } else {
            std::cout << "Error: No such slave found!" << std::endl;
        }
    }

    void ATmega32u4SPI::slaveDeselect(const std::shared_ptr<SPIDevice>& slave) {
        auto value = mSlaves.find(slave.get());
        if(value != std::end(mSlaves)) {
            writeGPIO(gpio::gpioState::on, value->second);
        } else {
            std::cout << "Error: No such slave found!" << std::endl;
        }
    }

    std::vector<std::unique_ptr<Data>> ATmega32u4SPI::transfer(const std::initializer_list<std::unique_ptr<Data>> &spiData) {
        std::vector<std::unique_ptr<Data>> temp{};
        for(const auto &elem : spiData){
            temp.emplace_back(transfer(*elem));
        }
        return temp;
    }
}

