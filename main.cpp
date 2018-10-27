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
    if(argc < 2 ) {
        return consoleInterface("mcp");
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
    if(std::string(spiDevice) == "atmega"){
        std::cout << "Starting Atmega32u4..." << std::endl;
        usb::LibUSBDeviceList deviceList;
        std::cout << "Found " << deviceList.size() << " devices" << std::endl;
        if(auto atmega = deviceList.findDevice(spi::ATmega32u4SPI::vendorID, spi::ATmega32u4SPI::deviceID)) {
            std::cout << "INDIGO!" << std::endl;
            auto ptr = std::make_shared<spi::ATmega32u4SPI>(*atmega);
            device = std::make_shared<NS_A4963::A4963>(ptr);
        }
        else {
            std::cerr << " atmega not found " << std::endl;
        }
    } else {
        using namespace CustomDataTypes::literals;
        auto ptr = std::make_shared<MCP2210>();
        device = std::make_shared<NS_A4963::A4963>(ptr);
    }
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
    json j;
    std::ifstream ifs = std::ifstream("config.json");
    ifs >> j;
    ifs.close();
    auto vec = j["config"];
    for (auto &it : vec) {
        for (auto it1 = it.begin(); it1 != it.end(); ++it1) {
            if (it1.key() != "Duty Cycle Control" ||
                (it1.key() == "Duty Cycle Control" && it1.value() != "0")) {
                auto mask = RegisterStrings::get(it1.key());
                setRuntime(*device, mask, it1.value());
            }
            else
                device->turnOffDutyCycle();
        }
    }
    vec = j["mask"];
    for (auto &it : vec) {
        for (auto it1 = it.begin(); it1 != it.end(); ++it1) {
            device->configDiagnostic(static_cast<NS_A4963::RegisterMask>(NS_A4963::A4963MasksMap.at(it1.key())),it1.value());
            std::cout << "set: " << it1.key() << " to: " << (it1.value() ? " On " : " Off ") << std::endl;
        }
    }
}

void clearInput(){
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

/* Obsolete
 *
 * enum Chips {
    ATMEGA, MCP
};
static constexpr Chips used_chip = Chips::ATMEGA;

bool reconnect( std::shared_ptr<MCP2210>& ptr);
int userInput();
void clearInput();

int atmega_main() {
    using namespace spi::literals;
    using namespace std::chrono_literals;
    using namespace NS_A4963;
    std::cout << "Starting Atmega32u4..." << std::endl;
    usb::LibUSBDeviceList deviceList;
    std::cout << "Found " << deviceList.size() << " devices" << std::endl;
    if(auto atmega = deviceList.findDevice(spi::ATmega32u4SPI::vendorID, spi::ATmega32u4SPI::deviceID)) {
        std::cout << "INDIGO!" << std::endl;
        auto spi = std::make_shared<spi::ATmega32u4SPI>(*atmega);
        auto device = std::make_shared<NS_A4963::A4963>(spi);
        spi->slaveRegister(device, spi::ATmega32u4SPI::pin0);
        JsonSetter s{*device, "data.json"};

        using namespace utils::printable;
        auto bt = device->getRegEntry<NS_A4963::A4963RegisterNames::BlankTime>();
        std::cout << "Blank Time: " << bt << std::endl;
        auto cstv = device->getRegEntry<NS_A4963::A4963RegisterNames::CurrentSenseThresholdVoltage>();
        std::cout << "Current Sense Threshold Voltage: " << cstv << std::endl;
        auto dt = device->getRegEntry<NS_A4963::A4963RegisterNames::DeadTime>();
        std::cout << "Current Sense Threshold Voltage: " << dt << std::endl;
        auto pdcts = device->getRegEntry<NS_A4963::A4963RegisterNames::PWMDutyCycleHoldTorque>();
        std::cout << "PWM Duty Cycle Torque Startup: " << pdcts << std::endl;

        device->show_register();
    }
    return 0;
}

int mcp_main(){
    using namespace CustomDataTypes::literals;
    static constexpr auto min = 6.25_perc;
    static constexpr auto max = 100.0_perc;
    auto x = (3+1)*min;
    auto x1 = static_cast<ssize_t>((x.getPercent()/min)-1.0_perc);
    std:: cout << x.getPercent() << " " << x1;
    std::shared_ptr<MCP2210> ptr;
    ptr = std::make_shared<MCP2210>();
    using namespace nlohmann;
    using namespace NS_A4963;
    auto device = std::make_shared<NS_A4963::A4963>(ptr);
        ptr->exceptionHandling(-100);

    enum class TO : uint8_t {
        f = 1,
        ff = 2
    };

    using namespace spi::literals;

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
int main2() {
    if constexpr (used_chip == Chips::ATMEGA) {
        return atmega_main();
    } else if(used_chip == Chips::MCP) {
        return mcp_main();
    }
}


bool reconnect( std::shared_ptr<MCP2210>& ptr){
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
            std::cout << " please use a valid number! " << std::endl;
        }
    } while(z == -1);
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return z;
}

void clearInput(){
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
 */