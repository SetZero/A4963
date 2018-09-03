//
// Created by sebastian on 01.09.18.
//

#include "LibUSBDevices.h"

LibUSBDeviceList::LibUSBDeviceList() {
    int rc = 0;
    rc = libusb_init(&context);

    device_count = libusb_get_device_list(context, &list);

    for (size_t idx = 0; idx < device_count; ++idx) {
        libusb_device *device = list[idx];
        libusb_device_descriptor desc = {0};

        rc = libusb_get_device_descriptor(device, &desc);

        mDevices.push_back(std::make_shared<LibUSBDevice>(desc.idVendor, desc.idProduct, device));
    }
}

const std::vector<std::shared_ptr<LibUSBDevice>> &LibUSBDeviceList::getDevices() const {
    return mDevices;
}

const std::optional<std::shared_ptr<LibUSBDevice>> LibUSBDeviceList::findDevice(uint16_t vendorID, uint16_t deviceID) {
    for(std::shared_ptr<LibUSBDevice>& device : mDevices) {
        if(device->getDeviceID() == deviceID && device->getVendorID() == vendorID) {
            return device;
        }
    }
    return std::nullopt;
}

LibUSBDeviceList::~LibUSBDeviceList() {
    libusb_free_device_list(list, static_cast<int>(device_count));
    libusb_exit(context);
}
