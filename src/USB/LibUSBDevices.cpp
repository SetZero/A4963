//
// Created by sebastian on 01.09.18.
//

#include "LibUSBDevices.h"

namespace usb {
    LibUSBDeviceList::LibUSBDeviceList() {
        int rc = 0;
        rc = libusb_init(&context);

        device_count = libusb_get_device_list(context, &list);

        for (size_t idx = 0; idx < device_count; ++idx) {
            libusb_device *device = list[idx];
            libusb_device_descriptor desc = {0};

            rc = libusb_get_device_descriptor(device, &desc);
            VendorID vendorID{desc.idVendor};
            DeviceID deviceID{desc.idProduct};
            mDevices.push_back(std::make_shared<LibUSBDevice>(vendorID, deviceID, device));

            //using namespace device_list;
            //DeviceListType::const_iterator = deviceTypeMap.find({vendorID, deviceID});
            //std::unordered_map<int, int>::const_iterator value = deviceTypeMap.find(1);
                                                                            //= deviceTypeMap.find({vendorID, deviceID});
            /*if(value == deviceTypeMap.end())
                mDevices.push_back(std::make_shared<LibUSBDevice>(vendorID, deviceID, device));
            else {
                switch(value->second) {
                    case DeviceTypes::HID:
                        mDevices.push_back(std::make_shared<HIDevice>(vendorID, deviceID, device));
                    case DeviceTypes::Generic:
                        mDevices.push_back(std::make_shared<LibUSBDevice>(vendorID, deviceID, device));
                }
            }*/
        }
    }

    const std::vector<std::shared_ptr<LibUSBDevice>> &LibUSBDeviceList::getDevices() const {
        return mDevices;
    }

    const std::optional<std::shared_ptr<LibUSBDevice>>
    LibUSBDeviceList::findDevice(const VendorID& vendorID, const DeviceID& deviceID) {
        for (std::shared_ptr<LibUSBDevice> &device : mDevices) {
            if (device->getDeviceID() == deviceID && device->getVendorID() == vendorID) {
                return device;
            }
        }
        return std::nullopt;
    }

    LibUSBDeviceList::~LibUSBDeviceList() {
        libusb_free_device_list(list, static_cast<int>(device_count));
        libusb_exit(context);
    }
}