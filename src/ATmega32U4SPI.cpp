//
// Created by sebastian on 30.08.18.
//

#include "ATmega32U4SPI.h"

ATmega32u4SPI::ATmega32u4SPI(const std::shared_ptr<LibUSBDevice>& device) : mDevice{device} {
    device.get()->openDevice();
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
    mDevice.get()->sendData(spiData);
    return spi::SPIData(std::vector<unsigned char>());
}

void ATmega32u4SPI::slaveRegister(const SPIDevice &device, const gpio::GPIOPin &pin) {

}

void ATmega32u4SPI::slaveSelect(const SPIDevice &slave) {

}

void ATmega32u4SPI::slaveDeselect(const SPIDevice &slave) {

}


