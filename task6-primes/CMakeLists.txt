cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if( CMAKE_BUILD_TYPE EQUAL Release )
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -03")
endif()

include_guard(GLOBAL)
project(task6-primes.cpp)

include_directories(includes)

file(GLOB SOURCES src/*.cpp)

add_executable(task6-primes.cpp ${SOURCES})
target_link_libraries(task6-primes.cpp ${Boost_LIBRARIES})
