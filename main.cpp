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

int consoleInterface(const std::string& spiDevice,const std::string& config);
void flashJSON(const std::string& spiDevice, const std::string& filename, bool enable_debug = false);
int simpleInput(int min, int max);
int serverInterface(const char* spiDevice);
void parseArguments(int argc, char** argv);

static inline constexpr int nrOfOptions = 5;

void clearInput();
void flashFileFromDevice(std::shared_ptr<NS_A4963::A4963> &device, const std::string &filename,
                         bool enable_debug = false);
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
    const std::string path = argv[0];
    auto const pos = path.find_last_of('/');
    const auto leaf = path.substr(pos+1);

    cxxopts::Options options(leaf, "An A4963 USB configuration program");
    options.add_options()
            ("h,help", "Show help")
            ("d,debug", "Enable debugging mode")
            ("j,json", "Read in JSON file", cxxopts::value<std::string>())
            ("g,generate", "Generate JSON file, will take -j location if available")
            ("f,force", "Force overwrite of current JSON file, only possible in combination with -g")
            ("c,client", "Use interactive client interface")
            ("i,interface", "Select USB to SPI Bridge (atmega / mcp)", cxxopts::value<std::string>())
            ("s,settings-check", "Only check json file values, do not flash to device");

    auto result = options.parse(argc, argv);
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
        if(result.count("json") > 0)
            consoleInterface(interface,result["json"].as<std::string>());
        else{
            consoleInterface(interface,"config.json");
        }
        return;
    } else if(result.count("client") > 0 && result.count("interface") <= 0) {
        std::cout << "Missing Parameter: --interface!" << std::endl;
    }

    if(result.count("json") > 0 && (result.count("interface") > 0 || result.count("settings-check") > 0)) {
        auto json_filename  = result["json"].as<std::string>();
        std::string interface_name;
        if(result.count("interface") > 0) {
            interface_name = result["interface"].as<std::string>();
            if(interface_name.empty()) {
                std::cout << "Interface may not be empty" << std::endl;
                return;
            }
        }
        if(result.count("debug") > 0) {
            flashJSON(interface_name, json_filename, true);
        } else {
            flashJSON(interface_name, json_filename);
        }
    } else {
        if(result.count("json") > 0 && (result.count("interface") <= 0 && result.count("settings-check") <= 0)) {
            std::cout << "Missing parameter --interface [atmega|mcp] or --settings-check" << std::endl;
        } else if(result.count("json") <= 0 && result.count("interface") > 0) {
            std::cout << "Missing parameter --json [filename]" << std::endl;
        }
    }
    if(result.arguments().empty() || result.count("help") > 0) {
        std::cout << options.help() << std::endl;
    }
}

int simpleInput(int min, int max){
    std::cout << "enter a valid number from " << std::to_string(min) << " to " << std::to_string(max) << std::endl;
    int z = -1;
    do {
        try {
            std::string str = std::to_string(0);
            std::cin >> str;
            z = std::stoi(str);
        } catch(std::exception& e){
            std::cout << " please enter a valid number! " << std::endl;
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
            std::cerr << "No device connected!" << std::endl;
            return;
        }
    } else if(spiDevice == "mcp") {
        spi = std::make_shared<MCP2210<>>();
        pin = MCP2210<>::pin0;
    } else if(spiDevice.empty()) {
        spi = nullptr;
    }

    device = std::make_shared<NS_A4963::A4963>(spi, enable_debug);
    if(spi)
        spi->slaveRegister(device, pin);

    flashFileFromDevice(device, filename, enable_debug);
}

int consoleInterface(const std::string& spiDevice, const std::string& config){
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
            std::cerr << "No device connected!" << std::endl;
            return 0;
        }
    } else {
        std::shared_ptr<MCP2210<>> dev = std::make_shared<MCP2210<>>();
        while(!*dev){
            std::cout << " device not connected, try again?: y/n" << std::endl;
            std::string str;
            std::getline(std::cin,str);
            if(str == "y")
                dev->connect();
            else break;
        }
        spi = dev;
        pin = MCP2210<>::pin0;
    }
    device = std::make_shared<NS_A4963::A4963>(spi);
    spi->slaveRegister(device, pin);

    showOptions<nrOfOptions>(optMain);
    while(true) {
        int choice = simpleInput(1, nrOfOptions);
        switch (choice) {
            case 1: {
                flashFileFromDevice(device, config);
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

inline void showRegisterVal(std::shared_ptr<NS_A4963::A4963>& device){
    std::string str;
    while(true) {
        std::cout << "enter the name of the Register mask you want to read" << std::endl;
        std::getline(std::cin,str);
        if (str == "exit") break;
        try {
            auto mask = NS_A4963::RegisterStrings::get(str);
            std::cout << device->getRuntime(mask) << std::endl;
            break;
        } catch (std::exception &e) {
            std::cout << "enter a valid name or exit" << std::endl;
        }
    }
}

bool fileExists(const std::string& path){
    std::ifstream ifs{path};
    return ifs.good();
}

bool generateDefault(bool force, const std::string& filename){
    bool exists = fileExists(filename);
    std::ofstream of{filename};
    bool return_value = false;
    if(of.good() && (!exists|| force)) {
        of << std::setw(4) << NS_A4963::defaultValues << std::flush;
        return_value = true;
        std::cout << "New config successfully generated (" << filename << ")" << std::endl;
    } else {
        std::cout << "File already exists! Use -f to force overwrite" << std::endl;
    }
    of.close();
    return return_value;
}

void setRegisterVal(std::shared_ptr<NS_A4963::A4963>& device){
    std::cout << "Enter the name of the register you want to set, or \"exit\" to cancel" << std::endl;
    std::string str;
    NS_A4963::A4963RegisterNames mask;
    while(true) {
        std::getline(std::cin,str);
        if (str == "exit") break;
        try {
            mask = NS_A4963::RegisterStrings::get(str);
            break;
        } catch (std::exception &e) {
            std::cout << "Enter a valid name or exit" << std::endl;
        }
    }
    std::cout << "Enter the value you want to set" << std::endl;
    while(true) {
        std::getline(std::cin,str);
        if (str == "exit") break;
        try {
            NS_A4963::setRuntime(*device,mask,str);
            break;
        } catch (std::exception &e) {
            std::cout << "Enter a valid value or exit" << std::endl;
        }
    }
}

void flashFileFromDevice(std::shared_ptr<NS_A4963::A4963> &device, const std::string &filename, bool enable_debug){
    using namespace nlohmann;
    using namespace NS_A4963;

    JsonSetter s{*device, filename};
    if(enable_debug) {
        device->show_register();
    }
}

void clearInput(){
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}