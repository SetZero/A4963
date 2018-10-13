cmake_minimum_required(VERSION 3.11)
project(SPI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
if (UNIX)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -ludev")
elseif (WIN32)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif (UNIX)

include_directories(.)
include_directories(inc)
include_directories(src)

add_executable(test_cases tests/Test_SPIData.cpp tests/Test_src_CustomDataTypes_Volt.cpp tests/Test_utils_getFirstSetBitPos.cpp tests/Test_src_scales_RangedUnit.cpp)
add_executable(hid main.cpp)


## FIND LibUSB ##

if(WIN32)
    set(LIBUSB_USE_VERSION "1.0.20")
    set(LIBUSB_ARCHIVE "libusb-${LIBUSB_USE_VERSION}.7z")
    set(LIBUSB_DOWNLOAD_URL "https://sourceforge.net/projects/libusb/files/libusb-1.0/libusb-${LIBUSB_USE_VERSION}/${LIBUSB_ARCHIVE}/download")
    #set(ZIP_LOCATION, "C:\\Program Files\\7-Zip\\7z.exe")

    #file(DOWNLOAD
    #        ${LIBUSB_DOWNLOAD_URL}
    #        ${CMAKE_BINARY_DIR}/libusb-${LIBUSB_USE_VERSION}.7z
    #        SHOW_PROGRESS
    #        )

    #execute_process(COMMAND "C:\\Program Files\\7-Zip\\7z.exe" x -y libusb-${LIBUSB_USE_VERSION}.7z -olibusb-${LIBUSB_USE_VERSION}
    #        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    #        )
endif()

if (UNIX)
    find_library(LIBUSB_LIBRARY
            NAMES usb-1.0)
    find_path(LIBUSB_INCLUDE_DIR
            NAMES libusb.h
            PATH_SUFFIXES "include" "libusb" "libusb-1.0")

    if(NOT LIBUSB_LIBRARY AND LIBUSB_INCLUDE_DIR)
        message(FATAL_ERROR  "LibUSB Libary not found!")
    endif(NOT LIBUSB_LIBRARY AND LIBUSB_INCLUDE_DIR)
elseif (WIN32)
    FIND_PATH(LIBUSB_INCLUDE_DIR NAMES libusb.h
            HINTS
            #"${CMAKE_BINARY_DIR}/libusb-${LIBUSB_USE_VERSION}/include"
			"${CMAKE_BINARY_DIR}\\libusb-1.0.20\\include"
            PATH_SUFFIXES libusb-1.0
    )
    set(LIBUSB_NAME libusb-1.0.lib)
    FIND_LIBRARY(LIBUSB_LIBRARY NAMES ${LIBUSB_NAME}
        HINTS
        "${CMAKE_BINARY_DIR}\\libusb-1.0.20\\MS64\\static"
    )
    INCLUDE(FindPackageHandleStandardArgs)
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(Libusb DEFAULT_MSG LIBUSB_LIBRARY LIBUSB_INCLUDE_DIR)

    MARK_AS_ADVANCED(LIBUSB_INCLUDE_DIR LIBUSB_LIBRARY)
endif (UNIX)

if (WIN32)
	set(LEGACY_API_NAME legacy_stdio_definitions.lib)
	FIND_LIBRARY(LEGACY_API NAMES ${LEGACY_API_NAME})
	if(NOT LEGACY_API)
		FATAL_ERROR("NO LEGACY_API FOUND!")
	endif(NOT LEGACY_API)
endif (WIN32)

#find udev
if (UNIX)
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

    IF (UDEV_LIBRARIES AND UDEV_INCLUDE_DIR)
        SET(UDEV_FOUND "YES")
        execute_process(COMMAND pkg-config --atleast-version=143 libudev RESULT_VARIABLE UDEV_STABLE)
        # retvale is 0 of the condition is "true" so we need to negate the value...
        if (UDEV_STABLE)
            set(UDEV_STABLE 0)
        else (UDEV_STABLE)
            set(UDEV_STABLE 1)
        endif (UDEV_STABLE)
        MESSAGE("udev linked")
        message(STATUS "libudev stable: ${UDEV_STABLE}")
    ENDIF (UDEV_LIBRARIES AND UDEV_INCLUDE_DIR)

    IF (UDEV_FOUND)
        MESSAGE(STATUS "Found UDev: ${UDEV_LIBRARIES}")
        MESSAGE(STATUS "   include: ${UDEV_INCLUDE_DIR}")
    ELSE (UDEV_FOUND)
        MESSAGE(STATUS "UDev not found.")
        MESSAGE(STATUS "UDev: You can specify includes: -DUDEV_PATH_INCLUDES=/opt/udev/include")
        MESSAGE(STATUS "      currently found includes: ${UDEV_INCLUDE_DIR}")
        MESSAGE(STATUS "UDev: You can specify libs: -DUDEV_PATH_LIB=/opt/udev/lib")
        MESSAGE(STATUS "      currently found libs: ${UDEV_LIBRARIES}")
        IF (UDev_FIND_REQUIRED)
            MESSAGE(FATAL_ERROR "Could not find UDev library")
        ENDIF (UDev_FIND_REQUIRED)
    ENDIF (UDEV_FOUND)
endif (UNIX)

## Include Directories
if (UNIX)
    include_directories(SYSTEM ${UDEV_INCLUDE_DIR})
    include_directories(SYSTEM ${LIBUSB_INCLUDE_DIR})
elseif (WIN32)
    include_directories(${LIBUSB_INCLUDE_DIR})
endif (UNIX)
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

if (UNIX)
    target_link_libraries(hid     src spi usbb inc  utils scales a4963 cdt  ${LIBUSB_LIBRARY})
elseif (WIN32)
    target_link_libraries(hid     src spi usbb  utils scales a4963 cdt ${LEGACY_API}  ${LIBUSB_LIBRARY})
endif (UNIX)

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