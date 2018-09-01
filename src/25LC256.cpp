#include "25LC256.h"

EEPROM::EEPROM(std::unique_ptr<spi::SPIBridge>& bridge) 
 : mBridge{std::move(bridge)} {
 //   (*mBridge).setGPIODirection(spi::gpioDirection::out, spi::pin0);
 //   (*mBridge).writeGPIO(spi::gpioState::on, spi::pin0);
 }

void EEPROM::send16BitAddress(uint16_t address) {
//    (*mBridge).transfer((uint8_t) (address >> 8));
//    (*mBridge).transfer((uint8_t) address);
}

uint8_t EEPROM::readStatus() {
/*    (*mBridge).slaveSelect();
    //std::cout << "RDSR: " << std::hex << Commands::EEPROM_RDSR << std::endl;
    unsigned char output = (*mBridge).transfer(0b00000101);
    (*mBridge).transfer(0);
    (*mBridge).slaveDeselect();*/
    return 0;
}