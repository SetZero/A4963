SET(LIBNAME cdt)

file(GLOB SOURCE_FILES *.cpp)
file(GLOB HEADER_FILES *.h)

if (UNIX)
    add_library(${LIBNAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})
elseif (WIN32)
    add_library(${LIBNAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})
endif (UNIX)