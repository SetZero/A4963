//
// Created by sebastian on 03.09.18.
//

#pragma once

#include <algorithm>
#include "LibUsbDevice.h"

namespace usb {
    class HIDevice : public LibUSBDevice {
    public:
        HIDevice(const VendorID& vendorID, const DeviceID& deviceID, libusb_device *device, size_t usbID);
        std::vector<uint8_t> sendData(const std::vector<uint8_t>& data) override;

    private:
        //(source: http://janaxelson.com/code/generic_hid.c)
        static constexpr int CONTROL_REQUEST_TYPE_IN =
                LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE;
        static constexpr int CONTROL_REQUEST_TYPE_OUT =
                LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE;

        static constexpr int INTERRUPT_IN_ENDPOINT = 0x81;
        static constexpr int INTERRUPT_OUT_ENDPOINT = 0x01;

        static constexpr int MAX_INTERRUPT_IN_TRANSFER_SIZE = 8;
        static constexpr int MAX_INTERRUPT_OUT_TRANSFER_SIZE = 8;

        // From the HID spec:

        static constexpr int HID_GET_REPORT = 0x01;
        static constexpr int HID_SET_REPORT = 0x09;
        static constexpr int HID_REPORT_TYPE_INPUT = 0x01;
        static constexpr int HID_REPORT_TYPE_OUTPUT = 0x02;
        static constexpr int HID_REPORT_TYPE_FEATURE = 0x03;

        // With firmware support, transfers can be > the endpoint's max packet size.
        static constexpr int MAX_CONTROL_IN_TRANSFER_SIZE = 8;
        static constexpr int MAX_CONTROL_OUT_TRANSFER_SIZE = 8;

        static constexpr int INTERFACE_NUMBER = 0;
        static constexpr int TIMEOUT_MS = 5000;
    };
}


