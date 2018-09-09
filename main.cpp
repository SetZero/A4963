#include <iostream>
#include <array>
#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <thread>
#include "LibUSBDevices.h"
#include "src/SPI/ATmega32U4SPI.h"
#include "src/SPI/SPIBridge.h"
#include "mcp2210_api.h"
#include "src/SPI/mcp2210_hal.h"
#include "src/25LC256.h"

int main(int argc, char **argv) {
    int ictr;

    std::cout << "\nMCP2210 Evaluation Board Tests" << std::endl;
    std::cout << "\nParameters: " << argc << std::endl;
    for (ictr = 0; ictr < argc; ictr++)
	{
		std::cout << "\nParameter(" << ictr << ") -> " << argv[ictr] << std::endl;
	}

    using namespace spi::literals;
    std::cout << "Starting Atmega32u4..." << std::endl;
    usb::LibUSBDeviceList deviceList;
    std::cout << "Found " << deviceList.getDevices().size() << " devices" << std::endl;
    if(auto atmega = deviceList.findDevice(spi::ATmega32u4SPI::vendorID, spi::ATmega32u4SPI::deviceID)) {
        std::cout << "One of them was the Atmega!" << std::endl;
        auto spi = std::make_shared<spi::ATmega32u4SPI>(*atmega);
        auto device = std::make_shared<EEPROM>(spi);
        spi->slaveRegister(device, spi::ATmega32u4SPI::pin0);
        spi->setGPIODirection(gpio::gpioDirection::out, spi::ATmega32u4SPI::pin0);

        std::string str2 = "Other Text!";
        for(std::string::size_type i = 0; i < str2.size(); i++) {
            device->writeByte(static_cast<uint16_t>(i), static_cast<uint8_t>(str2[i]));
        }
        for(std::string::size_type i = 0; i < str2.size(); i++) {
            auto back = device->readByte(i);
            std::cout << "Data: " << back.getData()[0] << std::endl;
        }
    }

	//TODO: remove redundancy
    /*if(argc > 1) {
        using namespace spi::literals;
        std::string str = argv[1];
        std::cout << "Trying to open: " << argv[1] << std::endl;

        MCP2210 spi{str};
        SPIDevice device{};
        spi.slaveRegister(device, MCP2210::pin0);
        spi.setGPIODirection(gpio::gpioDirection::out, MCP2210::pin0);

        std::string str2 = "abc";
        for(std::string::size_type i = 0; i < str2.size(); i++) {
            EEPROM_writeByte(device, spi, static_cast<uint16_t>(i), static_cast<uint8_t>(str2[i]));
        }
        for(std::string::size_type i = 0; i < str2.size(); i++) {
            auto back = EEPROM_readByte(device, spi, i);
            std::cout << "Data: " << back.getData()[0] << std::endl;
        }
    } else {
        using namespace spi::literals;

        std::cout << "Starting Atmega32u4..." << std::endl;
        usb::LibUSBDeviceList deviceList;
        std::cout << "Found " << deviceList.getDevices().size() << " devices" << std::endl;
        if(auto atmega = deviceList.findDevice(spi::ATmega32u4SPI::vendorID, spi::ATmega32u4SPI::deviceID)) {
            std::cout << "One of them was the Atmega!" << std::endl;
            spi::ATmega32u4SPI spi{*atmega};
            SPIDevice device{};
            spi.slaveRegister(device, spi::ATmega32u4SPI::pin0);
            spi.setGPIODirection(gpio::gpioDirection::out, spi::ATmega32u4SPI::pin0);

            std::string str2 = "Welcome to my World!";
            for(std::string::size_type i = 0; i < str2.size(); i++) {
                EEPROM_writeByte(device, spi, static_cast<uint16_t>(i), static_cast<uint8_t>(str2[i]));
            }
            for(std::string::size_type i = 0; i < str2.size(); i++) {
                auto back = EEPROM_readByte(device, spi, i);
                std::cout << "Data: " << back.getData()[0] << std::endl;
            }
        }
    }*/
}


