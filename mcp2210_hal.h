#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "SPIBridge.h"

class MCP2210 : public spi::SPIBridge{
private:
    static constexpr int SPI_BUF_LEN = 1024;
    unsigned char txdata[SPI_BUF_LEN], rxdata[SPI_BUF_LEN];
    int fd;

    struct spiSettings {
        static const int spimode			= 0;
        static const int speed			    = 1000000;
        static const int actcsval		    = 0xFFEF;
        static const int idlecsval		    = 0xFFFF;
        static const int gpcsmask		    = 0x0010;
        static const int cs2datadly		    = 0;
        static const int data2datadly	    = 0;
        static const int data2csdly		    = 0;
    };
public:
    MCP2210(std::string device);
    ~MCP2210();
    virtual unsigned char transfer(unsigned char input);
    virtual std::vector<unsigned char> transfer(std::vector<unsigned char>& input);
    virtual void setGPIODirection(const spi::gpioDirection& direction, const spi::GPIOPins pin);
    virtual void writeGPIO(const spi::gpioState& state, const spi::GPIOPins pin);
    virtual void slaveSelect();
    virtual void slaveDeselect();
};