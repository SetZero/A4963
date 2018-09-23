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

int main(int argc, char **argv) {
    int ictr;

    std::cout << "\nMCP2210 Evaluation Board Tests" << std::endl;
    std::cout << "\nParameters: " << argc << std::endl;
    for (ictr = 0; ictr < argc; ictr++)
	{
		std::cout << "\nParameter(" << ictr << ") -> " << argv[ictr] << std::endl;
	}

	//MCP2210 spi{str};

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

        //device->setRecirculationMode(A4963::RecirculationModeTypes::High);
        //device->commit();

        auto minValue = device->getRegisterRange<NS_A4963::A4963RegisterNames::DeadTime>().getMMinValue();
        auto actldedtime = device->setDeadTime(minValue);
        std::cout << "Actual Dead Time: " << (*actldedtime).count() << "ns" <<  std::endl;
        auto actlblktime = device->setBlankTime(90ns);
        std::cout << "Actual Blank Time: " << std::chrono::nanoseconds(*actlblktime).count() << "ns" <<  std::endl;
        device->commit();

        device->show_register();

        //std::string str2 = "Other Text!";
        //for(std::string::size_type i = 0; i < str2.size(); i++) {
        //    device->writeByte(static_cast<uint16_t>(i), static_cast<uint8_t>(str2[i]));
        //}
        //for(std::string::size_type i = 0; i < str2.size(); i++) {
        //    auto back = device->readByte(static_cast<uint16_t>(i));
        //    std::cout << "Data: " << back.getData()[0] << std::endl;
        //}
    }
}


