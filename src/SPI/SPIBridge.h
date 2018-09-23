#pragma once
#include <utility>
#include <vector>
#include <map>
#include "GPIOBridge.h"
#include "SPIDevice.h"
#include "SPIData.h"
#include <memory>

using SPI8 = typename spi::SPIData<1,spi::EndianMode::little_endian>;
using SPI16 = typename spi::SPIData<2,spi::EndianMode::little_endian>;

namespace spi {

    /* this is obsolete
    class SPIData {
    public:
        SPIData(std::vector<unsigned char> data) : mData(std::move(data)) {}

        SPIData(std::initializer_list<int> data) {
            mData.insert(mData.end(), data.begin(), data.end());
        }

        SPIData& operator+=(const SPIData& rhs){
            mData.insert(std::end(mData),std::begin(rhs.mData),std::end(rhs.mData));
            return *this;
        }

        explicit SPIData(int data) {
            mData.push_back(static_cast<unsigned char &&>(data));
        }

        const std::vector<unsigned char> &getData() const {
            return mData;
        }

        //todo: is this really necessary?
        explicit operator uint8_t(){
            return mData.empty() ? static_cast<uint8_t >(mData[0]): static_cast<uint8_t >(0);
        }

        //todo: is this really how this should work????? SPIData tupel? template? (tripel quadripel etc)
        explicit operator uint16_t(){
            auto size = mData.size();
            switch (size){
                case 0 : return 0;
                case 1 : return mData[0];
                default: return static_cast<uint16_t>(mData[0]<<8) | static_cast<uint16_t>(mData[1]);
            }
        }
    private:
        std::vector<unsigned char> mData;
    };*/


    class SPIBridge {
    public:
        virtual ~SPIBridge() = default;
        virtual SPI8 transfer(const SPI8& spiData) const = 0;
        virtual std::vector<SPI8> transfer(const std::initializer_list<SPI8>& spiData) const = 0;
        virtual void slaveRegister(std::shared_ptr<SPIDevice>, const gpio::GPIOPin& pin) = 0;
        virtual void slaveSelect(std::shared_ptr<SPIDevice> slave) = 0;
        virtual void slaveDeselect(std::shared_ptr<SPIDevice> slave) = 0;
    };
}