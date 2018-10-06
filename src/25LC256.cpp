#include <utility>

#include "25LC256.h"


void EEPROM::send16bitAddress(uint16_t address) {
    mBridge->transfer(spi::SPIData<2>(address));
    //mBridge->transfer(SPI8{second});
}

std::shared_ptr<spi::Data> EEPROM::readStatus() {
    using namespace spi::literals;
    mBridge->slaveSelect(shared_from_this());
    mBridge->transfer(0x05_spi8);
    auto data = mBridge->transfer(0x00_spi8);
    mBridge->slaveDeselect(shared_from_this());
    return data;
}


void EEPROM::writeEnable() {
    using namespace spi::literals;
    mBridge->slaveSelect(shared_from_this());
    mBridge->transfer(0x06_spi8);
    mBridge->slaveDeselect(shared_from_this());
}

std::shared_ptr<spi::Data> EEPROM::readByte(uint16_t address) {
    using namespace spi::literals;
    mBridge->slaveSelect(shared_from_this());
    mBridge->transfer(0x03_spi8);
    send16bitAddress(address);
    auto data = mBridge->transfer(0x00_spi8);
    mBridge->slaveDeselect(shared_from_this());
    return data;
}

void EEPROM::writeByte(uint16_t address, uint8_t byte) {
    using namespace spi::literals;
    writeEnable();
    mBridge->slaveSelect(shared_from_this());
    mBridge->transfer(0x02_spi8);
    send16bitAddress(address);
    mBridge->transfer(spi::SPIData<>{byte});
    mBridge->slaveDeselect(shared_from_this());
    while((*readStatus())[0] & (1 << 0)){__asm("nop");}
}

EEPROM::EEPROM(std::shared_ptr<spi::SPIBridge> mBridge) : mBridge(std::move(mBridge)) {}
