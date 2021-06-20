cmake_minimum_required(VERSION 3.17)
project(ckws)

set(CMAKE_CXX_STANDARD 11)
file(GLOB SOURCE_FILE "src/*.h" "src/*.cpp")
message(${SOURCE_FILE})

add_library(ckws SHARED ${SOURCE_FILE})

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG")
endif ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")

target_include_directories(ckws PRIVATE includes)

if ("${BUILD_ENV}" STREQUAL "osx_x86_64")
    target_link_directories(ckws PUBLIC libs/osx_x86_64)
elseif ("${BUILD_ENV}" STREQUAL "rpi_armv7l")
    target_link_directories(ckws PUBLIC libs/rpi_armv7l)
elseif ("${BUILD_ENV}" STREQUAL "arm64-v8a")
    target_link_directories(ckws PUBLIC libs/arm64-v8a)
else()
    target_link_directories(ckws PUBLIC libs/osx_arm64)
endif ("${BUILD_ENV}" STREQUAL "osx_x86_64")

if ("${BUILD_ENV}" STREQUAL "arm64-v8a")
    target_link_libraries(ckws log android tensorflowlite_jni m dl)
else()
    target_link_libraries(ckws tensorflowlite_c pthread m dl)
endif ("${BUILD_ENV}" STREQUAL "arm64-v8a")

add_executable(test ${SOURCE_FILE})
target_include_directories(test PRIVATE includes)
target_link_libraries(test ckws)