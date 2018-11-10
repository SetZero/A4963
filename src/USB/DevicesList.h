//
// Created by sebastian on 03.09.18.
//

#pragma once

#include <unordered_map>
#include "src/SPI/ATmega32U4SPI.h"
//#include "USBUtils.h"


namespace device_list {
    struct DeviceList {
        using ATmega = spi::ATmega32u4SPI;
        using DeviceListType = std::unordered_map<std::pair<usb::VendorID, usb::DeviceID>, usb::DeviceTypes, usb::pair_hash>;
        static const DeviceListType deviceTypeMap;
    };
}