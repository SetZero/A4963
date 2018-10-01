//
// Created by keven on 30.09.18.
//

#include "spi8.h"

std::shared_ptr<spi::SPIData> spi::spi8::operator+(const SPIData &rhs) const {
    auto temp = std::make_shared<spi8>(*this);
    for (size_t i = 0; i < rhs.bytesUsed(); i++) {
        if (temp->mData.size() < numberOfBytes) {
            temp->mData.push_back(rhs[i]);
        } else {
            throw std::exception{};
        }
    }
    return temp;
}

spi::spi8::spi8() : SPIData(BYTES) {

}

spi::spi8::spi8(const spi8 &data) = default;

spi::spi8::spi8(const std::initializer_list<uint8_t> &data) : SPIData(data,BYTES) {

}

spi::spi8::spi8(const uint8_t &data) : spi8() {
    mData.push_back(data);
}

spi::spi8::~spi8() = default;
