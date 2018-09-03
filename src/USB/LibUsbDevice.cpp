//
// Created by sebastian on 03.09.18.
//

#include "LibUsbDevice.h"

namespace usb {
    VendorID LibUSBDevice::getVendorID() const {
        return vendorID;
    }

    DeviceID LibUSBDevice::getDeviceID() const {
        return deviceID;
    }

    LibUSBDevice::LibUSBDevice(const VendorID& vendorID, const DeviceID& deviceID, libusb_device *device) : vendorID(vendorID),
                                                                                                            deviceID(deviceID),
                                                                                                            device(device) {}

    LibUSBDevice::~LibUSBDevice() {
        _closeDevice();
    }

    void LibUSBDevice::openDevice() {
        if (isOpen) {
            std::cout << "Device is already opened!" << std::endl;
            return;
        }

        int ret = libusb_open(device, &handle);

        if (LIBUSB_SUCCESS == ret) {
            isOpen = true;
            std::cout << "Device Opened!" << std::endl;

            if (libusb_kernel_driver_active(handle, 0) == 1) {
                if (libusb_detach_kernel_driver(handle, 0) < 0) {
                    std::cout << "Unable to detach kernel driver: " << std::endl;
                    return;
                }
            }

            if (libusb_claim_interface(handle, 0) < 0) {
                std::cout << "Unable to claim Interface: " << std::endl;
                return;
            }

        } else {
            std::cout << "Failed to open device! (Code: " << ret << ")" << std::endl;
        }

    }

    void LibUSBDevice::closeDevice() {
        if (!isOpen) {
            std::cout << "This device isn't opened!" << std::endl;
            return;
        }
        _closeDevice();
    }

    void LibUSBDevice::_closeDevice() {
        if (isOpen) {
            isOpen = false;
            libusb_attach_kernel_driver(handle, 0);
            libusb_release_interface(handle, 0);
            libusb_close(handle);
        }
    }

    bool LibUSBDevice::sendData(const spi::SPIData &data) {
        if (!isOpen)
            return false;

        /*unsigned char spiData[data.getData().size()];
        std::copy(data.getData().begin(), data.getData().end(), spiData);

        int actual_length = 0;
        int r = libusb_bulk_transfer(handle, (1 | LIBUSB_ENDPOINT_OUT), spiData, sizeof(spiData), &actual_length, 0);
        std::cout << "Sending Data Code: " << r << std::endl;
        return false;*/
        return true;
    }

/*LibUSBDevice &LibUSBDevice::operator=(LibUSBDevice &&other) noexcept  {
    std::swap(vendorID, other.vendorID);
    std::swap(deviceID, other.deviceID);
    std::swap(device, other.device);
    return *this;
}*/
    DeviceID::DeviceID(uint16_t deviceID) : deviceID(deviceID) {}

    uint16_t DeviceID::getDeviceID() const {
        return deviceID;
    }

    bool DeviceID::operator==(const DeviceID &rhs) const {
        return deviceID == rhs.deviceID;
    }

    bool DeviceID::operator!=(const DeviceID &rhs) const {
        return !(rhs == *this);
    }

    VendorID::VendorID(uint16_t vendorID) : vendorID(vendorID) {}

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