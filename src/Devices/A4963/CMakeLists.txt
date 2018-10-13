SET(LIBNAME a4963)

file(GLOB SOURCE_FILES *.cpp)
file(GLOB HEADER_FILES *.h)

if (UNIX)
    add_library(${LIBNAME} SHARED ${SOURCE_FILES} ${HEADER_FILES} A4963RegisterInfo.cpp)
elseif (WIN32)
    add_library(${LIBNAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})
endif(UNIX)