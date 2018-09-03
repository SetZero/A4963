//
// Created by sebastian on 01.09.18.
//

#pragma once

#include <vector>
#include <cstdint>
#include <libusb.h>
#include <optional>
#include "LibUsbDevice.h"

class LibUSBDeviceList {
public:
    LibUSBDeviceList();

    virtual ~LibUSBDeviceList();

    const std::vector<std::shared_ptr<LibUSBDevice>> &getDevices() const;
    const std::optional<std::shared_ptr<LibUSBDevice>> findDevice(uint16_t vendorID, uint16_t deviceID);

private:
    libusb_context *context = nullptr;
    libusb_device **list = nullptr;
    ssize_t device_count = 0;

    std::vector<std::shared_ptr<LibUSBDevice>> mDevices;

};

