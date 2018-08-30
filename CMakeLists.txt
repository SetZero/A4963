find_package(PkgConfig)
cmake_minimum_required(VERSION 3.12)
project(hid)

set(CMAKE_CXX_STANDARD 17)

## FIND LibUSB ##
find_library(LIBUSB_LIBRARY
        NAMES usb-1.0)
find_path(LIBUSB_INCLUDE_DIR
        NAMES libusb.h
        PATH_SUFFIXES "include" "libusb" "libusb-1.0")

add_executable(hid main.cpp)

## Include Directories

target_link_libraries(hid ${LIBUSB_LIBRARY} src inc)
include_directories(SYSTEM ${LIBUSB_INCLUDE_DIR})
include_directories(inc)
add_subdirectory(inc)
include_directories(src)
add_subdirectory(src)
#message(FATAL_ERROR "PATH: ${LIBUSB_INCLUDE_DIR}")

