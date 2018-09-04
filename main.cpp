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
    bool blink = false;

    std::cout << "\nMCP2210 Evaluation Board Tests" << std::endl;
    std::cout << "\nParameters: " << argc << std::endl;
    for (ictr = 0; ictr < argc; ictr++)
	{
		std::cout << "\nParameter(" << ictr << ") -> " << argv[ictr] << std::endl;
	}

    if(argc > 2) {
        std::string str = argv[1];
        std::cout << "Trying to open: " << argv[1] << std::endl;

        std::unique_ptr<spi::SPIBridge> bridge = std::make_unique<MCP2210>(str);
        EEPROM eeprom{bridge};
        std::cout << "Status: " << std::hex << eeprom.readStatus() << std::endl;
    } else {
        using namespace spi::literals;

        std::cout << "Starting Atmega32u4..." << std::endl;
        usb::LibUSBDeviceList deviceList;
        std::cout << "Found " << deviceList.getDevices().size() << " devices" << std::endl;
        if(auto atmega = deviceList.findDevice(spi::ATmega32u4SPI::vendorID, spi::ATmega32u4SPI::deviceID)) {
            std::cout << "One of them was the Atmega!" << std::endl;
            spi::ATmega32u4SPI spi{*atmega};
            spi.setGPIODirection(gpio::gpioDirection::out, spi::ATmega32u4SPI::pin0);
            /*while(true) {
                if (blink) {
                    spi.writeGPIO(gpio::gpioState::on, spi::ATmega32u4SPI::pin0);
                    blink = false;
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                } else {
                    spi.writeGPIO(gpio::gpioState::off, spi::ATmega32u4SPI::pin0);
                    blink = true;
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }*/
            spi::SPIData data1 = spi.transfer(0x41_spi);
            spi::SPIData data2 = spi.transfer(0x42_spi);
            spi::SPIData data3 = spi.transfer(0x43_spi);
            spi::SPIData data4 = spi.transfer(0x44_spi);
            spi::SPIData data5 = spi.transfer(0x45_spi);
            for(auto data : data1.getData()) {
                std::cout << data << ", ";
            }
        }
    }
}



