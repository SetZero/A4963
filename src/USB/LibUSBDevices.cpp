#include <utility>

//
// Created by sebastian on 01.09.18.
//

#include "LibUSBDevices.h"

namespace usb {
    LibUSBDeviceList::LibUSBDeviceList() {
        libusb_init(&context);

        device_count = libusb_get_device_list(context, &list);

        for (ssize_t idx = 0; idx < device_count; ++idx) {
            libusb_device *device = list[idx];
            libusb_device_descriptor desc{};

            libusb_get_device_descriptor(device, &desc);
            VendorID vendorID{desc.idVendor};
            DeviceID deviceID{desc.idProduct};
            //mDevices.push_back(std::make_shared<LibUSBDevice>(vendorID, deviceID, device));

            using namespace device_list;

            auto value = DeviceList::deviceTypeMap.find({vendorID, deviceID});
            if(value == DeviceList::deviceTypeMap.end())
                addDevice(vendorID, deviceID, device);
            else {
                switch(value->second) {
                    case DeviceTypes::HID:
                        addDevice(vendorID, deviceID, device);
                        break;
                    case DeviceTypes::Generic:
                    default:
                        addDevice(vendorID, deviceID, device);
                        break;
                }
            }
        }
    }

    /*const std::vector<std::shared_ptr<LibUSBDevice>> &LibUSBDeviceList::getDevices() const {
        return mDevices;
    }*/

    const std::optional<std::shared_ptr<LibUSBDevice>>
    LibUSBDeviceList::findDevice(const VendorID& vendorID, const DeviceID& deviceID) {
        for (std::pair<const size_t, std::shared_ptr<LibUSBDevice>> &device : mDevices) {
            if (device.second->getDeviceID() == deviceID && device.second->getVendorID() == vendorID) {
                return device.second;
            }
        }
        return std::nullopt;
    }

    LibUSBDeviceList::~LibUSBDeviceList() {
        libusb_free_device_list(list, static_cast<int>(device_count));
        libusb_exit(context);
    }

    void LibUSBDeviceList::addDevice(VendorID vendorID, DeviceID deviceID, libusb_device *device) {
        mDevices[mDeviceID] = std::make_shared<HIDevice>(vendorID, deviceID, device, mDeviceID);
        mDeviceID++;
    }

    size_t LibUSBDeviceList::size() {
        return mDevices.size();
    }
}