//
// Created by keven on 30.09.18.
//

#pragma once

#include "spidata.h"

namespace spi {
    class spi8 : public SPIData {
    public:
        static constexpr uint8_t BYTES = 1;

        spi8();

        explicit spi8(const uint8_t& data);

        spi8(const spi8 &data);

        spi8(const std::initializer_list<uint8_t> &data);

        std::shared_ptr<SPIData> operator+(const SPIData &rhs) const override;

        ~spi8() override;

    };

    inline namespace literals {
        inline std::shared_ptr<SPIData> operator"" _spi8(unsigned long long elem){
           return std::make_shared<spi8>(static_cast<uint8_t>(elem));
        }
    }
}

