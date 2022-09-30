cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if( CMAKE_BUILD_TYPE EQUAL Release )
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -03")
endif()

project(task3-hash)

include_directories(includes)

file(GLOB SOURCES src/*.cpp)

add_executable(task3-hash ${SOURCES})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lcrypto")

