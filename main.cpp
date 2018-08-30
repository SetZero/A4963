#include <iostream>
#include <array>
#include <string>
#include <memory>
#include <vector>
#include "src/SPIBridge.h"
#include "mcp2210_api.h"
#include "src/mcp2210_hal.h"
#include "src/25LC256.h"
#include "ATmega32U4SPI.h"

int main(int argc, char **argv) {
    int ictr;

    std::cout << "\nMCP2210 Evaluation Board Tests" << std::endl;
    std::cout << "\nParameters: " << argc << std::endl;
    for (ictr = 0; ictr < argc; ictr++)
	{
		std::cout << "\nParameter(" << ictr << ") -> " << argv[ictr] << std::endl;
	}

    if(argc > 1) {
        std::string str = argv[1];
        std::cout << "Trying to open: " << argv[1] << std::endl;

        std::unique_ptr<spi::SPIBridge> bridge = std::make_unique<MCP2210>(str);
        EEPROM eeprom{bridge};
        std::cout << "Status: " << std::hex << eeprom.readStatus() << std::endl;
        //(*bridge).setGPIODirection(spi::gpioDirection::out, spi::pin0);
        //(*bridge).writeGPIO(spi::gpioState::on, spi::pin0);

        //std::vector<unsigned char> txVector = {0x01, 0x02, 0x03};
        //std::vector<unsigned char> rxVector = (*bridge).transfer(txVector);
    } else {
        ATmega32u4SPI spi;
    }
}



