//
// Created by sebastian on 03.09.18.
//

#pragma once

#include <memory>
#include <libusb.h>
#include <iostream>
#include <atomic>
#include "src/SPI/SPIBridge.h"
#include "USBUtils.h"

namespace usb {
    class LibUSBDevice {
    public:
        LibUSBDevice(const VendorID& vendorID, const DeviceID& deviceID, libusb_device *device, size_t usbID);
        virtual ~LibUSBDevice() noexcept;
        //There should always only be one device with a given device handle (and it should be managed by the device list)!
        LibUSBDevice(const LibUSBDevice &other) = delete;
        LibUSBDevice &operator=(const LibUSBDevice &other) = delete;
        LibUSBDevice(LibUSBDevice &&other) noexcept = delete;
        LibUSBDevice &operator=(LibUSBDevice &&other) noexcept = delete;
        VendorID getVendorID() const;
        DeviceID getDeviceID() const;
        void openDevice();
        void closeDevice();
        virtual std::vector<uint8_t> sendData(const std::vector<uint8_t>& data);

    protected:
        void _closeDevice() noexcept;

        VendorID vendorID;
        DeviceID deviceID;
        libusb_device *device = nullptr;
        libusb_device_handle *handle = nullptr;
        size_t mUsbID;
        std::atomic<bool> isOpen = false;
    };

    inline namespace literals {
        DeviceID operator ""_did(unsigned long long element);
        VendorID operator ""_vid(unsigned long long element);
    }
}
