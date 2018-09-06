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

void EEPROM_send16bitAddress(spi::SPIBridge& spi, uint16_t address) {
    auto first = static_cast<uint8_t>(address >> 8);
    auto second = static_cast<uint8_t>(address);
    spi.transfer({first});
    spi.transfer({second});
}

spi::SPIData EEPROM_readStatus(SPIDevice& device, spi::SPIBridge& spi) {
    using namespace spi::literals;
    spi.slaveSelect(device);
    spi.transfer(0x05_spi);
    auto data = spi.transfer(0x00_spi);
    spi.slaveDeselect(device);
    return data;
}


void EEPROM_writeEnable(SPIDevice& device, spi::SPIBridge& spi) {
    using namespace spi::literals;
    spi.slaveSelect(device);
    spi.transfer(0x06_spi);
    spi.slaveDeselect(device);
}

spi::SPIData EEPROM_readByte(SPIDevice& device, spi::SPIBridge& spi, uint16_t address) {
    using namespace spi::literals;
    spi.slaveSelect(device);
    spi.transfer(0x03_spi);
    EEPROM_send16bitAddress(spi, address);
    auto data = spi.transfer(0x00_spi);
    spi.slaveDeselect(device);
    return data;
}

void EEPROM_writeByte(SPIDevice& device, spi::SPIBridge& spi, uint16_t address, uint8_t byte) {
    using namespace spi::literals;
    EEPROM_writeEnable(device, spi);
    spi.slaveSelect(device);
    spi.transfer(0x02_spi);
    EEPROM_send16bitAddress(spi, address);
    spi.transfer({byte});
    spi.slaveDeselect(device);
    while(EEPROM_readStatus(device, spi).getData()[0] & (1 << 0)){;}
}

int main(int argc, char **argv) {
    int ictr;
    bool blink = false;

    std::cout << "\nMCP2210 Evaluation Board Tests" << std::endl;
    std::cout << "\nParameters: " << argc << std::endl;
    for (ictr = 0; ictr < argc; ictr++)
	{
		std::cout << "\nParameter(" << ictr << ") -> " << argv[ictr] << std::endl;
	}

    if(argc > 1) {
        using namespace spi::literals;
        std::string str = argv[1];
        std::cout << "Trying to open: " << argv[1] << std::endl;

        std::unique_ptr<spi::SPIBridge> bridge = std::make_unique<MCP2210>(str);
        bridge->transfer(0x23_spi);
        //EEPROM eeprom{bridge};
        //std::cout << "Status: " << std::hex << eeprom.readStatus() << std::endl;
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
            /*spi.writeGPIO(gpio::gpioState::on, spi::ATmega32u4SPI::pin0);

            spi.writeGPIO(gpio::gpioState::off, spi::ATmega32u4SPI::pin0);
            spi::SPIData data1 = spi.transfer(0x05_spi);
            spi::SPIData data2 = spi.transfer(0x00_spi);
            spi::SPIData data3 = spi.transfer(0x01_spi);
            spi::SPIData data4 = spi.transfer(0x02_spi);
            spi.writeGPIO(gpio::gpioState::on, spi::ATmega32u4SPI::pin0);*/
            std::string str = "golem";
            for(std::string::size_type i = 0; i < str.size(); i++) {
                EEPROM_writeByte(device, spi, static_cast<uint16_t>(i), static_cast<uint8_t>(str[i]));
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            for(std::string::size_type i = 0; i < str.size(); i++) {
                auto back = EEPROM_readByte(device, spi, i);
                for (auto j : back.getData()) {
                    std::cout << "Data: " << j << std::endl;
                }
            }
            //std::string str = "Guten Morgen";
            //for(std::string::size_type i = 0; i < str.size(); i++) {
                /*for(size_t i=0;i<4;i++) {
                    EEPROM_writeByte(device, spi, i, 'k');
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                    auto back = EEPROM_readByte(device, spi, i);
                    for (auto j : back.getData()) {
                        std::cout << "Data: " << j << std::endl;
                    }
                }*/
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            //}
        }
    }
}


