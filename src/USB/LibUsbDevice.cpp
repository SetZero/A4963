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

    std::vector<uint8_t> LibUSBDevice::sendData(const std::vector<uint8_t>& data) {
        if (!isOpen)
            return {};
        std::cout << "This shouldn't happen..." << std::endl;
        unsigned char spiData[data.size()];
        std::copy(data.begin(), data.end(), spiData);

        int actual_length = 0;
        int r = libusb_bulk_transfer(handle, (1 | LIBUSB_ENDPOINT_OUT), spiData, sizeof(spiData), &actual_length, 0);
        std::cout << "Sending Data Code: " << r << std::endl;
        return {};
    }
}