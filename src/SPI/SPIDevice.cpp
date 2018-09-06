//
// Created by sebastian on 31.08.18.
//

#include "SPIDevice.h"

bool SPIDevice::operator<(const SPIDevice &rhs) const {
    return someValue < rhs.someValue;
}

bool SPIDevice::operator>(const SPIDevice &rhs) const {
    return rhs < *this;
}

bool SPIDevice::operator<=(const SPIDevice &rhs) const {
    return !(rhs < *this);
}

bool SPIDevice::operator>=(const SPIDevice &rhs) const {
    return !(*this < rhs);
}
