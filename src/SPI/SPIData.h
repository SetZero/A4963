//
// Created by keven on 22.09.18.
//

#pragma once

#pragma once
#include <stdint.h>
#include "utils.h"
#include <vector>
#include <string>
#include <exception>
#include <byteswap.h>
#include <assert.h>


namespace spi {

	enum EndianMode{
		little_endian, //small letters because macro is already named this way
		big_endian
	};

	template<unsigned char numberOfBytes = 1, EndianMode endian = little_endian >
	class SPIData {
		static_assert((numberOfBytes &(numberOfBytes -1)) == 0  , " the number of bytes have to be a pow of 2");
		static_assert(numberOfBytes != 0, " 0 means no data, so this is not possible");
	private:
		std::vector<uint8_t> mData = std::vector<uint8_t>();
	public:
		SPIData() = default;

		template<typename t, typename ... args>
		explicit SPIData(t first, args... ss) {
			std::initializer_list<t> ins = { first, ss... };
			static_assert(sizeof...(args)+1 <= numberOfBytes, "too much bytes for this data type" );
			if constexpr (endian == little_endian)
				mData.insert(std::begin(mData), std::begin(ins), std::end(ins));
			else { //back insert
				auto i = ins.size() -1;
				for (auto elem : ins) {
					assert(i >=0);
					mData[i] = elem;
					i--;
				}
			}
		};

		SPIData(const SPIData& other) {
			mData.insert(std::end(mData), std::begin(other.mData), std::end(other.mData));
		}

		inline void swap(SPIData& other) {
			using namespace std;
			swap(this->mData, other.mData);
		}

		inline uint8_t operator[](const uint8_t& index) const {
			assert(index >= 0);
			return index >= 0 ? mData[index] : mData[0];
		}

        inline uint8_t& operator[](const uint8_t& index) {
            assert(index >= 0);
            return index >= 0 ? mData[index] : mData[0];
        }

		inline auto begin() const {
			return mData.begin();
		}

		inline auto end() const {
			return mData.end();
		}


		inline SPIData operator+(const SPIData& rhs) const {
		    SPIData temp{*this};
			for (size_t i = 0; i < rhs.mData.size(); i++) {
				if (temp.mData.size() < numberOfBytes) {
					temp.mData.push_back(rhs.mData[i]);
				} else {
					throw std::exception{};
				}
			}
			return temp;
		}

		inline SPIData operator+=(const SPIData& rhs) {
		    SPIData temp = (*this) +rhs;
		    swap(temp);
		};

		inline void operator+(uint8_t data){
			if (mData.size() < numberOfBytes)
				mData.push_back(data);
			else
				throw std::exception{};
		}

		explicit operator uint8_t () const {
			static_assert(numberOfBytes <= 1 && numberOfBytes > 0, "SPIData did not fit into a uint8_t type");
			return mData[0];
		};

		explicit operator uint16_t() const {
			static_assert(numberOfBytes <= 2 && numberOfBytes > 0, "SPIData did not fit into a uint16_t type");
				return (mData[0] << 8) | mData[1];
		}

		explicit operator uint32_t() const {
			static_assert(numberOfBytes <= 4 && numberOfBytes > 0, "SPIData did not fit into a uint32_t type");
				return (mData[0] << 24) | (mData[1] << 16) | (mData[2] << 8) | mData[3];
		}

		explicit operator uint64_t() const {
			static_assert(numberOfBytes <= 8 && numberOfBytes > 0, "SPIData did not fit into a uint64_t type");
				return ((uint64_t)(mData[0]) << 56) | ((uint64_t)(mData[1]) << 48) | ((uint64_t)(mData[2]) << 40) | ((uint64_t)(mData[3]) << 32)
				|(mData[4] << 24) | (mData[5] << 16) | (mData[6] << 8)  | mData[7];
		}


		inline void operator-(const SPIData& rhs) = delete;
		inline void operator*(const SPIData& rhs) = delete;
		inline void operator/(const SPIData& rhs) = delete;

		inline uint8_t bytesUsed() const { return static_cast<uint8_t>(mData.size()); };

		~SPIData() = default;
	};

	template<unsigned char numberOfBytes = 1, EndianMode endian = little_endian>
	inline SPIData<numberOfBytes, endian> operator+(const SPIData<numberOfBytes, endian>& lhs,const SPIData<numberOfBytes, endian>& rhs) {
		return	rhs + rhs;
	 }

	template<unsigned char numberOfBytes = 1, EndianMode endian = little_endian>
	inline void swap(SPIData<numberOfBytes, endian>& lhs, SPIData<numberOfBytes, endian> rhs) {
		lhs.swap(rhs);
	}

	template<typename t>
	inline auto swapEndian(t value)
	{
		if constexpr (utils::isEqual<t, uint8_t>::value || utils::isEqual<t,int8_t>::value) return value;
		else if constexpr (utils::isEqual<t,uint16_t>::value|| utils::isEqual<t,int16_t>::value) return bswap_16(value);
		else if constexpr (utils::isEqual<t,uint32_t>::value|| utils::isEqual<t,int32_t>::value) return bswap_32(value);
		else /*if constexpr (utils::isEqual<t,uint64_t>::value|| utils::isEqual<t,int64_t>::value)*/ return bswap_64(value);
	}

    inline namespace literals {

        inline auto operator ""_spi8(unsigned long long element) {
            return SPIData(static_cast<uint8_t>(element));
        }
        inline auto operator ""_spi16(unsigned long long element) {
            return SPIData<2>(static_cast<uint16_t>(element));
        }
        inline auto operator ""_spi32(unsigned long long element) {
            return SPIData<4>(static_cast<uint32_t>(element));
        }
        inline auto operator ""_spi64(unsigned long long element) {
            return SPIData<8>(static_cast<uint64_t>(element));
        }
        inline auto operator ""_spi8_big(unsigned long long element) {
            return SPIData<1,big_endian>(static_cast<uint8_t>(element));
        }
        inline auto operator ""_spi16_big(unsigned long long element) {
            return SPIData<2,big_endian>(static_cast<uint16_t>(element));
        }
        inline auto operator ""_spi32_big(unsigned long long element) {
            return SPIData<4,big_endian>(static_cast<uint32_t>(element));
        }
        inline auto operator ""_spi64_big(unsigned long long element) {
            return SPIData<8,big_endian>(static_cast<uint64_t>(element));
        }
    };

}
