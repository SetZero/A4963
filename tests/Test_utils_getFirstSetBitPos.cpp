//
// Created by sebastian on 19.09.18.
//


#include "catch.hpp"
#include "../src/utils/utils.h"

TEST_CASE("Convertion Test", "[Convertion Test]") {
    REQUIRE(utils::getFirstSetBitPos(0b0001001) == 0);
    REQUIRE(utils::getFirstSetBitPos(0b0001010) == 1);
    REQUIRE(utils::getFirstSetBitPos(0b0001100) == 2);
    REQUIRE(utils::getFirstSetBitPos(0b0001000) == 3);
    REQUIRE(utils::getFirstSetBitPos(0b0011000) == 3);
    REQUIRE(utils::getFirstSetBitPos(0b0110000) == 4);
}