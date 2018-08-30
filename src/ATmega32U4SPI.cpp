//
// Created by sebastian on 30.08.18.
//

#include "ATmega32U4SPI.h"

ATmega32u4SPI::ATmega32u4SPI() {
    libusb_context *context = NULL;
    libusb_device **list = NULL;
    int rc = 0;
    ssize_t count = 0;

    rc = libusb_init(&context);
    assert(rc == 0);

    count = libusb_get_device_list(context, &list);
    assert(count > 0);

    for (size_t idx = 0; idx < count; ++idx) {
        libusb_device *device = list[idx];
        libusb_device_descriptor desc = {0};

        rc = libusb_get_device_descriptor(device, &desc);
        assert(rc == 0);

        std::cout << "Vendor:Device = " << std::hex << desc.idVendor << ":" << std::hex << desc.idProduct << std::endl;
    }

    libusb_free_device_list(list, count);
    libusb_exit(context);
}

unsigned char ATmega32u4SPI::transfer(unsigned char input) {
    return 0;
}

std::vector<unsigned char> ATmega32u4SPI::transfer(std::vector<unsigned char> &input) {
    return std::vector<unsigned char>();
}

void ATmega32u4SPI::setGPIODirection(const spi::gpioDirection &direction, const spi::GPIOPins pin) {

}

void ATmega32u4SPI::writeGPIO(const spi::gpioState &state, const spi::GPIOPins pin) {

}

void ATmega32u4SPI::slaveSelect() {

}

void ATmega32u4SPI::slaveDeselect() {

}
