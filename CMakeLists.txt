cmake_minimum_required(VERSION 3.17)
project(ckws)

set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILE main.cpp Kws.cpp Kws.h logging.h wav_data.h wav_writer.cpp wav_writer.h)

#add_executable(ckws ${SOURCE_FILE})
add_library(ckws SHARED ${SOURCE_FILE})

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG")
endif ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")

target_include_directories(ckws PRIVATE includes/tensorflow-lite)

if ("${BUILD_ENV}" STREQUAL "osx_x86_64")
    target_link_directories(ckws PUBLIC libs/osx_x86_64)
elseif ("${BUILD_ENV}" STREQUAL "rpi_armv7l")
    target_link_directories(ckws PUBLIC libs/rpi_armv7l)
elseif ("${BUILD_ENV}" STREQUAL "arm64-v8a")
    target_link_directories(ckws PUBLIC libs/arm64-v8a)
endif ("${BUILD_ENV}" STREQUAL "osx_x86_64")

#target_link_libraries(ckws tensorflow-lite pthread m dl)
target_link_libraries(ckws tensorflow-lite m dl)