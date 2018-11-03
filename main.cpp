#include <iostream>
#include <array>
#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <thread>
#include <src/CustomDataTypes/Percentage.h>
#include <fstream>
#include <iomanip>
#include <cxxopts.hpp>
#include "LibUSBDevices.h"
#include "src/SPI/ATmega32U4SPI.h"
#include "src/SPI/SPIBridge.h"
#include "inc/json.h"
#ifdef __linux__
    #include "mcp2210_api.h"
    #include "src/SPI/mcp2210_hal.h"
#endif
#include "src/25LC256.h"
#include "src/Devices/A4963/A4963.h"
#include "src/Devices/A4963/DeserializeA4963.h"
#include "src/utils/scales/UnitScale.h"

int consoleInterface(const std::string& spiDevice);
void flashJSON(const std::string& spiDevice, const std::string& filename, bool enable_debug = false);
int simpleInput(int min, int max);
int serverInterface(const char* spiDevice);
void parseArguments(int argc, char** argv);

static inline constexpr int nrOfOptions = 5;

void clearInput();
void loadFromFile(std::shared_ptr<NS_A4963::A4963>& device, const std::string& filename);
void setRegisterVal(std::shared_ptr<NS_A4963::A4963>& device);
void showRegisterVal(std::shared_ptr<NS_A4963::A4963>& device);
bool generateDefault(bool force = false, const std::string& filename = "config.json");

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
    parseArguments(argc, argv);
}

void parseArguments(int argc, char** argv) {
    cxxopts::Options options("hid", "An A4963 USB configuration program");
    options.add_options()
            ("h,help", "show help")
            ("d,debug", "Enable debugging mode")
            ("j,json", "read JSON file", cxxopts::value<std::string>()->implicit_value("config.json"))
            ("g,generate", "generate JSON file, will take -j location if given")
            ("f,force", "override current JSON file if there is any")
            ("c,client", "use interactive client interface")
            ("i,interface", "select USB to SPI Bridge", cxxopts::value<std::string>()->implicit_value("mcp"));

    auto result = options.parse(argc, argv);
    if(result.count("debug") > 0) {
        std::cout << "Debugging option fully implemented yet..." << std::endl;
    }

    if(result.count("generate") > 0) {
        std::cout << "Generating JSON File" << std::endl;
        if(result.count("force") > 0) {
            std::cout << "Forcing generation..." << std::endl;
            if(result.count("json") > 0) {
                std::cout << "Taking " << result["json"].as<std::string>() << " as location..." << std::endl;
                generateDefault(true, result["json"].as<std::string>());
            } else {
                generateDefault(true);
            }
        } else {
            if(result.count("json") > 0) {
                generateDefault(false, result["json"].as<std::string>());
            } else {
                generateDefault(false);
            }
        }
    }

    if(result.count("client") > 0 && result.count("interface") > 0) {
        auto interface = result["interface"].as<std::string>();
        consoleInterface(interface);
        return;
    } else if(result.count("client") > 0 && result.count("interface") <= 0) {
        std::cout << "Missing Parameter: --interface!" << std::endl;
    }

    if(result.count("json") > 0 && result.count("interface") > 0) {
        auto json_filename  = result["json"].as<std::string>();
        auto interface_name = result["interface"].as<std::string>();
        if(result.count("debug") > 0) {
            flashJSON(interface_name, json_filename, true);
        } else {
            flashJSON(interface_name, json_filename);
        }
    } else {
        if(result.count("json") > 0 && result.count("interface") <= 0) {
            std::cout << "Missing parameter --interface [atmega|mcp]" << std::endl;
        } else if(result.count("json") <= 0 && result.count("interface") > 0) {
            std::cout << "Missing parameter --json [filename]" << std::endl;
        }
    }
    if(result.arguments().empty() || result.count("help") > 0) {
        std::cout << options.help() << std::endl;
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

void flashJSON(const std::string& spiDevice, const std::string& filename, bool enable_debug) {
    std::cout << "Device name: " << spiDevice << std::endl;
    std::shared_ptr<NS_A4963::A4963> device;
    std::shared_ptr<spi::SPIBridge> spi;
    usb::LibUSBDeviceList deviceList;
    gpio::GPIOPin pin;

    if(spiDevice == "atmega"){
        if(auto atmega = deviceList.findDevice(spi::ATmega32u4SPI::vendorID, spi::ATmega32u4SPI::deviceID)) {
            std::cout << "INDIGO!" << std::endl;
            spi = std::make_shared<spi::ATmega32u4SPI>(*atmega);
            pin = spi::ATmega32u4SPI::pin0;
        } else {
            std::cerr << "No Device Connected!" << std::endl;
            return;
        }
    } else {
        spi = std::make_shared<MCP2210>();
        pin = MCP2210::pin0;
    }
    if(enable_debug) {
        std::cout << "[DEBUG] Disabled Cache!" << std::endl;
        device = std::make_shared<NS_A4963::A4963>(spi, true);
    } else {
        device = std::make_shared<NS_A4963::A4963>(spi);
    }
    spi->slaveRegister(device, pin);

    loadFromFile(device, filename);
}

int consoleInterface(const std::string& spiDevice){
    std::shared_ptr<NS_A4963::A4963> device;
    std::shared_ptr<spi::SPIBridge> spi;
    usb::LibUSBDeviceList deviceList;
    gpio::GPIOPin pin;

    if(spiDevice == "atmega"){
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
                loadFromFile(device, "config.json");
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
    if(std::string(spiDevice) == "mcp")
        return 1;
 return 0;
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

bool generateDefault(bool force, const std::string& filename){
    std::ofstream of{filename};
    bool return_value = false;
    if(of.good() && force) {
        of << std::setw(4) << NS_A4963::defaultValues;
        return_value = true;
        std::cout << "new config file successfully generated (" << filename << ")" << std::endl;
    } else {
        std::cout << "File already exists! use -f to force overwrite" << std::endl;
    }
    of.close();
    return return_value;
}

void setRegisterVal(std::shared_ptr<NS_A4963::A4963>& device){
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

void loadFromFile(std::shared_ptr<NS_A4963::A4963>& device, const std::string& filename){
    using namespace nlohmann;
    using namespace NS_A4963;

    JsonSetter s{*device, filename};
}

void clearInput(){
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}