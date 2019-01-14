#pragma once

#include <string>
#include <array>
#include <iostream>
#include <memory>
#include "mcp2210_api.h"
#include "SPIBridge.h"
#include "GPIOBridge.h"
#include "../utils/utils.h"
#include <libudev.h>
#include "../../inc/spdlog/spdlog.h"
#include "../../inc/spdlog/sinks/basic_file_sink.h"

#ifndef linux
    static_assert(false, "only linux supported");
#endif

using error_pair = std::pair<int, const char *>;

namespace mcp {

    struct MCPIOException : public std::exception {
        const char *what() const noexcept override {
            return "MCPIOException";
        };
    };

    inline static constexpr const char *vendor_ID = "04d8";
    inline static constexpr const char *device_ID = "00de";

    enum class spiMode : uint16_t {
        mode0 = 0, //clock polarity = 0 clock phase = 0
        mode1 = 1, //clock polarity = 0 clock phase = 1
        mode2 = 2, //clock polarity = 1 clock phase = 0
        mode3 = 3  //clock polarity = 1 clock phase = 1
    };

    struct spiSettings {
        spiMode mode = spiMode::mode0;
        uint16_t speed = 20000,  //bits per second
                actcsval = 0xFFEF, //active chip select value
                idlecsval = 0xFFFF, //idle chip select value
                gpcsmask = 0x0010, //general purpose chip select?
                cs2datadly = 0,      //chip select to data delay
                data2datadly = 0,       // delay between subsequent data bytes
                data2csdly = 0;       // last data byte to chip select delay
    };

    inline auto defaultSetup = spiSettings{};
}

using namespace mcp;
template<mcp::spiSettings& SETTINGS = defaultSetup>
class MCP2210 final : public spi::SPIBridge, public gpio::GPIOBridge {
private:
    std::shared_ptr<spdlog::logger> logger;
    struct udev *udev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices, *dev_list_entry;
    struct udev_device *dev;
    spiSettings settings = SETTINGS;
    std::unique_ptr<bool> connection = std::make_unique<bool>(false);
    static constexpr int SPI_BUF_LEN = 1024;
    std::array<unsigned char, SPI_BUF_LEN> txdata{}, rxdata{};
    int fd = -1;

    [[nodiscard]] int32_t send(const uint16_t dataCount) noexcept {
                return spi_data_xfer(fd, txdata.data(), rxdata.data(), dataCount,
                static_cast<uint16_t>(settings.mode), settings.speed, settings.actcsval, settings.idlecsval,
                settings.gpcsmask,
                settings.cs2datadly, settings.data2datadly, settings.data2csdly);
    }

public:
    static constexpr gpio::GPIOPin
            pin0 = gpio::GPIOPin(1 << 0), pin1 = gpio::GPIOPin(1 << 1), pin2 = gpio::GPIOPin(
            1 << 2), pin3 = gpio::GPIOPin(1 << 3),
            pin4 = gpio::GPIOPin(1 << 4), pin5 = gpio::GPIOPin(1 << 6), pin6 = gpio::GPIOPin(
            1 << 6), pin7 = gpio::GPIOPin(1 << 7);

    [[nodiscard]] spiSettings getSettings() const noexcept {
        return settings;
    }

    void setSettings(const spiSettings &settings) noexcept {
        this->settings = settings;
    }

    explicit MCP2210() {
        try {
            logger = spdlog::basic_logger_mt("mcplogger", "mcp_log.txt");
        } catch (const spdlog::spdlog_ex &ex) {
            std::cerr << "could not create logger: " << ex.what() << std::endl;
        }
        connect();
    }

    ~MCP2210() {
        close_device(fd);
    }

    std::unique_ptr<spi::Data> transfer(const spi::Data &input) override {
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

    std::vector<std::unique_ptr<spi::Data>>
    transfer(const std::initializer_list<std::unique_ptr<spi::Data>> &spiData) override {
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

    void setGPIODirection(const gpio::gpioDirection &direction, const gpio::GPIOPin &pin) override {
        int32_t err = 0;
        if (direction == gpio::gpioDirection::in) {
            err = gpio_direction(fd, 0x01FF, static_cast<uint8_t>(pin));
        } else if (direction == gpio::gpioDirection::out) {
            err = gpio_direction(fd, 0x0000, static_cast<uint8_t>(pin));
        }
        exceptionHandling(err);
    }

    void writeGPIO(const gpio::gpioState &state, const gpio::GPIOPin &pin) override {
        int32_t err = 0;
        if (state == gpio::gpioState::off) {
            err = gpio_write(fd, ~static_cast<uint8_t>(pin), static_cast<uint8_t>(pin));
        } else {
            err = gpio_write(fd, static_cast<uint8_t>(pin), static_cast<uint8_t>(pin));
        }
        exceptionHandling(err);
    }

    [[nodiscard]] gpio::gpioState readGPIO(const gpio::GPIOPin &pin) const override {
        int val = 0;
        exceptionHandling(gpio_read(fd, &val, static_cast<uint8_t>(pin)));
        auto erg = val == static_cast<uint8_t>(pin) ? gpio::gpioState::on
                                                    : gpio::gpioState::off; // cannot be null it would cause an exception
        return erg;
    }

    void slaveSelect(const std::shared_ptr<SPIDevice> &slave) override {
        writeGPIO(gpio::gpioState::off, slave->getSlavePin());
    }

    void slaveDeselect(const std::shared_ptr<SPIDevice> &slave) override {
        writeGPIO(gpio::gpioState::on, slave->getSlavePin());
    }

    void slaveRegister(const std::shared_ptr<SPIDevice> &device, const gpio::GPIOPin &pin) override {
        device->selectPin(pin);
    }

    void exceptionHandling(int32_t errorCode) const noexcept {
        switch (-errorCode) {
            case (ERR_NOERR) :
                return;
            case (ERR_WRERR): {
                std::cerr << " Write Error " << std::endl;
                logger->log(spdlog::level::err, (" Write Error "));
                break;
            }
            case (ERR_RDERR): {
                std::cout << " Read Error " << std::endl;
                logger->log(spdlog::level::err, (" Read Error "));
                break;
            }
            case (ERR_HWERR): {
                std::cout << " Hardware Error " << std::endl;
                logger->log(spdlog::level::err, (" Hardware Error "));
                break;
            }
            case (ERR_GETCHIPSTATUS): {
                std::cout << " Chip Status Error " << std::endl;
                logger->log(spdlog::level::err, (" Chip Status Error "));
                break;
            }
            case (ERR_GETSETTINGS): {
                std::cout << " Get Settings Error " << std::endl;
                logger->log(spdlog::level::err, (" Get Settings Error "));
                break;
            }
            case (ERR_SETSETTINGS): {
                std::cout << " set Settings Error " << std::endl;
                logger->log(spdlog::level::err, ("set Settings Error"));
                break;
            }
            case (ERR_GETSPISETTINGS): {
                std::cout << " Get SPI Settings Error " << std::endl;
                logger->log(spdlog::level::err, (" Get SPI Settings Error "));
                break;
            }
            case (ERR_SETSPISETTINGS): {
                std::cout << " set SPI Settings Error " << std::endl;
                logger->log(spdlog::level::err, (" set SPI Settings Error "));
                break;
            }
            case (ERR_ADDROUTOFRANGE): {
                std::cout << " Address out of range Error " << std::endl;
                logger->log(spdlog::level::err, (" Address out of range Error "));
                break;
            }
            case (ERR_BLOCKEDACCESS): {
                std::cout << " Blocked Access Error " << std::endl;
                logger->log(spdlog::level::err, (" Blocked Access Error "));
                break;
            }
            case (ERR_WRITEGPIO): {
                std::cout << " Write GPIO Error " << std::endl;
                logger->log(spdlog::level::err, (" Write GPIO Error "));
                break;
            }
            case (ERR_READGPIO): {
                std::cout << " Read GPIO Error " << std::endl;
                logger->log(spdlog::level::err, (" Read GPIO Error "));
                break;
            }
            case (ERR_SETGPIODIR): {
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

    [[nodiscard]] operator bool() noexcept {
        return *connection;
    }

    void connect() {
        if (!(*connection)) {
            /* Create the udev object */
            udev = udev_new();
            if (!udev) {
                std::cerr << "Can't create udev\n" << std::endl;
                logger->log(spdlog::level::err, "udev creation failed");
                throw MCPIOException();
            }

            /* Create a list of the devices in the 'hidraw' subsystem. */
            enumerate = udev_enumerate_new(udev);
            udev_enumerate_add_match_subsystem(enumerate, "hidraw");
            udev_enumerate_scan_devices(enumerate);
            devices = udev_enumerate_get_list_entry(enumerate);

            udev_list_entry_foreach(dev_list_entry, devices) {
                const char *path = nullptr;
                const char *npath = nullptr;
                path = udev_list_entry_get_name(dev_list_entry);
                dev = udev_device_new_from_syspath(udev, path);

                npath = udev_device_get_devnode(dev);

                dev = udev_device_get_parent_with_subsystem_devtype(
                        dev,
                        "usb",
                        "usb_device");
                if (!dev) {
                    std::cerr << "device error" << std::endl;
                    logger->log(spdlog::level::err, "device error");
                    throw MCPIOException();
                }

                if (std::string(udev_device_get_sysattr_value(dev, "idVendor")) ==
                std::string(mcp::vendor_ID) &&
                std::string(udev_device_get_sysattr_value(dev, "idProduct")) ==
                std::string(mcp::device_ID)) {
                    fd = open_device(npath);
                    if(fd > 0) {
                        std::cout << "device found: " << std::endl;
                        printf("  VID/PID: %s %s\n",
                               udev_device_get_sysattr_value(dev, "idVendor"),
                               udev_device_get_sysattr_value(dev, "idProduct"));
                        printf("  %s\n  %s\n",
                               udev_device_get_sysattr_value(dev, "manufacturer"),
                               udev_device_get_sysattr_value(dev, "product"));
                        printf("  serial: %s\n",
                               udev_device_get_sysattr_value(dev, "serial"));
                        break;
                    }
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
};
