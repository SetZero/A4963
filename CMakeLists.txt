cmake_minimum_required(VERSION 3.12)
project(SPI)

set(CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

include_directories(.)
include_directories(inc)
include_directories(src)

## FIND LibUSB ##
find_library(LIBUSB_LIBRARY
        NAMES usb-1.0)
find_path(LIBUSB_INCLUDE_DIR
        NAMES libusb.h
        PATH_SUFFIXES "include" "libusb" "libusb-1.0")

if(NOT LIBUSB_LIBRARY AND LIBUSB_INCLUDE_DIR)
    message(FATAL_ERROR  "LibUSB Libary not found!")
endif(NOT LIBUSB_LIBRARY AND LIBUSB_INCLUDE_DIR)


add_executable(test_cases tests/Test_src_CustomDataTypes_Volt.cpp)
add_executable(hid main.cpp)


## Include Directories

target_link_libraries(hid ${LIBUSB_LIBRARY} src spi usb inc cdt utils scales)
target_link_libraries(test_cases ${LIBUSB_LIBRARY} src spi usb inc cdt utils scales)
include_directories(SYSTEM ${LIBUSB_INCLUDE_DIR})

include_directories(inc)
add_subdirectory(inc)
include_directories(src)
add_subdirectory(src)
include_directories(src/SPI)
add_subdirectory(src/SPI)
include_directories(src/USB)
add_subdirectory(src/USB)
include_directories(src/CustomDataTypes)
add_subdirectory(src/CustomDataTypes)
include_directories(src/utils)
add_subdirectory(src/utils)
include_directories(src/utils/scales)
add_subdirectory(src/utils/scales)
#add_executable(SPI
#        inc/mcp2210_api.c
#        inc/mcp2210_api.h
#        src/25LC256.cpp
#        src/25LC256.h
#        src/ATmega32U4SPI.cpp
#        src/ATmega32U4SPI.h
#        src/mcp2210_hal.cpp
#        src/mcp2210_hal.h
#        src/SPIBridge.h
#        CMakeLists.txt
#        LICENSE
#        main.cpp
#        README.md)
