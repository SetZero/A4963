#include <utility>

#include "A4963.h"


void A4963::clearRegister(const A4963::RegisterCodes &reg) {
    mRegisterData[reg].data = 0;
}

spi::SPIData A4963::send16bitRegister(size_type address) {
    auto first = static_cast<uint8_t>(address >> 8);
    auto second = static_cast<uint8_t>(address);
    auto msb = mBridge->transfer({first});
    msb += mBridge->transfer({second});
    return msb;
}

A4963::A4963(std::shared_ptr<spi::SPIBridge> mBridge) : mBridge(std::move(mBridge)) {}

void A4963::writeRegister(const A4963::RegisterCodes &reg, size_type data) {
    clearRegister(reg);
    mRegisterData[reg].data |= createRegisterEntry(reg, RegisterPosition::RegisterAddress, RegisterMask::RegisterAddress);
    mRegisterData[reg].data |= createRegisterEntry(WriteBit::Write, RegisterPosition::WriteAddress, RegisterMask::WriteAddress);
    mRegisterData[reg].data |= createRegisterEntry(data, RegisterPosition::GeneralData, RegisterMask::GeneralData);
    mRegisterData[reg].dirty = true;
}

void A4963::reloadRegister(const A4963::RegisterCodes &reg) {
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

void A4963::setRecirculationMode(const A4963::RecirculationModeTypes &type) {
    A4963::size_type data = createRegisterEntry(type, RegisterPosition::RecirculationModeAddress, RegisterMask::RecirculationModeAddress);
    writeRegister(RegisterCodes::Config0, data);
}

void A4963::commit() {
    for(auto& data : mRegisterData) {
        if(data.second.dirty) {
            mBridge->slaveSelect(shared_from_this());
            if(getRegisterEntry(data.first, RegisterPosition::WriteAddress, RegisterMask::WriteAddress) == static_cast<uint16_t>(WriteBit::Read)) {
                mRegisterData[data.first].data = static_cast<uint16_t>(send16bitRegister(data.second.data));
            } else {
                send16bitRegister(data.second.data);
            }
            mBridge->slaveDeselect(shared_from_this());
            data.second.dirty = false;
        }
    }
}

uint16_t A4963::_dbg_reload_commit_and_get_register0() {
    reloadRegister(A4963::RegisterCodes::Config0);
    commit();
    return mRegisterData[A4963::RegisterCodes::Config0].data;
}
