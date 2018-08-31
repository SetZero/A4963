#include "mcp2210_hal.h"
#include "utils.h"

/*use udev or other similar mechanisms to get the system path "/dev/hidraw1" */
MCP2210::MCP2210(std::string device) {
    //TODO: find out witch hid number is the right for the device

    fd = open_device(device.c_str());
    if(fd <= 0)
        std::cout << "Error with device: " << fd << std::endl;
    else
        std::cout << "Device successfully opened" << std::endl;
}

MCP2210::~MCP2210() {
    close_device(fd);
}

unsigned char MCP2210::transfer(unsigned char input) {
    txdata[0] = input;
    int out = spi_data_xfer(fd, txdata.get(), rxdata.get(), 1,
            static_cast<uint16_t >(spiSettings::mode), static_cast<uint16_t >(spiSettings::speed), static_cast<uint16_t >(spiSettings::actcsval),
            static_cast<uint16_t >(spiSettings::idlecsval), static_cast<uint16_t >(spiSettings::gpcsmask), static_cast<uint16_t >(spiSettings::cs2datadly),
            static_cast<uint16_t >(spiSettings::data2datadly), static_cast<uint16_t >(spiSettings::data2csdly));
    return rxdata[0];
}

std::vector<unsigned char> MCP2210::transfer(const std::vector<unsigned char>& input) {
    std::vector<unsigned char> out(input.size());

    for(auto elem: input){
        out.emplace_back(transfer(elem));
    }

    return out;
}

template<unsigned char first, typename... values>
std::vector<unsigned char> MCP2210::transfer(unsigned char f, values... val) {
    static_assert(utils::sameTypes<unsigned char,values...>(),"mismatch of types in parameter list, they all have to be from type unsigned char");
    std::vector<unsigned char> in({f,val...});
    return transfer(in);
}


void MCP2210::writeGPIO(const gpio::gpioState& state, const gpio::GPIOPin pin) {
    if(state == gpio::gpioState::off) {
        gpio_write(fd, ~pin, pin);
    } else {
        gpio_write(fd, pin, pin);
    }
}

void MCP2210::setGPIODirection(const gpio::gpioDirection& direction, const gpio::GPIOPin pin) {
    if(direction == gpio::gpioDirection::in) {
        gpio_direction(fd, 0x01FF, pin);
    } else if(direction == gpio::gpioDirection::out) {
        gpio_direction(fd, 0x0000, pin);
    }
}

void MCP2210::slaveSelect(const gpio::GPIOPin& slave) {
    writeGPIO(gpio::gpioState::off, slave);
}

void MCP2210::slaveDeselect(const gpio::GPIOPin& slave) {
    writeGPIO(gpio::gpioState::on, slave);
}
