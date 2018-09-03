//
// Created by sebastian on 03.09.18.
//

#include "DevicesList.h"

namespace device_list {
    const DeviceList::DeviceListType DeviceList::deviceTypeMap = {
            {{ATmega::vendorID, ATmega::deviceID}, usb::DeviceTypes::HID}
    };
}