#pragma once
#include <utility>
#include <vector>
#include <map>
#include <memory>
#include "GPIOBridge.h"
#include "SPIDevice.h"

namespace spi {
    class SPIData {
    public:
        SPIData(std::vector<unsigned char> data) : mData(std::move(data)) {}

        SPIData(std::initializer_list<int> data) {
            mData.insert(mData.end(), data.begin(), data.end());
        }

        explicit SPIData(int data) {
            mData.push_back(static_cast<unsigned char &&>(data));
        }

        const std::vector<unsigned char> &getData() const {
            return mData;
        }

    private:
        std::vector<unsigned char> mData;
    };
    inline namespace literals {
        SPIData operator ""_spi(unsigned long long element);
    }

    class SPIBridge {
    public:
        virtual ~SPIBridge() = default;
        virtual SPIData transfer(const SPIData& spiData) const = 0;
        virtual void slaveRegister(std::shared_ptr<SPIDevice> device, const gpio::GPIOPin& pin) = 0;
        virtual void slaveSelect(std::shared_ptr<SPIDevice> slave) = 0;
        virtual void slaveDeselect(std::shared_ptr<SPIDevice> slave) = 0;
    };
}