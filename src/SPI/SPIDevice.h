//
// Created by sebastian on 31.08.18.
//

#pragma once

#include "GPIOBridge.h"
#include <memory>

class SPIDevice : public std::enable_shared_from_this<SPIDevice> {
private:
    gpio::GPIOPin slavePin;
public:
    void selectPin(const gpio::GPIOPin& pin);
    const gpio::GPIOPin& getSlavePin() const;

};
