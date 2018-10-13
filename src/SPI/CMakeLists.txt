SET(LIBNAME spi)

if (UNIX)
    file(GLOB SOURCE_FILES *.cpp)
    file(GLOB HEADER_FILES *.h)

    add_library(${LIBNAME} SHARED ${SOURCE_FILES} ${HEADER_FILES} )
elseif (WIN32)
    add_library(${LIBNAME} STATIC ATmega32U4SPI.cpp ATmega32U4SPI.h GPIOBridge.h SPIBridge.cpp SPIBridge.h SPIData.h SPIDevice.h SPIDevice.cpp )
endif (UNIX)