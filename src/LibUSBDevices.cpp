//
// Created by sebastian on 01.09.18.
//

#include "LibUSBDevices.h"

uint16_t LibUSBDevice::getVendorID() const {
    return vendorID;
}

uint16_t LibUSBDevice::getDeviceID() const {
    return deviceID;
}

LibUSBDevice::LibUSBDevice(uint16_t vendorID, uint16_t deviceID) : vendorID(vendorID), deviceID(deviceID) {}


LibUSBDeviceList::LibUSBDeviceList() {
    libusb_context *context = nullptr;
    libusb_device **list = nullptr;
    int rc = 0;
    ssize_t count = 0;

    rc = libusb_init(&context);

    count = libusb_get_device_list(context, &list);

    for (size_t idx = 0; idx < count; ++idx) {
        libusb_device *device = list[idx];
        libusb_device_descriptor desc = {0};

        rc = libusb_get_device_descriptor(device, &desc);

        LibUSBDevice usbDevice{desc.idVendor, desc.idProduct};
        mDevices.push_back(usbDevice);
    }

    libusb_free_device_list(list, static_cast<int>(count));
    libusb_exit(context);
}

const std::vector<LibUSBDevice> &LibUSBDeviceList::getDevices() const {
    return mDevices;
}
