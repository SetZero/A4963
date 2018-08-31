cmake_minimum_required(VERSION 3.12)
project(SPI)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)
include_directories(inc)
include_directories(src)


add_executable(SPI
        inc/mcp2210_api.c
        inc/mcp2210_api.h
        src/25LC256.cpp
        src/25LC256.h
        src/ATmega32U4SPI.cpp
        src/ATmega32U4SPI.h
        src/mcp2210_hal.cpp
        src/mcp2210_hal.h
        src/SPIBridge.h
        CMakeLists.txt
        LICENSE
        main.cpp
        README.md)
