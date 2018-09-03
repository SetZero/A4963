//
// Created by sebastian on 03.09.18.
//

#include "HIDevice.h"

bool usb::HIDevice::sendData(const spi::SPIData &data) {
    if (!isOpen)
        return false;

    int bytes_received;
    int bytes_sent;
    char data_in[MAX_CONTROL_IN_TRANSFER_SIZE];
    unsigned char data_out[MAX_CONTROL_OUT_TRANSFER_SIZE] = {2, 0, 1, 0, 0, 0, 0, 0};
    int i = 0;
    int result = 0;


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
