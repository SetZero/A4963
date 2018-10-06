#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include "mcp2210_api.h"
#include "SPIBridge.h"
#include "GPIOBridge.h"
#include "./utils.h"
#include <libudev.h>
//#include "USB/LibUSBDevices.h"

struct MCPIOException : public std::exception {
    const char* what() const noexcept override {
        return "MCPIOException";
    };
};
using error_pair = std::pair<int, const char*>;


namespace deviceinformations {
    inline static constexpr const char *vendor_ID = "04d8";
    inline static constexpr const char *device_ID = "00de";
};

class MCP2210 final : public spi::SPIBridge, public gpio::GPIOBridge{
public:
    enum class spiMode : uint16_t{
        mode0 = 0, //clock polarity = 0 clock phase = 0
        mode1 = 1, //clock polarity = 0 clock phase = 1
        mode2 = 2, //clock polarity = 1 clock phase = 0
        mode3 = 3  //clock polarity = 1 clock phase = 1
    };
    struct spiSettings  {
        spiMode mode                = spiMode::mode0;
        uint16_t speed			    = 20000,  //bits per second
        actcsval		    = 0xFFEF, //active chip select value
        idlecsval		    = 0xFFFF, //idle chip select value
        gpcsmask		    = 0x0010, //general purpose chip select?
        cs2datadly		    = 0,      //chip select to data delay
        data2datadly	    = 0,       // delay between subsequent data bytes
        data2csdly		    = 0;       // last data byte to chip select delay
    };
private:
    spiSettings settings;
    bool connection = false;
    static constexpr int SPI_BUF_LEN = 1024;
    inline static auto txdata = std::make_unique<unsigned char[]>(SPI_BUF_LEN);
    inline static auto rxdata = std::make_unique<unsigned char[]>(SPI_BUF_LEN);
    int fd;

    int32_t send(const uint16_t& dataCount) const;


public:
    static constexpr gpio::GPIOPin
    pin0 = gpio::GPIOPin(1<<0), pin1 = gpio::GPIOPin(1<<1), pin2 = gpio::GPIOPin(1<<2), pin3 = gpio::GPIOPin(1<<3),
    pin4 = gpio::GPIOPin(1<<4), pin5 = gpio::GPIOPin(1<<6), pin6 = gpio::GPIOPin(1<<6), pin7 = gpio::GPIOPin(1<<7);

    spiSettings getSettings() const;
    void setSettings(const spiSettings& settings);
    explicit MCP2210();
    ~MCP2210() override;
    std::shared_ptr<spi::Data>  transfer(const spi::Data& input) override;
    std::vector<std::shared_ptr<spi::Data>> transfer(const std::initializer_list<std::shared_ptr<spi::Data>>& spiData) override;
    void setGPIODirection(const gpio::gpioDirection& direction, const gpio::GPIOPin& pin) override;
    void writeGPIO(const gpio::gpioState& state, const gpio::GPIOPin& pin) override;
    gpio::gpioState readGPIO(const gpio::GPIOPin& pin) const override;
    void slaveSelect(std::shared_ptr<SPIDevice> slave) override;
    void slaveDeselect(std::shared_ptr<SPIDevice> slave) override;
    void slaveRegister(std::shared_ptr<SPIDevice> device, const gpio::GPIOPin& pin) override;
    void exceptionHandling(int32_t errorCode);
    operator bool();
    void connect();
};