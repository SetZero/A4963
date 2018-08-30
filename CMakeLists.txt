cmake_minimum_required(VERSION 3.12)
project(hid)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

include_directories(inc)
link_directories(inc)
add_subdirectory(inc)

include_directories(src)
link_directories(src)
add_subdirectory(src)

add_executable(hid
        main.cpp)
target_link_libraries(hid src)
target_link_libraries(hid inc)