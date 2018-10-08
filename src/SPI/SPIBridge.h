#pragma once
#include <utility>
#include <vector>
#include <map>
#include "GPIOBridge.h"
#include "SPIDevice.h"
#include "SPIData.h"
#include <memory>


namespace spi {
    class SPIBridge {
    public:
        virtual ~SPIBridge() = default;
        virtual std::unique_ptr<spi::Data> transfer(const spi::Data& spiData)  = 0;
        virtual std::vector<std::unique_ptr<spi::Data>> transfer(const std::initializer_list<std::unique_ptr<spi::Data>>& spiData) = 0;
        virtual void slaveRegister(const std::shared_ptr<SPIDevice>&, const gpio::GPIOPin& pin) = 0;
        virtual void slaveSelect(const std::shared_ptr<SPIDevice>& slave) = 0;
        virtual void slaveDeselect(const std::shared_ptr<SPIDevice>& slave) = 0;
    };
}