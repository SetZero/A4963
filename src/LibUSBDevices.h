//
// Created by sebastian on 01.09.18.
//

#pragma once

#include <vector>
#include <cstdint>
#include <libusb.h>

class LibUSBDevice {
public:
    LibUSBDevice(uint16_t vendorID, uint16_t deviceID);

    uint16_t getVendorID() const;
    uint16_t getDeviceID() const;

private:
    uint16_t vendorID;
    uint16_t deviceID;
};

class LibUSBDeviceList {
public:
    LibUSBDeviceList();
    const std::vector<LibUSBDevice> &getDevices() const;

private:
    std::vector<LibUSBDevice> mDevices;
};

