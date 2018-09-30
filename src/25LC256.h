#pragma once
#include <stdint.h>
#include <memory>
#include "src/SPI/mcp2210_hal.h"

class EEPROM : public SPIDevice {
private:
    struct Commands {
        static const uint8_t EEPROM_READ    =   0b00000011; //read memory
        static const uint8_t EEPROM_WRITE   =   0b00000010; //write to memory

        static const uint8_t EEPROM_WRDI    =   0b00000100; //write disable
        static const uint8_t EEPROM_WREN    =   0b00000110; //write enable

        static const uint8_t EEPROM_RDSR    =   0b00000101; // read status register
        static const uint8_t EEPROM_WRSR    =   0b00000001; // write status register
    };
    
    std::shared_ptr<spi::SPIBridge> mBridge;
public:
    EEPROM(std::shared_ptr<spi::SPIBridge> mBridge);

    void send16bitAddress(uint16_t address);
    std::shared_ptr<spi::SPIData> readStatus();
    void writeEnable();
    std::shared_ptr<spi::SPIData> readByte(uint16_t address);
    void writeByte(uint16_t address, uint8_t byte);
};