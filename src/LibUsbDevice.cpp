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

        //TODO: move to own class (source: http://janaxelson.com/code/generic_hid.c)
        static const int CONTROL_REQUEST_TYPE_IN =
                LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE;
        static const int CONTROL_REQUEST_TYPE_OUT =
                LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE;

// From the HID spec:

        static const int HID_GET_REPORT = 0x01;
        static const int HID_SET_REPORT = 0x09;
        static const int HID_REPORT_TYPE_INPUT = 0x01;
        static const int HID_REPORT_TYPE_OUTPUT = 0x02;
        static const int HID_REPORT_TYPE_FEATURE = 0x03;

// With firmware support, transfers can be > the endpoint's max packet size.

        static const int MAX_CONTROL_IN_TRANSFER_SIZE = 8;
        static const int MAX_CONTROL_OUT_TRANSFER_SIZE = 8;

        static const int INTERFACE_NUMBER = 0;
        static const int TIMEOUT_MS = 5000;

        int bytes_received;
        int bytes_sent;
        char data_in[MAX_CONTROL_IN_TRANSFER_SIZE];
        unsigned char data_out[MAX_CONTROL_OUT_TRANSFER_SIZE] = {2, 0, 1, 0, 0, 0, 0, 0};
        int i = 0;
        int result = 0;

        /*for (i=0;i < MAX_CONTROL_OUT_TRANSFER_SIZE; i++)
        {
            data_out[i]=0x40+i;
        }*/


        bytes_sent = libusb_control_transfer(
                handle,
                CONTROL_REQUEST_TYPE_OUT,
                HID_SET_REPORT,
                (HID_REPORT_TYPE_OUTPUT << 8) | 0x00,
                INTERFACE_NUMBER,
                data_out,
                sizeof(data_out),
                TIMEOUT_MS);

        if (bytes_sent >= 0) {
            std::cout << "Feature report data sent:" << std::endl;
            for (i = 0; i < bytes_sent; i++) {
                std::cout << std::hex << data_out[i] << std::endl;
            }
        }
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