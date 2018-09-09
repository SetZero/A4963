#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include "mcp2210_api.h"
#include "SPIBridge.h"
#include "GPIOBridge.h"

class MCP2210 final : public spi::SPIBridge, public gpio::GPIOBridge{
private:
    static constexpr int SPI_BUF_LEN = 1024;
    inline static auto txdata = std::unique_ptr<unsigned char[]>(new unsigned char[SPI_BUF_LEN]);
    inline static auto rxdata = std::unique_ptr<unsigned char[]>(new unsigned char[SPI_BUF_LEN]);
    int fd;

    enum class spiMode : uint16_t{
        mode0 = 0, //clock polarity = 0 clock phase = 0
        mode1 = 1, //clock polarity = 0 clock phase = 1
        mode2 = 2, //clock polarity = 1 clock phase = 0
        mode3 = 3  //clock polarity = 1 clock phase = 1
    };

    enum class spiSettings : uint16_t {
        mode                = static_cast<uint16_t>(spiMode::mode0),
        speed			    = 65535,  //bits per second
        actcsval		    = 0xFFEF, //active chip select value
        idlecsval		    = 0xFFFF, //idle chip select value
        gpcsmask		    = 0x0010, //general purpose chip select?
        cs2datadly		    = 0,      //chip select to data delay
        data2datadly	    = 0,       // delay between subsequent data bytes
        data2csdly		    = 0       // last data byte to chip select delay
    };
    std::map<std::shared_ptr<SPIDevice>, gpio::GPIOPin> mSlaves;

public:

    static constexpr gpio::GPIOPin
    pin0 = gpio::GPIOPin(1<<0), pin1 = gpio::GPIOPin(1<<1), pin2 = gpio::GPIOPin(1<<2), pin3 = gpio::GPIOPin(1<<3),
    pin4 = gpio::GPIOPin(1<<4), pin5 = gpio::GPIOPin(1<<6), pin6 = gpio::GPIOPin(1<<6), pin7 = gpio::GPIOPin(1<<7);

    explicit MCP2210(std::string device);
    ~MCP2210() override;
    spi::SPIData transfer(const spi::SPIData& input) const override;
    void setGPIODirection(const gpio::gpioDirection& direction, gpio::GPIOPin pin) override;
    void writeGPIO(const gpio::gpioState& state, gpio::GPIOPin pin) override;
    gpio::gpioState readGPIO(gpio::GPIOPin pin) const override;
    void slaveRegister(std::shared_ptr<SPIDevice> device, const gpio::GPIOPin& pin) override;
    void slaveSelect(std::shared_ptr<SPIDevice> slave) override;
    void slaveDeselect(std::shared_ptr<SPIDevice> slave) override;
};