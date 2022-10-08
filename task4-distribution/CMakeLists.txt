cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if( CMAKE_BUILD_TYPE EQUAL Release )
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -03")
endif()

project(task4-distribution)

include_directories(includes)

file(GLOB SOURCES src/*.cpp)

set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost COMPONENTS random REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(task4-distribution ${SOURCES})
target_link_libraries(task4-distribution ${Boost_LIBRARIES})
