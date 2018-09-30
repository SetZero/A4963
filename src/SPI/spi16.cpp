//
// Created by keven on 30.09.18.
//

#include "spi16.h"

std::shared_ptr<spi::SPIData> spi::spi16::operator+(const spi::SPIData &rhs) const {
    auto temp = std::make_shared<spi16>(*this);
    for (size_t i = 0; i < rhs.bytesUsed(); i++) {
        if (temp->size < numberOfBytes) {
            temp->push_back(rhs[i]);
            //ss
        } else {
            throw std::exception{};
        }
    }
    return temp;
}

spi::spi16::spi16() : SPIData(std::make_unique<uint8_t[]>(2),2) {

}

spi::spi16::spi16(const spi16 &data) : SPIData(data){

}

spi::spi16::spi16(const std::initializer_list<uint8_t> &data) : SPIData(data,1) {

}

spi::spi16::spi16(const uint16_t &data) : spi16() {
    push_back(static_cast<uint8_t>(data >> 8));
    push_back(static_cast<uint8_t>(data));
}

spi::spi16::spi16(const SPIData &other) : SPIData(other) {

}


spi::spi16::~spi16() = default;