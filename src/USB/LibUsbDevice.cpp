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

    LibUSBDevice::LibUSBDevice(const VendorID& vendorID, const DeviceID& deviceID, libusb_device *device, size_t usbID) : vendorID(vendorID),
                                                                                                            deviceID(deviceID),
                                                                                                            device(device),
                                                                                                            mUsbID(usbID){}


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
                    throw std::logic_error{ "Unable to detach kernel driver: "};
                    return;
                }
            }

            if (libusb_claim_interface(handle, 0) < 0) {
                throw std::logic_error{"Unable to claim Interface: "};
                return;
            }

        } else {
            throw std::logic_error{"Failed to open device! (Code: " + std::to_string(ret) + ")"};
        }

    }

    void LibUSBDevice::closeDevice() {
        if (!isOpen) {
            throw std::logic_error{"This device isn't opened!"};
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

    std::vector<uint8_t> LibUSBDevice::sendData(const std::vector<uint8_t>& data) {
        throw std::logic_error{"A generic SPI Device can't send or receive data, as the protocol is unknown!"};
        return {data};
    }
}