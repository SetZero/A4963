//
// Created by sebastian on 30.08.18.
//

#include "ATmega32U4SPI.h"

ATmega32u4SPI::ATmega32u4SPI() {
    std::cout << "Starting Atmega32u4..." << std::endl;
    LibUSBDeviceList deviceList;
    std::cout << "Found " << deviceList.getDevices().size() << " devices" << std::endl;
    for(LibUSBDevice device : deviceList.getDevices()) {
        std::cout << device.getVendorID() << " : " << device.getDeviceID() << std::endl;
    }
}

void ATmega32u4SPI::setGPIODirection(const gpio::gpioDirection &direction, gpio::GPIOPin pin) {

}

void ATmega32u4SPI::writeGPIO(const gpio::gpioState &state, gpio::GPIOPin pin) {

}

gpio::gpioState ATmega32u4SPI::readGPIO(gpio::GPIOPin pin) const {
    gpio::gpioState state = gpio::gpioState::off;
    return state;
}

spi::SPIData ATmega32u4SPI::transfer(const spi::SPIData &spiData) const {
    return spi::SPIData(std::vector<unsigned char>());
}

void ATmega32u4SPI::slaveRegister(const SPIDevice &device, const gpio::GPIOPin &pin) {

}

void ATmega32u4SPI::slaveSelect(const SPIDevice &slave) {

}

void ATmega32u4SPI::slaveDeselect(const SPIDevice &slave) {

}
