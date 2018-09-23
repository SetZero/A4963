#include "mcp2210_hal.h"

MCP2210::MCP2210() {
    //tut: http://www.signal11.us/oss/udev/

    connect();
}



//deprecated
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

SPI8 MCP2210::transfer(const SPI8& input) {
    if(connection) {
        txdata[0] = (uint8_t) input;
        int32_t err = send(1);
        if (err != ERR_NOERR) {
            exceptionHandling(err);
        }
#ifdef DEBUG_MCP
        if(err != 0) std::cout << " error: " << err << std::endl;
#endif
        return SPI8{rxdata[0]};
    }
    using namespace spi::literals;
    return 0_spi8;
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

int32_t MCP2210::send(const uint16_t &dataCount) const {
    return spi_data_xfer(fd, txdata.get(), rxdata.get(), dataCount,
                        static_cast<uint16_t >(spiSettings::mode), static_cast<uint16_t >(spiSettings::speed), static_cast<uint16_t >(spiSettings::actcsval),
                        static_cast<uint16_t >(spiSettings::idlecsval), static_cast<uint16_t >(spiSettings::gpcsmask), static_cast<uint16_t >(spiSettings::cs2datadly),
                        static_cast<uint16_t >(spiSettings::data2datadly), static_cast<uint16_t >(spiSettings::data2csdly));
}

std::vector<SPI8>
MCP2210::transfer(const std::initializer_list<SPI8> &spiData)  {
    if(connection) {
        uint16_t i = 0;
        for (const auto &elem: spiData) {
            if (i >= SPI_BUF_LEN) break;
            txdata[i] = (uint8_t) elem;
            i++;
        }
        int32_t error = send(static_cast<uint16_t>(spiData.size()));
        if (error != ERR_NOERR) exceptionHandling(error);
        std::vector<SPI8> dataOut = std::vector<SPI8>{i};
        for (i = 0; i < spiData.size(); i++) {
            if (i >= SPI_BUF_LEN) break;
            dataOut.emplace_back(rxdata[i]);
        }
        return dataOut;
    }
    return std::vector<SPI8>{};
}

void MCP2210::connect() {
    if(!connection) {
        const char *npath = nullptr;
        struct udev *udev;
        struct udev_enumerate *enumerate;
        struct udev_list_entry *devices, *dev_list_entry;
        struct udev_device *dev;

        /* Create the udev object */
        udev = udev_new();
        if (!udev) {
            std::cerr << "Can't create udev\n" << std::endl;
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

            if (std::string(udev_device_get_sysattr_value(dev, "idVendor")) ==
            std::string(deviceinformations::vendor_ID) &&
            std::string(udev_device_get_sysattr_value(dev, "idProduct")) == std::string(deviceinformations::device_ID)){
                fd = open_device(npath);
                std::cout << "device found: " << std::endl;
                printf("  VID/PID: %s %s\n",
                       udev_device_get_sysattr_value(dev, "idVendor"),
                       udev_device_get_sysattr_value(dev, "idProduct"));
                printf("  %s\n  %s\n",
                       udev_device_get_sysattr_value(dev, "manufacturer"),
                       udev_device_get_sysattr_value(dev, "product"));
                printf("  serial: %s\n",
                       udev_device_get_sysattr_value(dev, "serial"));
            }

            udev_device_unref(dev);
        }
        /* Free the enumerator object */
        udev_enumerate_unref(enumerate);

        udev_unref(udev);
        if (fd <= 0) std::cout << "could not detect your device, check the device and try to reconnect." << std::endl;
        else { //connection was successful, init the gpios
            gpio_setdir(fd, 0);
            gpio_setval(fd, 0);
            connection = true;
        }
    } else {
        std::cout << "device is already connected" << std::endl;
    }
}

MCP2210::operator bool() {
    return connection;
}

//TODO: debug strings
void MCP2210::exceptionHandling(int32_t errorCode) {
    close_device(fd);
    connection = false;
    std::cerr << "device disconnected" << std::endl;
    throw MCPIOException{};
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