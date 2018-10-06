SET(LIBNAME spi)

file(GLOB SOURCE_FILES *.cpp)
file(GLOB HEADER_FILES *.h)

add_library(${LIBNAME} STATIC ${SOURCE_FILES} ${HEADER_FILES} spidata.h spi8.cpp spi8.h spi16.cpp spi16.h)