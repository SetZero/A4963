//
// Created by keven on 30.09.18.
//

#pragma once

#include "spidata.h"


namespace spi {
    class spi16 : public SPIData {
    public:

        static constexpr uint8_t BYTES = 2;

        spi16();

        spi16(const uint16_t& data);

        spi16(const spi16& data);

        spi16(const SPIData& other);

        spi16(const std::initializer_list<uint8_t> &data);

        std::shared_ptr<spi::SPIData> operator+(const spi::SPIData &rhs) const override;

        ~spi16() override;

    };

    inline namespace literals {
        inline std::shared_ptr<SPIData> operator"" _spi16(unsigned long long elem){
            return std::make_shared<spi16>(static_cast<uint16_t >(elem));
        }
    }
}

