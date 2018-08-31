#pragma once
#include <vector>
#include <map>
#include "GPIOBridge.h"

namespace spi {


    class SPIBridge {
    public:
        virtual ~SPIBridge() = default;
        virtual unsigned char transfer(unsigned char input) = 0;
        virtual std::vector<unsigned char> transfer(const std::vector<unsigned char>& input) = 0;
        virtual void slaveSelect(const gpio::GPIOPin& slave) = 0;
        virtual void slaveDeselect(const gpio::GPIOPin& slave) = 0;
    };
}