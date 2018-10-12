//
// Created by keven on 10.10.18.
//

#include "catch.hpp"
#include "SPIData.h"

TEST_CASE("SPIData Constructor"){
    uint8_t t8 = 42;
    uint16_t t16 = 4711;
    uint32_t t32 = 1000000;
    uint64_t t64 = 18000000000;
    spi::Data* d1 = new spi::SPIData<1>{t8};
    spi::Data* d2 = new spi::SPIData<2>{t16};
    spi::Data* d4 = new spi::SPIData<4>{t32};
    spi::Data* d8 = new spi::SPIData<8>{t64};
    spi::Data* d_big4 = new spi::SPIData<4,spi::big_endian>{t32};
    REQUIRE((*d1)[0] == t8);
    REQUIRE((*d2)[0] == static_cast<uint8_t>(t16));
    REQUIRE((*d2)[1] == static_cast<uint8_t>(t16 >> 8));
    REQUIRE((*d4)[0] == static_cast<uint8_t>(t32));
    REQUIRE((*d4)[3] == static_cast<uint8_t>(t32 >> 24));
    REQUIRE((*d_big4)[3] == (*d4)[0]);
    REQUIRE((*d_big4)[0] == static_cast<uint8_t>(t32 >> 24));
    REQUIRE((*d8)[0] == static_cast<uint8_t>(t64));
    REQUIRE((*d8)[7] == static_cast<uint8_t>(t64 >> 56));
}

TEST_CASE("SPIData Conversion"){
    uint8_t t8 = 42;
    uint16_t t16 = 4711;
    uint32_t t32 = 1000000;
    uint64_t t64 = 18000000000;
    spi::Data* d1 = new spi::SPIData<1>{t8};
    spi::Data* d2 = new spi::SPIData<2>{t16};
    spi::Data* d4 = new spi::SPIData<4>{t32};
    spi::Data* d8 = new spi::SPIData<8>{t64};
    REQUIRE((uint8_t)(*d1) == t8);
    REQUIRE((uint16_t)(*d2) == t16);
    REQUIRE((uint32_t)(*d4) == t32);
    REQUIRE((uint64_t)(*d8) == t64);
    REQUIRE_THROWS((uint8_t)(*d2));
    REQUIRE_THROWS((uint16_t)(*d4));
    REQUIRE_THROWS((uint32_t)(*d8));
}

TEST_CASE("SPIData Literals"){
    using namespace spi::literals;
    auto d1 = 42_spi8;
    auto d2 = 4711_spi16;
    auto d4 = 1000000_spi32;
    auto d8 = 18000000000_spi64;
    REQUIRE((*d1).bytesUsed() == 1);
    REQUIRE((uint8_t)(*d1) == 42);
    REQUIRE((*d2).bytesUsed() == 2);
    REQUIRE((uint16_t)(*d2) == 4711);
    REQUIRE((*d4).bytesUsed() == 4);
    REQUIRE((uint32_t)(*d4) == 1000000);
    REQUIRE((*d8).bytesUsed() == 8);
    REQUIRE((uint64_t)(*d8) == static_cast<uint64_t>(18000000000));
}

TEST_CASE("SPIData Operator+"){
    auto d2 = spi::SPIData<2>{static_cast<uint8_t >(42)};
    using namespace spi::literals;
    auto d3 = (d2 + *21_spi8);
    REQUIRE((*d3)[0] == 42);
    REQUIRE((*d3)[1] == 21);
}

TEST_CASE("SPIData read"){
    using namespace spi;
    Data* d = new SPIData<2>(static_cast<uint16_t>(4711));
    //we handle it if we would'nt know the concrete type here
    Data* tmp = d->create().release();
    int i = 1;
    for(auto elem : *d){
        //send elem .......
        *tmp+= 42/i; //simulate the returned value
        i++;
    }
    REQUIRE((*tmp)[0] == 42); //1st data received
    REQUIRE((*tmp)[1] == 21); //2nd data received
}

