#include <utility>

#include "A4963.h"

void A4963::send16bitRegister(size_type address) {
    auto first = static_cast<uint8_t>(address >> 8);
    auto second = static_cast<uint8_t>(address);
    mBridge->transfer({first});
    mBridge->transfer({second});
}

A4963::A4963(std::shared_ptr<spi::SPIBridge> mBridge) : mBridge(std::move(mBridge)) {}

void A4963::writeRegister(const A4963::RegisterCodes &reg, size_type data) {
    mRegisterData[reg] |= createRegisterEntry(reg, RegisterPosition::RegisterAddress, RegisterMask::RegisterAddress);
    mRegisterData[reg] |= createRegisterEntry(WriteBit::Write, RegisterPosition::WriteAddress, RegisterMask::WriteAddress);
    mRegisterData[reg] |= createRegisterEntry(data, RegisterPosition::GeneralData, RegisterMask::GeneralData);
}

spi::SPIData A4963::readRegister(const A4963::RegisterCodes &reg) {
    return spi::SPIData({});
}

template<typename T>
A4963::size_type A4963::createRegisterEntry(T data, const A4963::RegisterPosition &position, const A4963::RegisterMask &mask) {
    A4963::size_type registerData = 0;
    registerData = (static_cast<size_type>(data) << static_cast<size_type>(position)) & static_cast<size_type>(mask);
    return registerData;
}

void A4963::setRecirculationMode(const A4963::RecirculationModeTypes &type) {
    A4963::size_type data = createRegisterEntry(type, RegisterPosition::RecirculationModeAddress, RegisterMask::RecirculationModeAddress);
    writeRegister(RegisterCodes::Config0, data);
}

void A4963::commit() {
    for(auto data : mRegisterData) {
        mBridge->slaveSelect(shared_from_this());
        send16bitRegister(data.second);
        mBridge->slaveDeselect(shared_from_this());
    }
}
