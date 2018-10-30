#include <utility>
#include <bitset>

#include "A4963.h"

namespace NS_A4963 {
    void A4963::clearRegister(const detail::RegisterCodes &reg) {
        mRegisterData[reg].data = 0;
    }


    void A4963::clearRegister(const detail::RegisterCodes &reg, const detail::RegisterMask &mask) {
        mRegisterData[reg].data &= ~(static_cast<A4963::size_type>(mask));
    }

    std::unique_ptr<spi::Data> A4963::send16bitRegister(size_type address) {
         return mBridge->transfer(spi::SPIData<sizeof(size_type),spi::big_endian>{address});
    }

    A4963::A4963(std::shared_ptr<spi::SPIBridge> mBridge) : mBridge(std::move(mBridge)) {
        //reload all register
        markRegisterForReload(detail::RegisterCodes::Config0);
        markRegisterForReload(detail::RegisterCodes::Config1);
        markRegisterForReload(detail::RegisterCodes::Config2);
        markRegisterForReload(detail::RegisterCodes::Config3);
        markRegisterForReload(detail::RegisterCodes::Config4);
        markRegisterForReload(detail::RegisterCodes::Config5);
        markRegisterForReload(detail::RegisterCodes::Mask);
        markRegisterForReload(detail::RegisterCodes::Run);
    }

    void A4963::writeRegisterEntry(const detail::RegisterCodes &reg, const detail::RegisterMask &mask, size_type data) {
        clearRegister(reg, RegisterMask::RegisterAndWriteAddress);
        clearRegister(reg, mask);

        mRegisterData[reg].data |= createRegisterEntry(static_cast<size_type>(reg), detail::RegisterMask::RegisterAddress);
        mRegisterData[reg].data |= createRegisterEntry(static_cast<size_type>(WriteBit::Write), detail::RegisterMask::WriteAddress);
        mRegisterData[reg].data |= createRegisterEntry(data, detail::RegisterMask::GeneralData);
        mRegisterData[reg].dirty = true;
    }

    void A4963::markRegisterForReload(const detail::RegisterCodes &reg) {
        clearRegister(reg);
        mRegisterData[reg].data |= createRegisterEntry(static_cast<size_type>(reg), detail::RegisterMask::RegisterAddress);
        mRegisterData[reg].data |= createRegisterEntry(static_cast<size_type>(WriteBit::Read), detail::RegisterMask::WriteAddress);
        mRegisterData[reg].dirty = true;
    }

    A4963::size_type A4963::createRegisterEntry(size_type data, const detail::RegisterMask &mask) {
        A4963::size_type registerData = 0;
        registerData = (data << utils::getFirstSetBitPos(static_cast<size_type>(mask))) &
                       static_cast<size_type>(mask);
        return registerData;
    }


    A4963::size_type
    A4963::getRegisterEntry(const detail::RegisterCodes &registerEntry, const detail::RegisterMask &mask) {
        A4963::size_type registerData = 0;
        registerData = (mRegisterData[registerEntry].data & static_cast<size_type>(mask))
                >> utils::getFirstSetBitPos(static_cast<size_type>(mask));
        return registerData;
    }

    void A4963::commit() {
        for (auto &data : mRegisterData) {
            commit(data.first);
        }
    }

    void A4963::commit(const detail::RegisterCodes &registerCodes) {
        if (mRegisterData[registerCodes].dirty) {
            mBridge->slaveSelect(shared_from_this());
            if (getRegisterEntry(registerCodes, detail::RegisterMask::WriteAddress) ==
                static_cast<A4963::size_type>(WriteBit::Read))
            {
                mRegisterData[registerCodes].data = createRegisterEntry(static_cast<size_type>(registerCodes), detail::RegisterMask::RegisterAddress) |
                                                    ( static_cast<A4963::size_type>(*send16bitRegister(mRegisterData[registerCodes].data)) &
                                                     static_cast<A4963::size_type>(RegisterMask::GeneralData));
            } else {
                send16bitRegister(mRegisterData[registerCodes].data);
                mRegisterData[registerCodes].data &= ~(1 << utils::getFirstSetBitPos(static_cast<size_type>(detail::RegisterMask::WriteAddress)));
            }
            mBridge->slaveDeselect(shared_from_this());
            mRegisterData[registerCodes].dirty = false;
        }
    }



    void A4963::show_register() {
        for (auto registerData : mRegisterData) {
            std::bitset<16> bitset{readRegister(registerData.first)};
            std::cout << static_cast<A4963::size_type>(registerData.first) << ": " << bitset << std::endl;
        }
    }

    A4963::size_type A4963::readRegister(const detail::RegisterCodes &registerCodes, bool forceNoReload) {
        if (!forceNoReload && mRegisterData[registerCodes].dirty) {
            commit(registerCodes);
        }
        return mRegisterData[registerCodes].data;
    }

    void
    A4963::setRegisterEntry(size_type data, const detail::RegisterMask &mask, const detail::RegisterCodes &registerEntry) {
        auto val = createRegisterEntry(data,mask);
        writeRegisterEntry(registerEntry,mask,val);
    }

    A4963::size_type A4963::extractRegisterValue(size_t registerValue, RegisterMask registerMask) {
        return static_cast<size_type>(registerValue & static_cast<size_type>(registerMask)) >> utils::getFirstSetBitPos(static_cast<size_type>(registerMask));
    }

    void A4963::configDiagnostic(const Masks& mask,bool activate) {
        auto val  = createRegisterEntry(static_cast<size_type>(activate),static_cast<RegisterMask>(mask));
        writeRegisterEntry(RegisterCodes::Mask,static_cast<RegisterMask>(mask),val);
    }

    std::vector<Diagnostic> A4963::readDiagnostic() {
        std::bitset<16> tmp{readRegister(RegisterCodes::Diagnostic)};
        std::vector<Diagnostic> ret{};
        if(tmp.to_ulong() != 0) {
            for(size_type i = 0; i < 16;i++){
                if(tmp.test(i))
                    ret.push_back(static_cast<Diagnostic>(1 << i));
            }
        }
        return ret;
    }
}