//
// Created by sebastian on 31.08.18.
//

#include "SPIDevice.h"

const gpio::GPIOPin &SPIDevice::getSlavePin() const{
    return slavePin;
}

void SPIDevice::selectPin(const gpio::GPIOPin &pin) const{
    slavePin = pin;
}
