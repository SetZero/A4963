#include "mcp2210_hal.h"


MCP2210::MCP2210(std::string device) {
    gpiopins = std::vector<spi::GPIOPin>(8);
    gpiopins.push_back(spi::GPIOPin(1<<0));
    gpiopins.push_back(spi::GPIOPin(1<<1));
    gpiopins.push_back(spi::GPIOPin(1<<2));
    gpiopins.push_back(spi::GPIOPin(1<<3));
    gpiopins.push_back(spi::GPIOPin(1<<4));
    gpiopins.push_back(spi::GPIOPin(1<<5));
    gpiopins.push_back(spi::GPIOPin(1<<6));
    gpiopins.push_back(spi::GPIOPin(1<<7));
    fd = open_device(device.c_str());
    if(fd <= 0)
        std::cout << "Error with device: " << fd << std::endl;
    else
        std::cout << "Device successfully opened" << std::endl;
}

MCP2210::~MCP2210() {
    close_device(fd);    
    //delete[] txdata; 
    //delete[] rxdata;
}

unsigned char MCP2210::transfer(unsigned char input) {
    txdata[0] = input;
    int out = spi_data_xfer(fd, txdata, rxdata, 1,
								spiSettings::spimode, spiSettings::speed, spiSettings::actcsval, spiSettings::idlecsval,
								spiSettings::gpcsmask, spiSettings::cs2datadly, spiSettings::data2datadly,
								spiSettings::data2csdly);
    return rxdata[0];
}

std::vector<unsigned char> MCP2210::transfer(std::vector<unsigned char>& input) {
    if(input.size() > sizeof(txdata) / sizeof(int)) {
        std::cout   << "Can't transfer this much data, maximum allowed: " << sizeof(txdata) / sizeof(int)
                    << ", sent: " << input.size() << std::endl;
        return std::vector<unsigned char>();
    }

    std::copy(input.begin(), input.end(), txdata);

    int out = spi_data_xfer(fd, txdata, rxdata, input.size(),
								spiSettings::spimode, spiSettings::speed, spiSettings::actcsval, spiSettings::idlecsval,
								spiSettings::gpcsmask, spiSettings::cs2datadly, spiSettings::data2datadly,
								spiSettings::data2csdly);
    if(out > 0) {
        return std::vector<unsigned char>(rxdata, rxdata + sizeof(rxdata) / sizeof(rxdata[0]));
    } else {
        //TODO: Add error message
        return std::vector<unsigned char>();
    }
}

void MCP2210::writeGPIO(const spi::gpioState& state, const spi::GPIOPin pin) {
    if(state == spi::gpioState::off) {
        gpio_write(fd, ~pin, pin);
    } else {
        gpio_write(fd, pin, pin);
    }
}

void MCP2210::setGPIODirection(const spi::gpioDirection& direction, const spi::GPIOPin pin) {
    if(direction == spi::gpioDirection::in) {
        gpio_direction(fd, 0x01FF, pin);
    } else if(direction == spi::gpioDirection::out) {
        gpio_direction(fd, 0x0000, pin);
    }
}

void MCP2210::slaveSelect() {
    //TODO: More generic (not only pin0 for SS and off for select)
    writeGPIO(spi::gpioState::off, spi::pin0);
}

void MCP2210::slaveDeselect() {
    writeGPIO(spi::gpioState::on, spi::pin0);
}

std::vector<spi::GPIOPin> MCP2210::pins() const {
    return std::vector<spi::GPIOPin>(8);
}
