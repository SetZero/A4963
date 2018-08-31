#include "mcp2210_hal.h"
#include "utils.h"

/*use udev or other similar mechanisms to get the system path "/dev/hidraw1" */
MCP2210::MCP2210(std::string device) {
    //TODO: find out witch hid number is the right for the device
    gpiopins = std::vector<spi::GPIOPin>(8);
    gpiopins.emplace_back(
            spi::GPIOPin(1<<0),
            spi::GPIOPin(1<<1),
            spi::GPIOPin(1<<2),
            spi::GPIOPin(1<<3),
            spi::GPIOPin(1<<4),
            spi::GPIOPin(1<<5),
            spi::GPIOPin(1<<6),
            spi::GPIOPin(1<<7));
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
                            static_cast<uint16_t >(spiSettings::mode), static_cast<uint16_t >(spiSettings::speed), static_cast<uint16_t >(spiSettings::actcsval),
                            static_cast<uint16_t >(spiSettings::idlecsval), static_cast<uint16_t >(spiSettings::gpcsmask), static_cast<uint16_t >(spiSettings::cs2datadly)
            , static_cast<uint16_t >(spiSettings::data2datadly), static_cast<uint16_t >(spiSettings::data2csdly));
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


void MCP2210::writeGPIO(const spi::gpioState& state, const spi::GPIOPin pin) {
    if(state == spi::gpioState::off) {
        gpio_write(fd, ~((int)pin), (int)pin);
    } else {
        gpio_write(fd, (int)pin, (int)pin);
    }
}

void MCP2210::setGPIODirection(const spi::gpioDirection& direction, const spi::GPIOPin pin) {
    if(direction == spi::gpioDirection::in) {
        gpio_direction(fd, 0x01FF, (int)pin);
    } else if(direction == spi::gpioDirection::out) {
        gpio_direction(fd, 0x0000, (int)pin);
    }
}

void MCP2210::slaveSelect(const spi::GPIOPin& slave) {
    writeGPIO(spi::gpioState::off, slave);
}

void MCP2210::slaveDeselect(const spi::GPIOPin& slave) {
    writeGPIO(spi::gpioState::on, slave);
}

std::vector<spi::GPIOPin> MCP2210::pins() const {
    return gpiopins;
}
