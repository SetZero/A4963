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
#include "src/Devices/A4963/A4963.h"
#include "src/utils/scales/UnitScale.h"

int main() {
    using namespace spi::literals;
    using namespace std::chrono_literals;

    std::cout << "Starting Atmega32u4..." << std::endl;
    usb::LibUSBDeviceList deviceList;
    std::cout << "Found " << deviceList.size() << " devices" << std::endl;
    if(auto atmega = deviceList.findDevice(spi::ATmega32u4SPI::vendorID, spi::ATmega32u4SPI::deviceID)) {
        std::cout << "One of them was the Atmega!" << std::endl;
        auto spi = std::make_shared<spi::ATmega32u4SPI>(*atmega);
        auto device = std::make_shared<NS_A4963::A4963>(spi);
        spi->slaveRegister(device, spi::ATmega32u4SPI::pin0);

        device->setRecirculationMode(NS_A4963::A4963::RecirculationModeTypes::Off);
        auto deadTimeRange = device->getRegisterRange<NS_A4963::A4963RegisterNames::DeadTime>();
        device->setDeadTime(deadTimeRange.getMinValue());

        auto blankTimeRange = device->getRegisterRange<NS_A4963::A4963RegisterNames::BlankTime>();
        device->setBlankTime(blankTimeRange.getMinValue());

        //auto thresholdVoltage = device->getRegisterRange<NS_A4963::A4963RegisterNames::CurrentSenseThresholdVoltage>();
        //double insert = 0;
        //std::cout << "Insert a value between 12.5 and 200 (mV): " << std::endl;
        //std::cin >> insert;
        //auto v = decltype(thresholdVoltage.getMinValue()){insert};
        //device->setCurrentSenseThresholdVoltage(v);
        device->commit();

        device->show_register();
    }
}


