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
        auto getRegEntry() {
            if constexpr(RegisterValues<toGet>::isRanged) {
                return getCheckedValue<toGet>(RegisterValues<toGet>::code, RegisterValues<toGet>::mask);
            } else {
                return static_cast<typename RegisterValues<toGet>::values>(extractRegisterValue(readRegister(RegisterValues<toGet>::code),
                        RegisterValues<toGet>::mask));
            }
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

        void configDiagnostic(const RegisterMask& mask,bool active);

        std::vector<Diagnostic> readDiagnostic();

        bool testDiagnostic();

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

        size_type extractRegisterValue(size_t registerValue, RegisterMask registerName);

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
        auto getCheckedValue(const RegisterCodes& registerCode, const RegisterMask& mask) {
            auto value = readRegister(registerCode);
            value = extractRegisterValue(value, mask);
            auto scale = getRegisterRange<Name>();
            return scale.getActualValue(value);
        }
    };

    template< A4963RegisterNames toSet>
    struct possibleValues {
        static_assert(!RegisterValues<toSet>::isRanged, "here is no checked type allowed");
        using values = typename RegisterValues<toSet>::values;
    };

    void setRuntime(A4963& device, A4963RegisterNames toSet, uint16_t data){
        switch(toSet){
            case A4963RegisterNames::PhaseAdvance: {
                auto d = decltype(RegisterValues<A4963RegisterNames::PhaseAdvance>::min){static_cast<double>(data)};
                device.set<A4963RegisterNames::PhaseAdvance>(d);
                break;
            }
            case A4963RegisterNames::BlankTime:{
                auto d = decltype(RegisterValues<A4963RegisterNames::BlankTime>::min){data};
                device.set<A4963RegisterNames::BlankTime>(d);
                break;
            };
            case A4963RegisterNames::DeadTime:{
                auto d = decltype(RegisterValues<A4963RegisterNames::DeadTime>::min){data};
                device.set<A4963RegisterNames::DeadTime>(d);
                break;
            };
            case A4963RegisterNames::CurrentSenseThresholdVoltage:{
                auto d = decltype(RegisterValues<A4963RegisterNames::CurrentSenseThresholdVoltage>::min){static_cast<double>(data)};
                device.set<A4963RegisterNames::CurrentSenseThresholdVoltage>(d);
                break;
            };
            case A4963RegisterNames::VDSThreshold:{
                auto d = decltype(RegisterValues<A4963RegisterNames::VDSThreshold>::min){static_cast<double>(data)};
                device.set<A4963RegisterNames::VDSThreshold>(d);
                break;
            };
            case A4963RegisterNames::PositionControllerProportionalGain:{
                auto d = decltype(RegisterValues<A4963RegisterNames::PositionControllerProportionalGain>::min){static_cast<double>(data)};
                device.set<A4963RegisterNames::PositionControllerProportionalGain>(d);
                break;
            };
            case A4963RegisterNames::RecirculationMode:{
                auto d = static_cast<RegisterValues<A4963RegisterNames::RecirculationMode>::values>(data);
                device.set<A4963RegisterNames::RecirculationMode>(d);
                break;
            };
            case A4963RegisterNames::PercentFastDecay:{
                auto d = static_cast<RegisterValues<A4963RegisterNames::PercentFastDecay>::values>(data);
                device.set<A4963RegisterNames::PercentFastDecay>(d);
                break;
            };
            case A4963RegisterNames::InvertPWMInput:{
                auto d = static_cast<RegisterValues<A4963RegisterNames::InvertPWMInput>::values>(data);
                device.set<A4963RegisterNames::InvertPWMInput>(d);
                break;
            };
            case A4963RegisterNames::BemfTimeQualifier:{
                auto d = static_cast<RegisterValues<A4963RegisterNames::BemfTimeQualifier>::values>(data);
                device.set<A4963RegisterNames::BemfTimeQualifier>(d);
                break;
            };
            case A4963RegisterNames::OverspeedLimitRatio:{
                auto d = static_cast<RegisterValues<A4963RegisterNames::OverspeedLimitRatio>::values>(data);
                device.set<A4963RegisterNames::OverspeedLimitRatio>(d);
                break;
            };
            case A4963RegisterNames::DegaussCompensation:{
                auto d = static_cast<RegisterValues<A4963RegisterNames::DegaussCompensation>::values>(data);
                device.set<A4963RegisterNames::DegaussCompensation>(d);
                break;
            };
            case A4963RegisterNames::FixedPeriod:{
                auto d = decltype(RegisterValues<A4963RegisterNames::FixedPeriod>::min){data};
                device.set<A4963RegisterNames::FixedPeriod>(d);
                break;
            };
            case A4963RegisterNames::PositionControllerIntegralGain:{
                auto d = decltype(RegisterValues<A4963RegisterNames::PositionControllerIntegralGain>::min){static_cast<double>(data)};
                device.set<A4963RegisterNames::PositionControllerIntegralGain>(d);
                break;
            };
            case A4963RegisterNames::PWMDutyCycleHoldTorque:{
                auto d = decltype(RegisterValues<A4963RegisterNames::PWMDutyCycleHoldTorque>::min){static_cast<double>(data)};
                device.set<A4963RegisterNames::PWMDutyCycleHoldTorque>(d);
                break;
            };
            case A4963RegisterNames::HoldTime:{
                auto d = decltype(RegisterValues<A4963RegisterNames::HoldTime>::min){data};
                device.set<A4963RegisterNames::HoldTime>(d);
                break;
            };
            case A4963RegisterNames::PIControllerProportionalGain:{
                auto d = decltype(RegisterValues<A4963RegisterNames::PIControllerProportionalGain>::min){static_cast<double>(data)};
                device.set<A4963RegisterNames::PIControllerProportionalGain>(d);
                break;
            };
            case A4963RegisterNames::PWMDutyCycleTorqueStartup:{
                auto d = decltype(RegisterValues<A4963RegisterNames::PWMDutyCycleTorqueStartup>::min){static_cast<double>(data)};
                device.set<A4963RegisterNames::PWMDutyCycleTorqueStartup>(d);
                break;
            };
            case A4963RegisterNames::StartSpeed:{
                auto d = decltype(RegisterValues<A4963RegisterNames::StartSpeed>::min){data};
                device.set<A4963RegisterNames::StartSpeed>(d);
                break;
            };
            case A4963RegisterNames::PIControllerIntegralGain:{
                auto d = decltype(RegisterValues<A4963RegisterNames::PIControllerIntegralGain>::min){static_cast<double>(data)};
                device.set<A4963RegisterNames::PIControllerIntegralGain>(d);
                break;
            };
            case A4963RegisterNames::SpeedOutputSelection:{
                auto d = static_cast<RegisterValues<A4963RegisterNames::SpeedOutputSelection>::values>(data);
                device.set<A4963RegisterNames::SpeedOutputSelection>(d);
                break;
            };
            case A4963RegisterNames::MaximumSpeedSetting:{
                auto d = decltype(RegisterValues<A4963RegisterNames::MaximumSpeedSetting>::min){static_cast<double>(data)};
                device.set<A4963RegisterNames::MaximumSpeedSetting>(d);
                break;
            };
            case A4963RegisterNames::MotorControlMode:{
                auto d = static_cast<RegisterValues<A4963RegisterNames::MotorControlMode>::values>(data);
                device.set<A4963RegisterNames::MotorControlMode>(d);
                break;
            };
            case A4963RegisterNames::EnableStopOnFail:{
                auto d = static_cast<RegisterValues<A4963RegisterNames::EnableStopOnFail>::values>(data);
                device.set<A4963RegisterNames::EnableStopOnFail>(d);
                break;
            };
            case A4963RegisterNames::DutyCycleControl:{
                auto d = decltype(RegisterValues<A4963RegisterNames::DutyCycleControl>::min){static_cast<double>(data)};
                device.set<A4963RegisterNames::DutyCycleControl>(d);
                break;
            };
            case A4963RegisterNames::RestartControl:{
                auto d = static_cast<RegisterValues<A4963RegisterNames::RestartControl>::values>(data);
                device.set<A4963RegisterNames::RestartControl>(d);
                break;
            };
            case A4963RegisterNames::Brake:{
                auto d = static_cast<RegisterValues<A4963RegisterNames::Brake>::values>(data);
                device.set<A4963RegisterNames::Brake>(d);
                break;
            };
            case A4963RegisterNames::DirectionOfRotation:{
                auto d = static_cast<RegisterValues<A4963RegisterNames::DirectionOfRotation>::values>(data);
                device.set<A4963RegisterNames::DirectionOfRotation>(d);
                break;
            };
            case A4963RegisterNames::Run:{
                auto d = static_cast<RegisterValues<A4963RegisterNames::Run>::values>(data);
                device.set<A4963RegisterNames::Run>(d);
                break;
            };
        }
    }
}