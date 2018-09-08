//
// Created by sebastian on 03.09.18.
//

#include <thread>
#include "HIDevice.h"

std::vector<uint8_t> usb::HIDevice::sendData(const std::vector<uint8_t>& data) {
    if (!isOpen)
        return {};

    int bytes_received = 0;
    int bytes_sent = 0;
    unsigned char data_in[MAX_CONTROL_IN_TRANSFER_SIZE];
    unsigned char data_out[MAX_CONTROL_OUT_TRANSFER_SIZE];

    //std::cout << "Splitting into " << (data.size() / MAX_CONTROL_OUT_TRANSFER_SIZE) + 1 << " packages" << std::endl;
    std::vector<uint8_t> dataOutVektor;
    for(size_t i=0; i < (data.size() / MAX_CONTROL_OUT_TRANSFER_SIZE) + 1; i++) {

        size_t end = ((i+1)*MAX_CONTROL_OUT_TRANSFER_SIZE < data.size() ? MAX_CONTROL_OUT_TRANSFER_SIZE :
                      data.size() - i*MAX_CONTROL_OUT_TRANSFER_SIZE);

        std::fill(std::begin(data_out), std::end(data_out), 0);
        std::copy_n(std::begin(data) + i*MAX_CONTROL_OUT_TRANSFER_SIZE,
                    end, std::begin(data_out));

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
            bytes_received = libusb_control_transfer(
                    handle,
                    CONTROL_REQUEST_TYPE_IN,
                    HID_GET_REPORT,
                    (HID_REPORT_TYPE_INPUT << 8) | 0x00,
                    INTERFACE_NUMBER,
                    data_in,
                    sizeof(data_in),
                    TIMEOUT_MS);
            if (bytes_received > 0) {
                dataOutVektor.insert(std::end(dataOutVektor), std::begin(data_in), std::begin(data_in) + bytes_received);
            }
        }
    }
    return dataOutVektor;
}

usb::HIDevice::HIDevice(const usb::VendorID &vendorID, const usb::DeviceID &deviceID, libusb_device *device)
        : LibUSBDevice(vendorID, deviceID, device) {

}
