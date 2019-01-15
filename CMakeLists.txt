cmake_minimum_required(VERSION 3.12)
project(SPI)

set(CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -ludev -O3 -Da4963_debug")

include_directories(.)
include_directories(inc)
include_directories(src)

add_executable(test_cases tests/Test_SPIData.cpp tests/Test_utils_getFirstSetBitPos.cpp
                tests/Test_src_scales_RangedUnit.cpp tests/UnitScale.cpp tests/Test_src_CustomDataTypes.cpp tests/Test_Percentage.cpp)
add_executable(hid main.cpp)


## FIND LibUSB ##
find_library(LIBUSB_LIBRARY
        NAMES usb-1.0)

find_library(UDEV
        NAMES libudev.so)

find_path(LIBUSB_INCLUDE_DIR
        NAMES libusb.h
        PATH_SUFFIXES "include" "libusb" "libusb-1.0")

if(NOT LIBUSB_LIBRARY AND LIBUSB_INCLUDE_DIR)
    message(FATAL_ERROR  "LibUSB Libary not found!")
endif(NOT LIBUSB_LIBRARY AND LIBUSB_INCLUDE_DIR)

#find udev
FIND_PATH(
        UDEV_INCLUDE_DIR
        libudev.h
        /usr/include
        /usr/local/include
        ${UDEV_PATH_INCLUDES}
)

FIND_LIBRARY(
        UDEV_LIBRARIES
        NAMES udev libudev
        PATHS ${ADDITIONAL_LIBRARY_PATHS}
        ${UDEV_PATH_LIB}
)


## Include Directories

include_directories(SYSTEM ${UDEV_INCLUDE_DIR})
include_directories(SYSTEM ${LIBUSB_INCLUDE_DIR})
add_subdirectory(inc)
add_subdirectory(src)
include_directories(src/SPI)
add_subdirectory(src/SPI)
include_directories(src/USB)
add_subdirectory(src/USB)
include_directories(src/utils)
add_subdirectory(src/utils)
include_directories(src/utils/scales)
add_subdirectory(src/utils/scales)
include_directories(src/Devices/A4963)
add_subdirectory(src/Devices/A4963)
include_directories(src/CustomDataTypes)
add_subdirectory(src/CustomDataTypes)

# LIBEVPP
target_link_libraries(hid     src spi usbb inc  utils scales a4963 cdt  ${UDEV} ${LIBUSB_LIBRARY})

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