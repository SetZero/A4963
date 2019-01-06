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
    constexpr void selectPin(const gpio::GPIOPin& pin) noexcept {
        slavePin = pin;
    }
    [[nodiscard]] constexpr  gpio::GPIOPin getSlavePin() const noexcept {
        return slavePin;
    }

};
