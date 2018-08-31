#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "mcp2210_api.h"
#include "SPIBridge.h"

class MCP2210 final : public spi::SPIBridge{
private:
    static constexpr int SPI_BUF_LEN = 1024;
    unsigned char txdata[SPI_BUF_LEN], rxdata[SPI_BUF_LEN];
    int fd;

    enum class spiMode : uint16_t{
        mode0 = 0, //clock polarity = 0 clock phase = 0
        mode1 = 1, //clock polarity = 0 clock phase = 1
        mode2 = 2, //clock polarity = 1 clock phase = 0
        mode3 = 3  //clock polarity = 1 clock phase = 1
    };

    enum class spiSettings : uint16_t {
        mode                = static_cast<uint16_t >(spiMode::mode0),
        speed			    = 20000,  //bits per second
        actcsval		    = 0xFFEF, //active chip select value
        idlecsval		    = 0xFFFF, //idle chip select value
        gpcsmask		    = 0x0010, //general purpose chip select?
        cs2datadly		    = 0,      //chip select to data delay
        data2datadly	    = 0,       // delay between subsequent data bytes
        data2csdly		    = 0       // last data byte to chip select delay
    };

public:
    explicit MCP2210(std::string device);
    ~MCP2210() override;
    unsigned char transfer(unsigned char input) override;
    std::vector<unsigned char> transfer(const std::vector<unsigned char>& input) override;
    template<unsigned char first, typename... values>
    std::vector<unsigned char> transfer(unsigned char f, values...);
    std::vector<spi::GPIOPin> pins() const;
    void setGPIODirection(const spi::gpioDirection& direction, spi::GPIOPin pin) override;
    void writeGPIO(const spi::gpioState& state, spi::GPIOPin pin) override;
    void slaveSelect(const spi::GPIOPin& slave) override;
    void slaveDeselect(const spi::GPIOPin& slave) override;
};