#pragma once
#include <utility>
#include <vector>
#include <map>
#include "GPIOBridge.h"
#include "SPIDevice.h"
#include "SPIData.h"
//#include "spi8.h"
//#include "spi16.h"
#include <memory>


namespace spi {


    class SPIBridge {
    public:
        virtual ~SPIBridge() = default;
        virtual std::shared_ptr<spi::Data> transfer(const spi::Data& spiData)  = 0;
        virtual std::vector<std::shared_ptr<spi::Data>> transfer(const std::initializer_list<std::shared_ptr<spi::Data>>& spiData) = 0;
        virtual void slaveRegister(std::shared_ptr<SPIDevice>, const gpio::GPIOPin& pin) = 0;
        virtual void slaveSelect(std::shared_ptr<SPIDevice> slave) = 0;
        virtual void slaveDeselect(std::shared_ptr<SPIDevice> slave) = 0;
    };
}