#include <utility>
#include <bitset>

#include "A4963.h"


void A4963::clearRegister(const A4963::RegisterCodes &reg) {
    mRegisterData[reg].data = 0;
}


void A4963::clearRegister(const A4963::RegisterCodes &reg, const A4963::RegisterMask &mask) {
    mRegisterData[reg].data &= ~(static_cast<A4963::size_type>(mask));
}

std::shared_ptr<spi::SPIData> A4963::send16bitRegister(size_type address) {
    //auto first = static_cast<uint8_t>(address >> 8);
    //auto second = static_cast<uint8_t>(address);
    //auto msb = SPI8{first};
    std::shared_ptr<spi::SPIData> d = mBridge->transfer(spi::spi16(address));
    return d;
}

A4963::A4963(std::shared_ptr<spi::SPIBridge> mBridge) : mBridge(std::move(mBridge)) {
    //reload all register
    markRegisterForReload(A4963::RegisterCodes::Config0);
    markRegisterForReload(A4963::RegisterCodes::Config1);
    markRegisterForReload(A4963::RegisterCodes::Config2);
    markRegisterForReload(A4963::RegisterCodes::Config3);
    markRegisterForReload(A4963::RegisterCodes::Config4);
    markRegisterForReload(A4963::RegisterCodes::Config5);
    markRegisterForReload(A4963::RegisterCodes::Mask);
    markRegisterForReload(A4963::RegisterCodes::Run);
}

void A4963::writeRegisterEntry(const A4963::RegisterCodes &reg, const A4963::RegisterMask &mask, size_type data) {
    clearRegister(reg, RegisterMask::RegisterAndWriteAddress);
    clearRegister(reg, mask);
    
    mRegisterData[reg].data |= createRegisterEntry(reg, RegisterPosition::RegisterAddress, RegisterMask::RegisterAddress);
    mRegisterData[reg].data |= createRegisterEntry(WriteBit::Write, RegisterPosition::WriteAddress, RegisterMask::WriteAddress);
    mRegisterData[reg].data |= createRegisterEntry(data, RegisterPosition::GeneralData, RegisterMask::GeneralData);
    mRegisterData[reg].dirty = true;
}

void A4963::markRegisterForReload(const A4963::RegisterCodes &reg) {
    clearRegister(reg);
    mRegisterData[reg].data |= createRegisterEntry(reg, RegisterPosition::RegisterAddress, RegisterMask::RegisterAddress);
    mRegisterData[reg].data |= createRegisterEntry(WriteBit::Read, RegisterPosition::WriteAddress, RegisterMask::WriteAddress);
    mRegisterData[reg].dirty = true;
}

template<typename T>
A4963::size_type A4963::createRegisterEntry(T data, const A4963::RegisterPosition &position, const A4963::RegisterMask &mask) {
    A4963::size_type registerData = 0;
    registerData = (static_cast<size_type>(data) << static_cast<size_type>(position)) & static_cast<size_type>(mask);
    return registerData;
}


A4963::size_type A4963::getRegisterEntry(const A4963::RegisterCodes& registerEntry, const A4963::RegisterPosition &position,
                                         const A4963::RegisterMask &mask) {
    A4963::size_type registerData = 0;
    registerData = (mRegisterData[registerEntry].data & static_cast<size_type>(mask)) >> static_cast<size_type>(position);
    return registerData;
}

void A4963::commit() {
    for(auto& data : mRegisterData) {
        commit(data.first);
    }
}

void A4963::commit(const A4963::RegisterCodes& registerCodes) {
    if(mRegisterData[registerCodes].dirty) {
        mBridge->slaveSelect(shared_from_this());
        if(getRegisterEntry(registerCodes, RegisterPosition::WriteAddress, RegisterMask::WriteAddress) == static_cast<A4963::size_type>(WriteBit::Read)) {
            mRegisterData[registerCodes].data = createRegisterEntry(registerCodes, RegisterPosition::RegisterAddress, RegisterMask::RegisterAddress) |
                    (static_cast<A4963::size_type>(*send16bitRegister(mRegisterData[registerCodes].data)) &
                    static_cast<A4963::size_type>(RegisterMask::GeneralData));
        } else {
            send16bitRegister(mRegisterData[registerCodes].data);
        }
        mBridge->slaveDeselect(shared_from_this());
        mRegisterData[registerCodes].dirty = false;
    }
}

void A4963::show_register() {
    for(auto registerData : mRegisterData) {
        std::bitset<16> set(readRegister(registerData.first));
        std::cout << static_cast<A4963::size_type>(registerData.first) << ": " <<  set << std::endl;
    }
}

A4963::size_type A4963::readRegister(const A4963::RegisterCodes &registerCodes) {
    if(mRegisterData[registerCodes].dirty) {
        commit(registerCodes);
    }
    return mRegisterData[registerCodes].data;
}

void A4963::setRecirculationMode(const A4963::RecirculationModeTypes &type) {
    A4963::size_type data = createRegisterEntry(type, RegisterPosition::RecirculationModeAddress, RegisterMask::RecirculationModeAddress);
    writeRegisterEntry(RegisterCodes::Config0, RegisterMask::RecirculationModeAddress, data);
}
