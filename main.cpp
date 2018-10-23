#include <iostream>
#include <array>
#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <thread>
#include <src/CustomDataTypes/Percentage.h>
#include "LibUSBDevices.h"
#include "src/SPI/ATmega32U4SPI.h"
#include "src/SPI/SPIBridge.h"
#include "inc/json.h"
#include <fstream>
#include <iomanip>
#ifdef __linux__
    #include "mcp2210_api.h"
    #include "src/SPI/mcp2210_hal.h"
#endif
#include "src/25LC256.h"
#include "src/Devices/A4963/A4963.h"
#include "src/utils/scales/UnitScale.h"


bool reconnect( std::unique_ptr<MCP2210>& ptr);
int userInput();
void clearInput();

enum Chips {
    ATMEGA, MCP
};
static constexpr Chips used_chip = Chips::ATMEGA;

int atmega_main() {
    using namespace spi::literals;
    using namespace std::chrono_literals;
    using namespace NS_A4963;
    std::cout << "Starting Atmega32u4..." << std::endl;
    usb::LibUSBDeviceList deviceList;
    std::cout << "Found " << deviceList.size() << " devices" << std::endl;
    if(auto atmega = deviceList.findDevice(spi::ATmega32u4SPI::vendorID, spi::ATmega32u4SPI::deviceID)) {
        std::cout << "One of them was the Atmega!" << std::endl;
        auto spi = std::make_shared<spi::ATmega32u4SPI>(*atmega);
        auto device = std::make_shared<NS_A4963::A4963>(spi);
        spi->slaveRegister(device, spi::ATmega32u4SPI::pin0);
        constexpr auto recirculationMode = A4963RegisterNames::RecirculationMode;
        device->set<recirculationMode>(possibleValues<recirculationMode>::values::Off);
        auto blankTime = device->getRegisterRange<NS_A4963::A4963RegisterNames::BlankTime>();
        device->set<NS_A4963::A4963RegisterNames::BlankTime>(blankTime.getMaxValue());
        auto deadTime = device->getRegisterRange<NS_A4963::A4963RegisterNames::DeadTime>();
        device->set<NS_A4963::A4963RegisterNames::DeadTime>(deadTime.getMaxValue());
        device->commit();
        setRuntime(*device,A4963RegisterNames::BlankTime,4711);
        auto cstv = device->getRegEntry<NS_A4963::A4963RegisterNames::CurrentSenseThresholdVoltage>();
        std::cout << "Current Sense Threshold: " << cstv << std::endl;

        auto mxspd = device->getRegEntry<NS_A4963::A4963RegisterNames::MaximumSpeedSetting>();
        std::cout << "Maximum Speed Setting: " << mxspd << std::endl;

        auto rcn = device->getRegEntry<NS_A4963::A4963RegisterNames::RecirculationMode>();
        std::cout << "Recirculation Mode: " << static_cast<int>(rcn) << std::endl;
        device->show_register();
/*
        auto blankTimeRange = device->getRegisterRange<NS_A4963::A4963RegisterNames::BlankTime>();
        device->setBlankTime(blankTimeRange.getMinValue());

        auto csThresholdVoltage = device->getRegisterRange<NS_A4963::A4963RegisterNames::CurrentSenseThresholdVoltage>();
        //double insert = 0;
        //std::cout << "Insert a value between 12.5 and 200 (mV): " << std::endl;
        //std::cin >> insert;
        //auto v = decltype(thresholdVoltage.getMinValue()){insert};
        device->setCurrentSenseThresholdVoltage(csThresholdVoltage.getMaxValue());

        auto vdsThresholdVoltage = device->getRegisterRange<NS_A4963::A4963RegisterNames::VDSThreshold>();
        device->setVDSThreshold(vdsThresholdVoltage.getMaxValue());
        device->commit();

        device->show_register();*/
    }
    return 0;
}

int mcp_main(){
    using namespace CustomDataTypes::literals;

    using namespace nlohmann;
    json j{"VDS Threshold","Mask: ",0b0000000000011111,"Register: ",0b001, "value: ", 42};
    json i; i.push_back(j);i.push_back(j);i.push_back(j);i.push_back(j);i.push_back(j);
    std::ofstream of = std::ofstream("test.json");
    of << std::setw(2) << i << std::endl;
    of.close();
    std::cout << "this is sparta!" << std::endl;
    std::unique_ptr<MCP2210> ptr;
    using namespace spi::literals;
    ptr = std::make_unique<MCP2210>();
    ptr->setGPIODirection(gpio::gpioDirection::in,MCP2210::pin3);
    while(true){
        int z = 0;
        std::cin >> z;
        if(z == 1)
            std::cout << "pin3: " << (ptr->readGPIO(MCP2210::pin3) == gpio::gpioState::off ? "0" : "1") << std::endl;
        else break;
    }
    auto eingaben = {"spi test", "eingabe test", "exit"};
    while(true) {
        if (*ptr) {
            std::cout << "mögliche eingaben: " << std::endl;
            size_t i = 0;
            for (auto val : eingaben) {
                std::cout << i << ".: " << val << std::endl;
                i++;
            }
            switch (userInput()) {
                case 0: {
                    for (int j = 0; j < 10; j++) {
                        using namespace spi::literals;
                        try {
                            if (*ptr)
                                ptr->transfer({42_spi8, 21_spi8});
                            else {
                                std::cout << " no connection ";
                                reconnect(ptr);
                            }
                        } catch (MCPIOException &e) {
                            std::cout << e.what();
                            if (!reconnect(ptr)) break;
                        }
                    }
                    break;
                }
                case 1: {
                    std::cout << "give your data (no whitespace): ";
                    std::string input{};
                    std::cin >> input;
                    clearInput();
                    for (auto ch: input) {
                        try {
                            if (*ptr)
                                ptr->transfer(spi::SPIData(ch));
                            else {
                                std::cout << " no connection ";
                                reconnect(ptr);
                            }
                        } catch (MCPIOException &e) {
                            std::cout << e.what();
                            reconnect(ptr);
                        }
                    }
                    break;
                }
                case 2 : {
                    return 0;
                }
                default:
                    break;
            }
        } else {
            std::cout
                    << "device currently not connected: type 0 to attempt connections or anything else to exit the application"
                    << std::endl;
            if (userInput()) {
                exit(1);
            } else reconnect(ptr);
        }
    }
}

//int argc, char **argv
int main() {
    if constexpr (used_chip == Chips::ATMEGA) {
        return atmega_main();
    } else if(used_chip == Chips::MCP) {
        return mcp_main();
    }
}


bool reconnect( std::unique_ptr<MCP2210>& ptr){
    do {
        std::cout << " type sth to attempt a reconnect ('break' for stop trying) " << std::endl;
        std::string z;
        std::cin >> z;
        if(z == "break") break;
        ptr->connect();
    } while(!(*ptr));
    return *ptr;
}

int userInput(){
    int z = -1;
    do {
        try {
            std::string str = std::to_string(0);
            std::cin >> str;
            z = std::stoi(str);
        } catch(std::exception& e){
            std::cerr << e.what();
            std::cout << " please use a valid number! " << std::endl;
        }
    } while(z == -1);
    clearInput();
    return z;
}

void clearInput(){
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}