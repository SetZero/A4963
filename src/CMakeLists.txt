SET(LIBNAME src)

file(GLOB SOURCE_FILES *.cpp)
file(GLOB HEADER_FILES *.h)

add_library(${LIBNAME} STATIC ${SOURCE_FILES} ${HEADER_FILES} LibUSBDevices.cpp LibUSBDevices.h LibUsbDevice.cpp LibUsbDevice.h)