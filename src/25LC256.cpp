#include <utility>

#include "25LC256.h"


void EEPROM::send16bitAddress(uint16_t address) {
    auto first = static_cast<uint8_t>(address >> 8);
    auto second = static_cast<uint8_t>(address);
    mBridge->transfer({first});
    mBridge->transfer({second});
}

spi::SPIData EEPROM::readStatus() {
    using namespace spi::literals;
    mBridge->slaveSelect(shared_from_this());
    mBridge->transfer(0x05_spi);
    auto data = mBridge->transfer(0x00_spi);
    mBridge->slaveDeselect(shared_from_this());
    return data;
}


void EEPROM::writeEnable() {
    using namespace spi::literals;
    mBridge->slaveSelect(shared_from_this());
    mBridge->transfer(0x06_spi);
    mBridge->slaveDeselect(shared_from_this());
}

spi::SPIData EEPROM::readByte(uint16_t address) {
    using namespace spi::literals;
    mBridge->slaveSelect(shared_from_this());
    mBridge->transfer(0x03_spi);
    send16bitAddress(address);
    auto data = mBridge->transfer(0x00_spi);
    mBridge->slaveDeselect(shared_from_this());
    return data;
}

void EEPROM::writeByte(uint16_t address, uint8_t byte) {
    using namespace spi::literals;
    writeEnable();
    mBridge->slaveSelect(shared_from_this());
    mBridge->transfer(0x02_spi);
    send16bitAddress(address);
    mBridge->transfer({byte});
    mBridge->slaveDeselect(shared_from_this());
    while(readStatus().getData()[0] & (1 << 0)){;}
}

EEPROM::EEPROM(std::shared_ptr<spi::SPIBridge> mBridge) : mBridge(std::move(mBridge)) {}
