//
// Created by sebastian on 03.09.18.
//
#include "USBUtils.h"

namespace usb {
    DeviceID::DeviceID(uint16_t deviceID) noexcept : deviceID(deviceID) {}

    uint16_t DeviceID::getDeviceID() const {
        return deviceID;
    }

    bool DeviceID::operator==(const DeviceID &rhs) const {
        return deviceID == rhs.deviceID;
    }

    bool DeviceID::operator!=(const DeviceID &rhs) const {
        return !(rhs == *this);
    }

    VendorID::VendorID(uint16_t vendorID) noexcept : vendorID(vendorID) {}

    uint16_t VendorID::getVendorID() const {
        return vendorID;
    }

    bool VendorID::operator==(const VendorID &rhs) const {
        return vendorID == rhs.vendorID;
    }

    bool VendorID::operator!=(const VendorID &rhs) const {
        return !(rhs == *this);
    }

    inline namespace literals {
        DeviceID operator ""_did(unsigned long long element) {
            return DeviceID(static_cast<uint16_t>(element));
        }
        VendorID operator ""_vid(unsigned long long element) {
            return VendorID(static_cast<uint16_t>(element));
        }
    }
}