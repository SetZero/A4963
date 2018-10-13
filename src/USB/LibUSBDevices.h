//
// Created by sebastian on 01.09.18.
//

#pragma once

#include <vector>
#include <cstdint>
#ifdef __linux__
    #include <libusb.h>
#elif _WIN32
    #ifdef __cplusplus
        extern "C" {
    #endif
        #include <libusb.h>
    #ifdef __cplusplus
        };
    #endif
#endif
#include <optional>
#include "LibUsbDevice.h"
#include "HIDevice.h"
#include "DevicesList.h"

namespace usb {

    class LibUSBDeviceList {
    public:
        LibUSBDeviceList();
        virtual ~LibUSBDeviceList();

        size_t size();
        const std::optional<std::shared_ptr<LibUSBDevice>> findDevice(const VendorID& vendorID, const DeviceID& deviceID);

    private:
        libusb_context *context = nullptr;
        libusb_device **list = nullptr;
        ssize_t device_count = 0;

        std::unordered_map<size_t, std::shared_ptr<LibUSBDevice>> mDevices;
        size_t mDeviceID = 0;

        void addDevice(VendorID vendorID, DeviceID deviceID, libusb_device *device);
    };
}

