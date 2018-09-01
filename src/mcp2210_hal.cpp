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
/*    spi::SPIData transfer(const spi::SPIData& input) const override;
    std::vector<spi::SPIData> transfer(const std::vector<spi::SPIData>& input) const;
    template<typename first, typename... values>
    std::vector<spi::SPIData> transfer(spi::SPIData f, values...) const;*/

spi::SPIData MCP2210::transfer(const spi::SPIData& input) const {
    //txdata[0] = input;
    int err = 0;
    auto temp = input.getData();
    int i = 0;
    for(auto elem: temp){
        if(i >= SPI_BUF_LEN){
            err = 42;
            break;
        };
        txdata[i] = elem;
        i++;
    }
    if(err == 0)
            err = spi_data_xfer(fd, txdata.get(), rxdata.get(), 1,
            static_cast<uint16_t >(spiSettings::mode), static_cast<uint16_t >(spiSettings::speed), static_cast<uint16_t >(spiSettings::actcsval),
            static_cast<uint16_t >(spiSettings::idlecsval), static_cast<uint16_t >(spiSettings::gpcsmask), static_cast<uint16_t >(spiSettings::cs2datadly),
            static_cast<uint16_t >(spiSettings::data2datadly), static_cast<uint16_t >(spiSettings::data2csdly));
    if(err != 0) std::cout << " error: " << err << std::endl;
    temp.clear();
    for(i = 0; i < SPI_BUF_LEN ; i++)
        temp.push_back(rxdata[i]);
    return spi::SPIData( temp );
}

void MCP2210::writeGPIO(const gpio::gpioState& state, const gpio::GPIOPin& pin) {
    if(state == gpio::gpioState::off) {
        gpio_write(fd, ~pin, pin);
    } else {
        gpio_write(fd, pin, pin);
    }
}

void MCP2210::setGPIODirection(const gpio::gpioDirection& direction, const gpio::GPIOPin& pin) {
    if(direction == gpio::gpioDirection::in) {
        gpio_direction(fd, 0x01FF, pin);
    } else if(direction == gpio::gpioDirection::out) {
        gpio_direction(fd, 0x0000, pin);
    }
}

void MCP2210::slaveSelect(const SPIDevice& slave) {
    writeGPIO(gpio::gpioState::off, slave.getSlavePin());
}

void MCP2210::slaveDeselect(const SPIDevice& slave) {
    writeGPIO(gpio::gpioState::on, slave.getSlavePin());
}

void MCP2210::slaveRegister(const SPIDevice &device, const gpio::GPIOPin &pin) {
    device.selectPin(pin);
}

gpio::gpioState MCP2210::readGPIO(const gpio::GPIOPin &pin) const {
    return pin == 1 ? gpio::gpioState::on : gpio::gpioState::off;
}