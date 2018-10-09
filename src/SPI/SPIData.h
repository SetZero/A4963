//
// Created by keven on 22.09.18.
//

#pragma once

#pragma once
#include <stdint.h>
#include "../utils/utils.h"
#include <vector>
#include <string>
#include <exception>
#include <byteswap.h>
#include <assert.h>
#include <memory>
#include <utility>
#include <iostream>


namespace spi {

    template<typename t>
    inline auto swapEndian(t value)
    {
        if constexpr (utils::isEqual<t, uint8_t>::value || utils::isEqual<t,int8_t>::value) return value;
        else if constexpr (utils::isEqual<t,uint16_t>::value|| utils::isEqual<t,int16_t>::value) return bswap_16(value);
        else if constexpr (utils::isEqual<t,uint32_t>::value|| utils::isEqual<t,int32_t>::value) return bswap_32(value);
        else /*if constexpr (utils::isEqual<t,uint64_t>::value|| utils::isEqual<t,int64_t>::value)*/ return bswap_64(value);
    }

class SPI_Exception : public std::exception {
    const char* info;
public:
    inline explicit SPI_Exception(const char* what) : info(what){

    };

    inline const char *what() const noexcept override {
        return info;
    }
};

    class Data {
	protected:
        std::vector<uint8_t> mData = std::vector<uint8_t>();
    public:

        std::vector<uint8_t>& getData() {return mData;};
        const std::vector<uint8_t>& getData() const {return mData;};

        Data() = default;

        virtual void swap(Data& other) = 0;

        inline uint8_t operator[](const uint8_t& index) const {
            return mData[index];
        }

        inline uint8_t& operator[](const uint8_t& index) {
            return mData[index];
        }


        auto begin() const {
            return mData.begin();
        }

        auto end() const {
            return mData.end();
        }

        virtual std::unique_ptr<Data> operator+(const Data& rhs) const = 0;

        virtual void operator+=(const Data& rhs) = 0;

        virtual void operator+(uint8_t data) = 0;

        virtual explicit operator uint8_t () const = 0;

        virtual explicit operator uint16_t() const = 0;

        virtual explicit operator uint32_t() const = 0;

        virtual explicit operator uint64_t() const = 0;


        inline void operator-(const Data& rhs) = delete;
        inline void operator*(const Data& rhs) = delete;
        inline void operator/(const Data& rhs) = delete;
        inline void operator-=(const Data& rhs) = delete;
        inline void operator*=(const Data& rhs) = delete;
        inline void operator/=(const Data& rhs) = delete;

        inline uint8_t bytesUsed() const { return static_cast<uint8_t>(mData.size()); };

        virtual ~Data() = default;
    };

	enum EndianMode{
		little_endian, //small letters because macro is already named this way
		big_endian
	};

	template<unsigned char numberOfBytes = 1, EndianMode endian = little_endian >
	class SPIData : public Data {
		static constexpr EndianMode endi = endian;
	    static_assert((numberOfBytes &(numberOfBytes -1)) == 0  , " the number of bytes have to be a pow of 2");
		static_assert(numberOfBytes != 0, " 0 means no data, so this is not possible");
	public:

		SPIData() {mData.reserve(numberOfBytes);};

		template<typename t, typename ... args>
		explicit SPIData(t first,args ... ss) : SPIData() {
            static constexpr uint8_t bytesSum = (sizeof...(args)+1) * sizeof(t);
            static_assert(utils::sameTypes<t,args...>(),"there are different types in the constructor, this is not allowed");
            //static_assert(bytesSum <= numberOfBytes,"too much data");
			static_assert(bytesSum <= numberOfBytes, "too much bytes for this data type" );
			auto ins = {first, ss...};
            for(auto elem : ins){
                for(uint8_t i = sizeof(t); i > 0; i--){
                    mData.emplace_back(static_cast<uint8_t>(elem >> ((i-1) * 8)));
                }
            }
		}


        explicit SPIData(const Data& other) : SPIData() {
			mData.insert(std::end(mData), std::begin(other.getData()), std::end(other.getData()));
		}

		explicit SPIData(const std::vector<uint8_t >& other) : SPIData() {
			mData.insert(std::end(mData), std::begin(other), std::end(other));
		}

		inline void swap(Data& other) override{
			std::swap(this->mData, other.getData());
		}


		inline std::unique_ptr<Data> operator+(const Data& rhs) const override {
            auto * temp = new SPIData{*this};
			for (size_t i = 0; i < rhs.bytesUsed(); i++) {
				if (temp->mData.size() < numberOfBytes) {
					temp->mData.push_back(rhs[i]);
				} else {
                    throw SPI_Exception{"Data Overflow"};
				}
			}
			return std::unique_ptr<Data>(temp);
		}

		inline void operator+=(const Data& rhs) override {
		    Data* temp = ((*this) +rhs).get();
		    swap(*temp);
		};

		inline void operator+(uint8_t data) override{
			if (mData.size() < numberOfBytes)
				mData.push_back(data);
			else
				throw SPI_Exception{"Data Overflow"};
		}


		explicit operator uint8_t () const override {
            if (numberOfBytes > 1) throw SPI_Exception{"SPIData did not fit into a uint8_t type"};
			return mData[0];
		};

		explicit operator uint16_t() const override {
            if (numberOfBytes > 2) throw SPI_Exception{"SPIData did not fit into a uint16_t type"};
            uint16_t erg = (mData[1] << 8) | mData[0];
            if constexpr (endian == little_endian)
				return erg;
            else return swapEndian<uint16_t>(erg);
		}

		explicit operator uint32_t() const override {
            if (numberOfBytes > 4) throw SPI_Exception{"SPIData did not fit into a uint32_t type"};
            uint32_t erg = (mData[3] << 24) | (mData[2] << 16) | (mData[1] << 8) | mData[0];
            if constexpr (endian == little_endian)
				return erg;
            else return swapEndian<uint32_t>(erg);
		}

		explicit operator uint64_t() const override {
            if (numberOfBytes > 8) throw SPI_Exception{"SPIData did not fit into a uint64_t type"};
            uint64_t erg = ((uint64_t)(mData[7]) << 56) | ((uint64_t)(mData[6]) << 48) | ((uint64_t)(mData[5]) << 40) | ((uint64_t)(mData[4]) << 32)
                           |(mData[3] << 24) | (mData[2] << 16) | (mData[1] << 8)  | mData[0];
            if constexpr (endian == little_endian)
				return erg;
            else return swapEndian<uint64_t>(erg);
		}


		inline void operator-(const SPIData& rhs) = delete;
		inline void operator*(const SPIData& rhs) = delete;
		inline void operator/(const SPIData& rhs) = delete;
        inline void operator-=(const SPIData& rhs) = delete;
        inline void operator*=(const SPIData& rhs) = delete;
        inline void operator/=(const SPIData& rhs) = delete;

		~SPIData() override = default;
	};

	inline std::ostream &operator<<(std::ostream &os, const Data &data) {
		uint32_t i = 0;
		for(auto& elem: data) {
			os << "data " << i << ": " << elem;
			i++;
		}
		return os;
	}

	template<unsigned char numberOfBytes = 1, EndianMode endian = little_endian>
	inline SPIData<numberOfBytes, endian> operator+(const SPIData<numberOfBytes, endian>& lhs,const SPIData<numberOfBytes, endian>& rhs) {
		return	lhs + rhs;
	 }

	template<unsigned char numberOfBytes = 1, EndianMode endian = little_endian>
	inline void swap(SPIData<numberOfBytes, endian>& lhs, SPIData<numberOfBytes, endian>& rhs) {
		lhs.swap(rhs);
	}

    inline namespace literals {

        inline auto operator ""_spi8(unsigned long long element) {
            return std::unique_ptr<Data>(new SPIData(static_cast<uint8_t>(element)));
        }
        inline auto operator ""_spi16(unsigned long long element) {
            return std::unique_ptr<Data>(new SPIData<2>(static_cast<uint8_t>(element)));
        }
        inline auto operator ""_spi32(unsigned long long element) {
            return std::unique_ptr<Data>(new SPIData<4>(static_cast<uint8_t>(element)));
        }
        inline auto operator ""_spi64(unsigned long long element) {
            return std::unique_ptr<Data>(new SPIData<8>(static_cast<uint8_t>(element)));
        }
        inline auto operator ""_spi8_big(unsigned long long element) {
            return std::unique_ptr<Data>(new SPIData<1,big_endian>(static_cast<uint8_t>(element)));
        }
        inline auto operator ""_spi16_big(unsigned long long element) {
            return std::unique_ptr<Data>(new SPIData<2,big_endian>(static_cast<uint8_t>(element)));
        }
        inline auto operator ""_spi32_big(unsigned long long element) {
            return std::unique_ptr<Data>(new SPIData<4,big_endian>(static_cast<uint8_t>(element)));
        }
        inline auto operator ""_spi64_big(unsigned long long element) {
            return std::unique_ptr<Data>(new SPIData<8,big_endian>(static_cast<uint8_t>(element)));
        }
    }

}
