#pragma once
#include <stdint.h>
#include <memory>
#include <chrono>
#include <any>
#include "src/SPI/mcp2210_hal.h"
#include "src/utils/scales/UnitScale.h"
#include "utils/utils.h"
#include "A4963RegisterInfo.h"

namespace NS_A4963 {

    using namespace std::literals::chrono_literals;

    class A4963 : public SPIDevice {
    public:
        using size_type = uint16_t;

        enum class ScalableRegisterEntries : uint8_t {
            DeadTime,
            BlankTime
        };

        enum class RecirculationModeTypes : uint8_t {
            Auto = 0b00,
            High = 0b01,
            Low = 0b10,
            Off = 0b11
        };

        enum class PercentFastDecayTypes : uint8_t {
            T12_5Percent = 0,
            T25Percent = 1
        };

        enum class InvertPWMInputTypes : uint8_t {
            NormalTrueLogic = 0,
            InverterLogic = 1
        };

        enum class BemfTimeQualifier : uint8_t {
            DebounceTimer = 0,
            WindowTimer = 1
        };

        explicit A4963(std::shared_ptr<spi::SPIBridge> mBridge);

        void commit();

        void show_register();

        //Config0
        void setRecirculationMode(const RecirculationModeTypes &type);

        template<typename Rep, typename Period>
        std::optional<const std::chrono::duration<Rep, Period>>
        setBlankTime(const std::chrono::duration<Rep, Period> &time);

        template<typename Rep, typename Period>
        std::optional<const std::chrono::duration<Rep, Period>>
        setDeadTime(const std::chrono::duration<Rep, Period> &time);

        //Config1
        void setPercentFastDecay(const PercentFastDecayTypes &type);

        void invertPWMInput(const InvertPWMInputTypes &type);


        template<A4963RegisterNames type>
        auto getRegisterRange() const {
            return std::any_cast<typename RegisterValues<type>::type>(data.at(type));
        }
    private:

        std::shared_ptr<spi::SPIBridge> mBridge;
        enum class RegisterMask : size_type {
            /* Common Addresses */
            RegisterAddress                     = 0b1110000000000000,
            WriteAddress                        = 0b0001000000000000,
            RegisterAndWriteAddress             = 0b1111000000000000,
            GeneralData                         = 0b0000111111111111,

            /* Config0 Addresses */
            RecirculationModeAddress            = 0b0000110000000000,
            BlankTimeAddress                    = 0b0000001111000000,
            DeadTimeAddress                     = 0b0000000000111111,

            /* Config1 Addresses */
            PercentFastDecayAddress             = 0b0000100000000000,
            InvertPWMInputAddress               = 0b0000010000000000,
            CurrentSenseThresholdVoltageAddress = 0b0000001111000000,
            BemfTimeQualifierAddress            = 0b0000000000100000,
            VDSThresholdAddress                 = 0b0000000000011111,

        };
        enum class RegisterCodes : uint8_t {
            Config0     = 0b000,
            Config1     = 0b001,
            Config2     = 0b010,
            Config3     = 0b011,
            Config4     = 0b100,
            Config5     = 0b101,
            Mask        = 0b110,
            Run         = 0b111,
            Diagnostic  = 0b110
        };

        enum class WriteBit : uint8_t {
            Write = 1,
            Read = 0
        };

        struct RegisterInfo {
            size_type data;
            bool dirty;
        };

        template<A4963RegisterNames reg>
        constexpr std::pair<A4963RegisterNames, typename RegisterValues<reg>::type> mapEntry(typename RegisterValues<reg>::type range) {
            return std::pair<A4963RegisterNames, typename RegisterValues<reg>::type>{reg, range};
        }


        const std::map<A4963RegisterNames, std::any> data = {
                mapEntry<A4963RegisterNames::BlankTime>({400ns, 6us, 0us}),
                mapEntry<A4963RegisterNames::DeadTime>({50ns, 3.15us, 100ns})
                //mapEntry<A4963RegisterNames::CurrentSenseThresholdVoltage>(),

        };

        std::map<RegisterCodes, RegisterInfo> mRegisterData;

        void clearRegister(const RegisterCodes &reg);

        void clearRegister(const RegisterCodes &reg, const A4963::RegisterMask &mask);

        void writeRegisterEntry(const RegisterCodes &reg, const A4963::RegisterMask &mask, size_type data);

        void markRegisterForReload(const RegisterCodes &reg);

        spi::SPIData send16bitRegister(size_type address);

        template<typename T>
        size_type createRegisterEntry(T data, const RegisterMask &mask);

        size_type getRegisterEntry(const RegisterCodes &registerEntry, const RegisterMask &mask);

        size_type readRegister(const RegisterCodes &registerCodes);

        void commit(const RegisterCodes &registerCodes);

        template<typename TUnitType>
        std::optional<TUnitType>
        insertCheckedValue(UnitScale<TUnitType, A4963::size_type> scale, const RegisterMask &mask,
                           const RegisterCodes &registerCode) {
            if (auto checkedValue = scale.convertValue(time)) {
                A4963::size_type data = createRegisterEntry(*checkedValue, mask);
                writeRegisterEntry(registerCode, mask, data);
                return {(scale.getActualValue(*checkedValue))};
            }
            return std::nullopt;
        }

    };

    template<typename Rep, typename Period>
    std::optional<const std::chrono::duration<Rep, Period>>
    A4963::setBlankTime(const std::chrono::duration<Rep, Period> &time) {
        using namespace std::chrono_literals;
        static UnitScale<std::chrono::duration<long double, std::nano>, A4963::size_type> scale{{.precision = 400ns, .maxValue = 6us, .minValue = 0us}};

        if (auto checkedValue = scale.convertValue(time)) {
            A4963::size_type data = createRegisterEntry(*checkedValue, RegisterMask::BlankTimeAddress);
            writeRegisterEntry(RegisterCodes::Config0, RegisterMask::BlankTimeAddress, data);
            return {std::chrono::duration_cast<std::chrono::duration<Rep, Period>>(
                    scale.getActualValue(*checkedValue))};
        }
        return std::nullopt;
        //return insertCheckedValue(scale, RegisterMask::BlankTimeAddress, RegisterCodes::Config0);
    }

    template<typename Rep, typename Period>
    std::optional<const std::chrono::duration<Rep, Period>>
    A4963::setDeadTime(const std::chrono::duration<Rep, Period> &time) {
        using namespace std::chrono_literals;
        static UnitScale<std::chrono::duration<long double, std::nano>, A4963::size_type> scale{{.precision = 50ns, .maxValue = 3.15us, .minValue = 100ns}};

        if (auto checkedValue = scale.convertValue(time)) {
            A4963::size_type data = createRegisterEntry(*checkedValue, RegisterMask::DeadTimeAddress);
            writeRegisterEntry(RegisterCodes::Config0, RegisterMask::DeadTimeAddress, data);
            return {std::chrono::duration_cast<std::chrono::duration<Rep, Period>>(
                    scale.getActualValue(*checkedValue))};
        }
        //return insertCheckedValue(scale, RegisterMask::DeadTimeAddress, RegisterCodes::Config0);
    }
}