#pragma once
#include <stdint.h>
#include <memory>
#include "src/SPI/mcp2210_hal.h"

class A4963 : public SPIDevice {
private:
    using size_type = uint16_t;

    std::shared_ptr<spi::SPIBridge> mBridge;
    enum class RegisterMask : size_type {
        /* Common Addresses */
        RegisterAddress          = 0b1110000000000000,
        WriteAddress             = 0b0001000000000000,
        GeneralData              = 0b0000111111111111,

        /* Config0 Addresses */
        RecirculationModeAddress = 0b0000110000000000,
        BlankTimeAddress         = 0b0000001111000000,
        DeadTimeAddress          = 0b0000000000111111
    };
    enum class RegisterPosition : size_type {
        /* Common Addresses */
        RegisterAddress          = 13,
        WriteAddress             = 12,
        GeneralData              = 0,

        /* Config0 Addresses */
        RecirculationModeAddress = 10,
        BlankTimeAddress         = 6,
        DeadTimeAddress          = 0
    };
    enum class RegisterCodes : uint8_t  {
        Config0    = 0b000,
        Config1    = 0b001,
        Config2    = 0b010,
        Config3    = 0b011,
        Config4    = 0b100,
        Config5    = 0b101,
        Mask       = 0b110,
        Run        = 0b111,
        Diagnostic = 0b110
    };

    enum class WriteBit : uint8_t {
        Write = 1,
        Read = 0
    };

    struct RegisterInfo {
        size_type data;
        bool dirty;
    };

    std::map<RegisterCodes, RegisterInfo> mRegisterData;

    void clearRegister(const RegisterCodes& reg);
    void writeRegister(const RegisterCodes& reg, size_type data);
    void reloadRegister(const RegisterCodes& reg);
    spi::SPIData send16bitRegister(size_type address);
    template<typename T>
    size_type createRegisterEntry(T data, const RegisterPosition& position, const RegisterMask& mask);
    size_type getRegisterEntry(const RegisterCodes& registerEntry,  const RegisterPosition& position, const RegisterMask& mask);
public:

    enum class RecirculationModeTypes : uint8_t {
        Auto = 0b00,
        High = 0b01,
        Low  = 0b10,
        Off  = 0b11
    };

    //TODO: read current values in mRegisterData
    A4963(std::shared_ptr<spi::SPIBridge> mBridge);
    void setRecirculationMode(const RecirculationModeTypes& type);
    void commit();
    uint16_t _dbg_reload_commit_and_get_register0();
};