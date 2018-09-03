//
// Created by sebastian on 03.09.18.
//

#pragma once

#include "LibUsbDevice.h"

namespace usb {
    class HIDevice : public LibUSBDevice {
    public:
        bool sendData(const spi::SPIData &data) override;

    private:
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
    };
}


