//
// Created by sebastian on 01.09.18.
//

#pragma once

#include <vector>
#include <cstdint>
#include <libusb.h>
#include <optional>
#include "LibUsbDevice.h"
#include "HIDevice.h"
#include "DevicesList.h"

namespace usb {
    enum class DeviceTypes {
        Generic, HID
    };

    class LibUSBDeviceList {
    public:
        LibUSBDeviceList();
        virtual ~LibUSBDeviceList();

        const std::vector<std::shared_ptr<LibUSBDevice>> &getDevices() const;
        const std::optional<std::shared_ptr<LibUSBDevice>> findDevice(const VendorID& vendorID, const DeviceID& deviceID);

    private:
        libusb_context *context = nullptr;
        libusb_device **list = nullptr;
        ssize_t device_count = 0;
        //DeviceID did = spi::ATmega32u4SPI::deviceID;

        std::vector<std::shared_ptr<LibUSBDevice>> mDevices;

    };
}

