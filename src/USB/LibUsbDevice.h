//
// Created by sebastian on 03.09.18.
//

#pragma once

#include <memory>
#include <libusb.h>
#include <iostream>
#include "src/SPI/SPIBridge.h"

namespace usb {
    //TODO: Move this in own class, create HID device if detected from map
    class DeviceID {
    public:
        explicit DeviceID(uint16_t deviceID);
        uint16_t getDeviceID() const;
        bool operator==(const DeviceID &rhs) const;
        bool operator!=(const DeviceID &rhs) const;

    private:
        uint16_t deviceID;
    };

    class VendorID {
    public:
        explicit VendorID(uint16_t vendorID);
        uint16_t getVendorID() const;
        bool operator==(const VendorID &rhs) const;
        bool operator!=(const VendorID &rhs) const;

    private:
        uint16_t vendorID;
    };

    class LibUSBDevice {
    public:
        LibUSBDevice(const VendorID& vendorID, const DeviceID& deviceID, libusb_device *device);
        virtual ~LibUSBDevice();
        //There should always only be one device with a given device handle (and it should be managed by the device list)!
        LibUSBDevice(const LibUSBDevice &other) = delete;
        LibUSBDevice &operator=(const LibUSBDevice &other) = delete;
        LibUSBDevice(LibUSBDevice &&other) noexcept = delete;
        LibUSBDevice &operator=(LibUSBDevice &&other) noexcept = delete;
        VendorID getVendorID() const;
        DeviceID getDeviceID() const;
        void openDevice();
        void closeDevice();

        //TODO: change this to something else, not spi::SPIData... (This is not yet a spi device, but more generic)
        virtual bool sendData(const spi::SPIData &data);

    protected:
        void _closeDevice();

        VendorID vendorID;
        DeviceID deviceID;
        libusb_device *device = nullptr;
        libusb_device_handle *handle = nullptr;
        bool isOpen = false;
    };

    inline namespace literals {
        DeviceID operator ""_did(unsigned long long element);
        VendorID operator ""_vid(unsigned long long element);
    }

    struct pair_hash {
        template <class T1, class T2>
        std::size_t operator () (const std::pair<T1,T2> &p) const {
            using std::hash;

            auto h1 = hash<T1>{}(p.first);
            auto h2 = hash<T2>{}(p.second);

            return h1 ^(h2 << 1);
        }
    };
}

namespace std {

    template <>
    struct hash<usb::VendorID>
    {
        std::size_t operator()(const usb::VendorID& v) const {
            using std::size_t;
            using std::hash;


            return (hash<uint16_t >()(v.getVendorID()));
        }
    };

    template <>
    struct hash<usb::DeviceID>
    {
        std::size_t operator()(const usb::DeviceID& d) const {
            using std::size_t;
            using std::hash;

            return (hash<uint16_t >()(d.getDeviceID()));
        }
    };
}