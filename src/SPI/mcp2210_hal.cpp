#include "mcp2210_hal.h"

MCP2210::MCP2210() {
    try {
        logger = spdlog::basic_logger_mt("mcplogger", "mcp_log.txt");
    } catch (const spdlog::spdlog_ex &ex) {
        std::cerr << "could not create logger: " << ex.what() << std::endl;
    }
    connect();
}

MCP2210::~MCP2210() {
    delete npath;
    close_device(fd);
}

std::unique_ptr<spi::Data> MCP2210::transfer(const spi::Data &input) {
    auto tmp = input.create();
    if (*connection) {

        uint16_t i = 0;

        for (auto data : input) {
            txdata[i] = data;
            i++;
        }

        int32_t error = send(static_cast<uint16_t>(i));

        for (i = 0; i < input.bytesUsed(); i++) {
            (*tmp)[i] = rxdata[i];
        }
        exceptionHandling(error);
    }
    return tmp;
}

void MCP2210::writeGPIO(const gpio::gpioState &state, const gpio::GPIOPin &pin) {
    if (state == gpio::gpioState::off) {
        gpio_write(fd, ~static_cast<uint8_t>(pin), static_cast<uint8_t>(pin));
    } else {
        gpio_write(fd, static_cast<uint8_t>(pin), static_cast<uint8_t>(pin));
    }
}

void MCP2210::setGPIODirection(const gpio::gpioDirection &direction, const gpio::GPIOPin &pin) {
    if (direction == gpio::gpioDirection::in) {
        gpio_direction(fd, 0x01FF, static_cast<uint8_t>(pin));
    } else if (direction == gpio::gpioDirection::out) {
        gpio_direction(fd, 0x0000, static_cast<uint8_t>(pin));
    }
}

void MCP2210::slaveSelect(const std::shared_ptr<SPIDevice> &slave) {
    auto &pin = slave->getSlavePin();
    if ((bool) pin)
        writeGPIO(gpio::gpioState::off, pin);
    else {

    }
}

void MCP2210::slaveDeselect(const std::shared_ptr<SPIDevice> &slave) {
    writeGPIO(gpio::gpioState::on, slave->getSlavePin());
}

void MCP2210::slaveRegister(const std::shared_ptr<SPIDevice> &device, const gpio::GPIOPin &pin) {
    device->selectPin(pin);
}

gpio::gpioState MCP2210::readGPIO(const gpio::GPIOPin &pin) const {
    int val = 0;
    exceptionHandling(gpio_read(fd, &val, static_cast<uint8_t>(pin)));
    auto erg = val == static_cast<uint8_t>(pin) ? gpio::gpioState::on
                                                : gpio::gpioState::off; // cannot be null it would cause an exception
    return erg;
}

int32_t MCP2210::send(const uint16_t byteCount) {
    return spi_data_xfer(fd, txdata.data(), rxdata.data(), byteCount,
                         static_cast<uint16_t>(settings.mode), settings.speed, settings.actcsval, settings.idlecsval,
                         settings.gpcsmask,
                         settings.cs2datadly, settings.data2datadly, settings.data2csdly);
}

std::vector<std::unique_ptr<spi::Data>>
MCP2210::transfer(const std::initializer_list<std::unique_ptr<spi::Data>> &spiData) {
    if (*connection) {
        uint16_t i = 0;
        for (const auto &elem: spiData) {
            if (i >= SPI_BUF_LEN) break;
            auto spicontainer = elem.get();
            for (uint8_t data : *spicontainer) {
                txdata[i] = data;
                i++;
            }
        }
        exceptionHandling(send(static_cast<uint16_t>(i)));
        std::vector<std::unique_ptr<spi::Data>> dataOut = std::vector<std::unique_ptr<spi::Data>>(i);
        for (i = 0; i < spiData.size(); i++) {
            if (i >= SPI_BUF_LEN) throw MCPIOException{};
            dataOut.emplace_back(std::make_unique<spi::SPIData<>>(rxdata[i]));
        }
        return dataOut;
    }
    return std::vector<std::unique_ptr<spi::Data>>{};
}

void MCP2210::connect() {
    if (!(*connection)) {
        /* Create the udev object */
        udev = udev_new();
        if (!udev) {
            std::cerr << "Can't create udev\n" << std::endl;
            logger->log(spdlog::level::err, "udev creation failed");
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

            npath = udev_device_get_devnode(dev);

            dev = udev_device_get_parent_with_subsystem_devtype(
                    dev,
                    "usb",
                    "usb_device");
            if (!dev) {
                exit(1);
            }

            if (std::string(udev_device_get_sysattr_value(dev, "idVendor")) ==
                std::string(deviceinformations::vendor_ID) &&
                std::string(udev_device_get_sysattr_value(dev, "idProduct")) ==
                std::string(deviceinformations::device_ID)) {
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
            *connection = true;
        }
    } else {
        std::cerr << "device is already connected" << std::endl;
    }

}

MCP2210::operator bool() {
    return *connection;
}

void MCP2210::exceptionHandling(int32_t errorCode) const {
    switch (-errorCode) {
        case (ERR_NOERR) :
            return;
        case (10): {
            std::cerr << " Write Error " << std::endl;
            logger->log(spdlog::level::err, (" Write Error "));
            break;
        }
        case (20): {
            std::cout << " Read Error " << std::endl;
            logger->log(spdlog::level::err, (" Read Error "));
            break;
        }
        case (30): {
            std::cout << " Hardware Error " << std::endl;
            logger->log(spdlog::level::err, (" Hardware Error "));
            break;
        }
        case (100): {
            std::cout << " Chip Status Error " << std::endl;
            logger->log(spdlog::level::err, (" Chip Status Error "));
            break;
        }
        case (110): {
            std::cout << " Get Settings Error " << std::endl;
            logger->log(spdlog::level::err, (" Get Settings Error "));
            break;
        }
        case (120): {
            std::cout << " set Settings Error " << std::endl;
            logger->log(spdlog::level::err, ("set Settings Error"));
            break;
        }
        case (130): {
            std::cout << " Get SPI Settings Error " << std::endl;
            logger->log(spdlog::level::err, (" Get SPI Settings Error "));
            break;
        }
        case (140): {
            std::cout << " set SPI Settings Error " << std::endl;
            logger->log(spdlog::level::err, (" set SPI Settings Error "));
            break;
        }
        case (150): {
            std::cout << " Address out of range Error " << std::endl;
            logger->log(spdlog::level::err, (" Address out of range Error "));
            break;
        }
        case (160): {
            std::cout << " Blocked Access Error " << std::endl;
            logger->log(spdlog::level::err, (" Blocked Access Error "));
            break;
        }
        case (170): {
            std::cout << " Write GPIO Error " << std::endl;
            logger->log(spdlog::level::err, (" Write GPIO Error "));
            break;
        }
        case (180): {
            std::cout << " Read GPIO Error " << std::endl;
            logger->log(spdlog::level::err, (" Read GPIO Error "));
            break;
        }
        case (190): {
            std::cout << " set GPIO direction Error " << std::endl;
            logger->log(spdlog::level::err, (" set GPIO direction Error "));
            break;
        }
        default: {
            return;
        }
    }
    *connection = false;
    close_device(fd);
    std::cerr << "device disconnected" << std::endl;
}

MCP2210::spiSettings MCP2210::getSettings() const {
    return settings;
}

void MCP2210::setSettings(const MCP2210::spiSettings &settings) {
    MCP2210::settings = settings;
}