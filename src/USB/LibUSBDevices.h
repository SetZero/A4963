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

    class LibUSBDeviceList {
    public:
        using devices_t = std::unordered_map<size_t, std::shared_ptr<LibUSBDevice>>;
        using const_iterator = devices_t::const_iterator;

        LibUSBDeviceList();
        virtual ~LibUSBDeviceList();

        [[nodiscard]] size_t size() const& noexcept {
            return mDevices.size();
        }
        [[nodiscard]] const std::optional<std::shared_ptr<LibUSBDevice>> findDevice(const VendorID& vendorID, const DeviceID& deviceID) const& noexcept;


        const const_iterator begin() const { return mDevices.begin(); }
        const const_iterator end() const { return mDevices.end(); }
        const const_iterator cbegin() const { return mDevices.cbegin(); }
        const const_iterator cend() const { return mDevices.cend(); }
    private:
        libusb_context *context = nullptr;
        libusb_device **list = nullptr;
        ssize_t device_count = 0;

        devices_t mDevices;
        size_t mDeviceID = 0;

        void addDevice(VendorID vendorID, DeviceID deviceID, libusb_device *device);
    };
}

