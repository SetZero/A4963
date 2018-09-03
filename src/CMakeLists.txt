SET(LIBNAME src)

file(GLOB SOURCE_FILES *.cpp)
file(GLOB HEADER_FILES *.h)

add_library(${LIBNAME} STATIC ${SOURCE_FILES} ${HEADER_FILES} USB/LibUSBDevices.cpp USB/LibUSBDevices.h USB/LibUsbDevice.cpp USB/LibUsbDevice.h USB/HIDevice.cpp USB/HIDevice.h USB/DevicesList.h)