//
// Created by sebastian on 31.08.18.
//

#pragma once

#include "GPIOBridge.h"

class SPIDevice {
private:
    gpio::GPIOPin slavePin;
public:
    void selectPin(const gpio::GPIOPin& pin) const;
    const gpio::GPIOPin& getSlavePin() const;

};
