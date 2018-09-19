SET(LIBNAME utils)

file(GLOB SOURCE_FILES *.cpp)
file(GLOB HEADER_FILES *.h)

add_library(${LIBNAME} STATIC ${SOURCE_FILES} ${HEADER_FILES} utils.cpp ScaleOptional.cpp ScaleOptional.h)