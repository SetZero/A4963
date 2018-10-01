//
// Created by keven on 29.09.18.
//

#pragma once

#include <cstdint>
#include <vector>
#include <cassert>
#include <memory>
#include "utils.h"

namespace spi {
class SPIData {
protected:
    std::vector<uint8_t > mData;
    uint8_t  numberOfBytes = 0;
public:
    SPIData() = delete;

    inline SPIData(std::vector<uint8_t> vector, uint8_t bytes) : mData(std::move(vector)), numberOfBytes(bytes){
        mData.reserve(bytes);
    };

    inline SPIData(const SPIData& other) :numberOfBytes(other.numberOfBytes){
        mData.reserve(numberOfBytes);
        mData.insert(std::end(mData), std::begin(other.mData), std::end(other.mData));
    }

    inline SPIData(const std::initializer_list<uint8_t>& list, uint8_t bytes) :  numberOfBytes(bytes){
        mData = std::vector<uint8_t >{};
        mData.reserve(numberOfBytes);
        mData.insert(std::end(mData), std::begin(list), std::end(list));
    };

    inline void swap(SPIData& other) {
        std::swap(this->mData, other.mData);
        std::swap(this->numberOfBytes, other.numberOfBytes);
    }

    inline uint8_t& operator[](const uint8_t& index) {
        return mData[index];
    }

    inline uint8_t operator[](const uint8_t& index) const {
        return mData[index];
    }

    inline auto begin() const {
        return mData.begin();
    }

    inline auto end() const {
        return mData.end();
    }

    virtual std::shared_ptr<SPIData> operator+(const SPIData& rhs) const = 0;

    inline void operator+=(const SPIData& rhs) {
        std::shared_ptr<SPIData> temp = (*this) + rhs;
        swap(*temp.get());
    };

    inline void operator+(uint8_t data){
        if (mData.size() < numberOfBytes)
            mData.push_back(data);
        else
            throw std::exception{};
    }

    inline explicit operator uint8_t () const {
        assert(numberOfBytes <= 1 && numberOfBytes > 0);
        return mData[0];
    };

    inline explicit operator uint16_t() const {
        assert(numberOfBytes <= 2 && numberOfBytes > 0);
        return (mData[0] << 8) | mData[1];
    }

    inline explicit operator uint32_t() const {
        assert(numberOfBytes <= 4 && numberOfBytes > 0);
        return (mData[0] << 24) | (mData[1] << 16) | (mData[2] << 8) | mData[3];
    }

    inline explicit operator uint64_t() const {
        assert(numberOfBytes <= 8 && numberOfBytes > 0);
        return ((uint64_t)(mData[0]) << 56) | ((uint64_t)(mData[1]) << 48) | ((uint64_t)(mData[2]) << 40) | ((uint64_t)(mData[3]) << 32)
               |(mData[4] << 24) | (mData[5] << 16) | (mData[6] << 8)  | mData[7];
    }


    inline void operator-(const SPIData& rhs) = delete;
    inline void operator*(const SPIData& rhs) = delete;
    inline void operator/(const SPIData& rhs) = delete;
    inline void operator-=(const SPIData& rhs) = delete;
    inline void operator*=(const SPIData& rhs) = delete;
    inline void operator/=(const SPIData& rhs) = delete;

    inline uint8_t bytesUsed() const { return static_cast<uint8_t>(mData.size()); };
    inline uint8_t bytes() const {return numberOfBytes;};
    inline uint8_t cap() const {return mData.capacity();};


    inline virtual ~SPIData() {};

};

}

inline void swap(spi::SPIData& lhs,spi::SPIData& rhs) {
    lhs.swap(rhs);
}
