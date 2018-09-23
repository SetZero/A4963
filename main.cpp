#include <iostream>
#include "src/SPI/mcp2210_hal.h"


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void reconnect( std::unique_ptr<MCP2210>& ptr);

int main(int argc, char **argv) {/*
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
        auto device = std::make_shared<A4963>(spi);
        spi->slaveRegister(device, spi::ATmega32u4SPI::pin0);

        //device->setRecirculationMode(A4963::RecirculationModeTypes::High);
        //device->commit();

        auto actldedtime = device->setDeadTime(1us);
        std::cout << "Actual Dead Time: " << std::chrono::nanoseconds(*actldedtime).count() << "ns" <<  std::endl;
        auto actlblktime = device->setBlankTime(800ns);
        std::cout << "Actual Blank Time: " << std::chrono::nanoseconds(*actlblktime).count() << "ns" <<  std::endl;
        device->commit();

        device->show_register();

        std::string str2 = "Other Text!";
        for(std::string::size_type i = 0; i < str2.size(); i++) {
            device->writeByte(static_cast<uint16_t>(i), static_cast<uint8_t>(str2[i]));
        }
        for(std::string::size_type i = 0; i < str2.size(); i++) {
            auto back = device->readByte(static_cast<uint16_t>(i));
            std::cout << "Data: " << back.getData()[0] << std::endl;
        }*/

    std::cout << "this is sparta" << std::endl;
    std::unique_ptr<MCP2210> ptr;
    ptr = std::make_unique<MCP2210>();

        while(true) {
            auto eingaben = { "spi test","eingabe test","exit"};
            std::cout << "mögliche eingaben: " << std::endl;
            size_t i = 0;
            for(auto val : eingaben){
                std::cout << i << ".: " << val << std::endl;
                i++;
            }
            uint32_t z = 0;
            std::cin >> z;
            switch(z) {
                case 0: {
                    for (int j = 0; j < 10; j++) {
                        using namespace spi::literals;
                        try {
                            if(*ptr)
                                ptr->transfer(42_spi8);
                            else{
                                std::cout << " no connection ";
                                reconnect(ptr);
                            }
                        } catch(MCPIOException& e){
                            std::cout << e.what();
                            reconnect(ptr);
                        }
                    }
                    break;
                }
                case 1: {
                    std::cout << "give your data: ";
                    std::string input{};
                    std::cin >> input;
                    for(auto ch: input){
                        try{
                            if(*ptr)
                                ptr->transfer(spi::SPIData(ch));
                            else{
                                std::cout << " no connection ";
                                reconnect(ptr);
                            }
                        } catch(MCPIOException& e){
                            std::cout << e.what();
                            reconnect(ptr);
                        }
                    }
                    break;
                }
                case 2 : return 0;
                default: break;
            }
        }
    }

void reconnect( std::unique_ptr<MCP2210>& ptr){
    std::cout << " type sth to attempt a reconnect " << std::endl;
    int z;
    std::cin >> z;
    ptr->connect();
};

#pragma clang diagnostic pop