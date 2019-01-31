//
// Created by sebastian on 03.09.18.
//

#pragma once
#include <memory>
#include <libusb.h>
#include <iostream>

namespace usb {
    enum class DeviceTypes {
        Generic, HID
    };

    class DeviceID {
    public:
        explicit DeviceID(uint16_t deviceID) noexcept;
        uint16_t getDeviceID() const;
        bool operator==(const DeviceID &rhs) const;
        bool operator!=(const DeviceID &rhs) const;

    private:
        uint16_t deviceID;
    };

    class VendorID {
    public:
        explicit VendorID(uint16_t vendorID) noexcept;
        bool operator==(const VendorID &rhs) const;
        bool operator!=(const VendorID &rhs) const;

        [[nodiscard]] constexpr uint16_t getVendorID() const noexcept {
            return vendorID;
        }

    private:
        uint16_t vendorID;
    };

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