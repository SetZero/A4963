#include "mcp2210_hal.h"

MCP2210::MCP2210() {
    //tut: http://www.signal11.us/oss/udev/

    std::string device = "/dev/hidraw";
    const char* npath = nullptr;
    std::unique_ptr<stChipStatus_T> x = std::make_unique<stChipStatus_T>();
    std::string temp;
    struct udev *udev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices, *dev_list_entry;
    struct udev_device *dev;

    /* Create the udev object */
    udev = udev_new();
    if (!udev) {
        printf("Can't create udev\n");
        exit(1);
    }

    /* Create a list of the devices in the 'hidraw' subsystem. */
    enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "hidraw");
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);

    udev_list_entry_foreach(dev_list_entry, devices) {
        const char *path;

        path = udev_list_entry_get_name(dev_list_entry);
        dev = udev_device_new_from_syspath(udev, path);
#ifdef MCP_DEBUG
        printf("Device Node Path: %s\n", udev_device_get_devnode(dev));
#endif
npath = udev_device_get_devnode(dev);

        dev = udev_device_get_parent_with_subsystem_devtype(
                dev,
                "usb",
                "usb_device");
        if (!dev) {
#ifdef MCP_DEBUG
            printf("Unable to find parent usb device.");
#endif
            exit(1);
        }

        if(std::string(udev_device_get_sysattr_value(dev,"idVendor")) == std::string(deviceinformations::vendor_ID) &&
        std::string(udev_device_get_sysattr_value(dev,"idProduct")) == std::string(deviceinformations::device_ID)){
            fd = open_device(npath);
            std::cout << "dev found: "<< std::endl;
            printf("  VID/PID: %s %s\n",
                   udev_device_get_sysattr_value(dev,"idVendor"),
                   udev_device_get_sysattr_value(dev, "idProduct"));
            printf("  %s\n  %s\n",
                   udev_device_get_sysattr_value(dev,"manufacturer"),
                   udev_device_get_sysattr_value(dev,"product"));
            printf("  serial: %s\n",
                   udev_device_get_sysattr_value(dev, "serial"));
        }

        udev_device_unref(dev);
    }
    /* Free the enumerator object */
    udev_enumerate_unref(enumerate);

    udev_unref(udev);
    if(fd <= 0) std::cout << "Auto detect could not find your device, try manually " << fd << std::endl;
    else{
        gpio_setdir(fd,0);
        gpio_setval(fd,0);
    }
}

//use ls /dev grep | "hid" to find devices, then give the correct number of the spi bridge
MCP2210::MCP2210(unsigned char number) {
    std::string device = "/dev/hidraw";
    std::unique_ptr<stChipStatus_T> x = std::make_unique<stChipStatus_T>();
    device.append(std::to_string(number));
    fd = open_device(device.c_str());
    if(fd>0) {
        get_chip_status(fd, x.get());
        if (x->ucSpiState == ERR_NOERR) {
            try {
                using namespace spi::literals;
                transfer(0_spi8);
                std::cout << "device found: " << device << std::endl;
            }
            catch (std::exception& e) {
                e.what();
                close_device(fd);
            }
        } else {
            close_device(fd);
        }
    }
    else std::cout << "Error with device: " << fd << std::endl;
}

MCP2210::~MCP2210() {
    close_device(fd);
}

SPI8 MCP2210::transfer(const SPI8& input) const {
    txdata[0] = (uint8_t)input;

    int32_t err = send(1);
    //TODO: Exception handling
    if(err != ERR_NOERR) throw MCPIOException{};
#ifdef DEBUG_MCP
    if(err != 0) std::cout << " error: " << err << std::endl;
#endif
    using namespace spi::literals;
    return SPI8{rxdata[0]};
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

void MCP2210::slaveSelect(std::shared_ptr<SPIDevice> slave) {
    writeGPIO(gpio::gpioState::off, slave->getSlavePin());
}

void MCP2210::slaveDeselect(std::shared_ptr<SPIDevice> slave) {
    writeGPIO(gpio::gpioState::on, slave->getSlavePin());
}

void MCP2210::slaveRegister(std::shared_ptr<SPIDevice> device, const gpio::GPIOPin& pin) {
    device->selectPin(pin);
}

gpio::gpioState MCP2210::readGPIO(const gpio::GPIOPin &pin) const {
    return pin == 1 ? gpio::gpioState::on : gpio::gpioState::off;
}

int MCP2210::getFd() const {
    return fd;
}

int32_t MCP2210::send(const uint16_t &dataCount) const {
    return spi_data_xfer(fd, txdata.get(), rxdata.get(), dataCount,
                        static_cast<uint16_t >(spiSettings::mode), static_cast<uint16_t >(spiSettings::speed), static_cast<uint16_t >(spiSettings::actcsval),
                        static_cast<uint16_t >(spiSettings::idlecsval), static_cast<uint16_t >(spiSettings::gpcsmask), static_cast<uint16_t >(spiSettings::cs2datadly),
                        static_cast<uint16_t >(spiSettings::data2datadly), static_cast<uint16_t >(spiSettings::data2csdly));
}

std::vector<SPI8>
MCP2210::transfer(const std::initializer_list<SPI8> &spiData) const {
    uint16_t i = 0;
    //TODO: Exception handling
    if(i >= SPI_BUF_LEN) throw 42;
    for(auto elem: spiData){
        txdata[i] = (uint8_t)elem;
        i++;
    }
    send(static_cast<uint16_t>(spiData.size()));
    std::vector<SPI8> dataOut = std::vector<SPI8>{spiData.size()};
    for(i = 0; i < spiData.size(); i++){
        dataOut.emplace_back(rxdata[i]);
    }
    return dataOut;
};


//old find mechanism for hidraw:
/*
    for(auto z = 0; z < 255; z++) {
        temp = device;
        temp.append(std::to_string(z));
        fd = open_device(temp.c_str());
        if(fd>0) {
            get_chip_status(fd, x.get());
            if (x->ucSpiState == ERR_NOERR) {
                try {
                    using namespace spi::literals;
                    transfer(0_spi8);
                    std::cout << "device found: " << temp << std::endl;
                    break;
                }
                catch (std::exception& e) {
                    std::cout << e.what() <<std::endl;
                    close_device(fd);
                }
            } else {
                close_device(fd);
            }
        }
       temp.empty();
    }*/