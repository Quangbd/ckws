cmake_minimum_required(VERSION 3.17)
project(ckws)

set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILE main.cpp Kws.cpp Kws.h logging.h)

add_executable(ckws ${SOURCE_FILE})
#add_library(ckws SHARED ${SOURCE_FILE})

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG")
endif ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")

target_include_directories(ckws PRIVATE includes/tensorflow-lite)

find_library(TENSORFLOW tensorflow-lite libs/osx_x86_64)
target_link_libraries(ckws ${TENSORFLOW})