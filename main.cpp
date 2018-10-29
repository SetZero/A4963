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
#include "src/Devices/A4963/DeserializeA4963.h"
#include "src/utils/scales/UnitScale.h"

int consoleInterface(const char* spiDevice);
int simpleInput(int min, int max);
int serverInterface(const char* spiDevice);

static inline constexpr int nrOfOptions = 5;

void clearInput();
void loadFromFile(std::shared_ptr<NS_A4963::A4963>& device);
void setRegisterVal(std::shared_ptr<NS_A4963::A4963>& device);
void showRegisterVal(std::shared_ptr<NS_A4963::A4963>& device);
void generateDefault();

static std::array<const char*,nrOfOptions> optMain {
        "1: load config from JSON",
        "2: setup Registers",
        "3: show Register value",
        "4: generate default config.json File",
        "5: exit program"
};

template<uint8_t N>
void showOptions(const std::array<const char*,N>& arr){
    std::cout << "you have the following options:" << std::endl;
    for(auto e : arr){
        std:: cout << e << std::endl;
    }
}

int main(int argc, char** argv){
    std::vector<std::string> arguments(argv, argv + argc);

    if(argc < 2 ) {
        return consoleInterface("atmega");
    } if(std::string(argv[0]) == "console") {
        if(std::string(argv[1]) == "mcp" || std::string(argv[1]) == "atmega" )
            return consoleInterface(argv[1]);
        else {
            std::cerr << "wrong argument: " << argv[1] << " it should be mcp or atmega" << std::endl;
            return -21;
        }
    } else if (std::string(argv[0]) == "server") {
        if(std::string(argv[1]) == "mcp" || std::string(argv[1]) == "atmega" )
            return serverInterface(argv[1]);
        else {
            std::cerr << "wrong argument: " << argv[1] << " it should be mcp or atmega" << std::endl;
            return -21;
        }
    }
    else {
        std::cerr << "wrong argument: " << argv[0] << " it should be console or server!" << std::endl;
        return -4711;
    }
}

int simpleInput(int min, int max){
    std::cout << "give a valid number from " << std::to_string(min) << " to " << std::to_string(max) << std::endl;
    int z = -1;
    do {
        try {
            std::string str = std::to_string(0);
            std::cin >> str;
            z = std::stoi(str);
        } catch(std::exception& e){
            std::cout << " please use a valid number! " << std::endl;
        }
    } while(z == -1);
    //clear input
    clearInput();
    return z;
}

int consoleInterface(const char* spiDevice){
    std::shared_ptr<NS_A4963::A4963> device;
    std::shared_ptr<spi::SPIBridge> spi;
    usb::LibUSBDeviceList deviceList;
    gpio::GPIOPin pin;

    if(std::string(spiDevice) == "atmega"){
        if(auto atmega = deviceList.findDevice(spi::ATmega32u4SPI::vendorID, spi::ATmega32u4SPI::deviceID)) {
            std::cout << "INDIGO!" << std::endl;
            spi = std::make_shared<spi::ATmega32u4SPI>(*atmega);
            pin = spi::ATmega32u4SPI::pin0;
        } else {
            std::cerr << "No Device Connected!" << std::endl;
            return 0;
        }
    } else {
        spi = std::make_shared<MCP2210>();
        pin = MCP2210::pin0;
    }
    device = std::make_shared<NS_A4963::A4963>(spi);
    spi->slaveRegister(device, pin);

    showOptions<nrOfOptions>(optMain);
    while(true) {
        int choice = simpleInput(1, nrOfOptions);
        switch (choice) {
            case 1: {
                loadFromFile(device);
                break;
            }
            case 2: {
                setRegisterVal(device);
                break;
            }
            case 3: {
                showRegisterVal(device);
                break;
            }
            case 4: {
                generateDefault();
                break;
            }
            case 5: {
                return 0;
            }
            default: return -42;
        }
    }
    return 0;
}

int serverInterface(const char* spiDevice){

}

inline void showRegisterVal(std::shared_ptr<NS_A4963::A4963>& device){
    std::string str;
    while(true) {
        std::cout << "type the name of the Register mask you want to read" << std::endl;
        std::getline(std::cin,str);
        if (str == "exit") break;
        try {
            auto mask = NS_A4963::RegisterStrings::get(str);
            std::cout << device->getRuntime(mask) << std::endl;
            break;
        } catch (std::exception &e) {
            std::cout << "type a valid name or exit" << std::endl;
        }
    }
}

inline void generateDefault(){
    std::cout << "this action will override the old file, type 'yes' if you are sure" << std::endl;
    std::string str;
    std::cin >> str;
    if(str != "yes") return;
    std::ofstream of{"config.json"};
    of << std::setw(4) << NS_A4963::defaultValues;
    of.close();
    std::cout << "new config file successfully generated" << std::endl;
}

inline void setRegisterVal(std::shared_ptr<NS_A4963::A4963>& device){
    std::cout << "type the Name of the Register you want to Set, or exit to cancel" << std::endl;
    std::string str;
    NS_A4963::A4963RegisterNames mask;
    while(true) {
        std::getline(std::cin,str);
        if (str == "exit") break;
        try {
            mask = NS_A4963::RegisterStrings::get(str);
            break;
        } catch (std::exception &e) {
            std::cout << "type a valid name or exit" << std::endl;
        }
    }
    std::cout << "type the value you want to set" << std::endl;
    while(true) {
        std::getline(std::cin,str);
        if (str == "exit") break;
        try {
            NS_A4963::setRuntime(*device,mask,str);
            break;
        } catch (std::exception &e) {
            std::cout << "type a valid value or exit" << std::endl;
        }
    }
}

inline void loadFromFile(std::shared_ptr<NS_A4963::A4963>& device){
    using namespace nlohmann;
    using namespace NS_A4963;

    JsonSetter s{*device, "config.json"};
    device->show_register();
}

void clearInput(){
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}