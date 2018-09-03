//
// Created by sebastian on 03.09.18.
//

#pragma once

#include <memory>
#include <libusb.h>
#include <iostream>
#include "SPIBridge.h"

class LibUSBDevice {
public:
    LibUSBDevice(uint16_t vendorID, uint16_t deviceID, libusb_device *device);
    virtual ~LibUSBDevice();

    //There should always only be one device with a given device handle (and it should be managed by the device list)!
    LibUSBDevice(const LibUSBDevice& other) = delete;
    LibUSBDevice& operator=(const LibUSBDevice& other) = delete;
    LibUSBDevice(LibUSBDevice&& other) noexcept = delete;
    LibUSBDevice& operator=(LibUSBDevice&& other) noexcept = delete;

    uint16_t getVendorID() const;
    uint16_t getDeviceID() const;
    void openDevice();
    void closeDevice();
    //TODO: change this to something else, not spi::SPIData... (This is not yet a spi device, but more generic)
    bool sendData(const spi::SPIData& data);

private:
    void _closeDevice();
    uint16_t vendorID;
    uint16_t deviceID;
    libusb_device *device = nullptr;
    libusb_device_handle *handle = nullptr;
    bool isOpen = false;
};

