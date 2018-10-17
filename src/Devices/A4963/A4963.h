#pragma once
#include <stdint.h>
#include <memory>
#include <chrono>
#include <any>
#include <src/SPI/SPIDevice.h>
#include "src/utils/scales/UnitScale.h"
#include "utils/utils.h"
#include "SPI/SPIBridge.h"
#include "A4963RegisterInfo.h"

namespace NS_A4963 {

    using namespace std::literals::chrono_literals;
    using namespace CustomDataTypes::Electricity::literals;

    class A4963 : public SPIDevice {

    public:
        using size_type = uint16_t;

        explicit A4963(std::shared_ptr<spi::SPIBridge> mBridge);

        void commit();

        void show_register();


        template<A4963RegisterNames reg>
        auto getRegisterRange() const {
            return RegisterValues<reg>::value;
        }

        template< A4963RegisterNames toGet>
        auto getRegEntry(){
            if constexpr(RegisterValues<toGet>::isRanged)
                return getCheckedValue<toGet>(RegisterValues<toGet>::code);
            else
                return readRegister(RegisterValues<toGet>::code);
        }

        template< A4963RegisterNames toSet, typename = std::enable_if_t<RegisterValues<toSet>::isRanged>>
        auto set(decltype(RegisterValues<toSet>::min) value){
            return insertCheckedValue<toSet>(value, RegisterValues<toSet>::mask, RegisterValues<toSet>::code);
        }

        template< A4963RegisterNames toSet, typename = std::enable_if_t<(!RegisterValues<toSet>::isRanged)>>
        auto set(typename RegisterValues<toSet>::values  value){
            return setRegisterEntry(static_cast<size_type>(value),RegisterValues<toSet>::mask, RegisterValues<toSet>::code);
        }

    private:

        std::shared_ptr<spi::SPIBridge> mBridge;

        enum class WriteBit : uint8_t {
            Write = 1,
            Read = 0
        };

        struct RegisterInfo {
            size_type data;
            bool dirty;
        };


        std::map<RegisterCodes, RegisterInfo> mRegisterData;

        void clearRegister(const RegisterCodes &reg);

        void clearRegister(const RegisterCodes &reg, const detail::RegisterMask &mask);

        void writeRegisterEntry(const RegisterCodes &reg, const detail::RegisterMask &mask, size_type data);

        void markRegisterForReload(const RegisterCodes &reg);

        std::unique_ptr<spi::Data> send16bitRegister(size_type address);


        void setRegisterEntry(size_type data, const RegisterMask &mask, const RegisterCodes& registerEntry);

        size_type createRegisterEntry(size_type data, const RegisterMask &mask);

        size_type getRegisterEntry(const RegisterCodes &registerEntry, const RegisterMask &mask);

        void commit(const RegisterCodes &registerCodes);

        size_type readRegister(const RegisterCodes &registerCodes, bool forceNoReload = false);

        template<A4963RegisterNames Name, typename T>
        std::optional<const T>
        insertCheckedValue(const T& value, const RegisterMask& mask, const RegisterCodes& registerName) {
            auto scale = getRegisterRange<Name>();
            if (auto checkedValue = scale.convertValue(value)) {
                A4963::size_type data = createRegisterEntry(*checkedValue, mask);
                writeRegisterEntry(registerName, mask, data);
                if constexpr(utils::is_duration<T>::value) {
                    return {std::chrono::duration_cast<T>(
                            scale.getActualValue(*checkedValue))};
                } else {
                    return {static_cast<T>(scale.getActualValue(*checkedValue))};
                }
            }
            return std::nullopt;
        }


        //TODO:
        template<A4963RegisterNames Name>
        auto
        getCheckedValue(const RegisterCodes& registerName) {
            auto scale = getRegisterRange<Name>();
            return scale.getActualValue(readRegister(registerName));
        }
    };

    template< A4963RegisterNames toSet>
    struct possibleValues {
        static_assert(!RegisterValues<toSet>::isRanged, "here is no checked type allowed");
        using values = typename RegisterValues<toSet>::values;
    };
}