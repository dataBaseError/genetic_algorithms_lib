cmake_minimum_required(VERSION 2.8.4)
project(GALibrary)

find_package (Threads)

set (GALibrary_VERSION_MAJOR 0)
set (GALibrary_VERSION_MINOR 1)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    src/main.cpp
    src/Generation.cpp
    src/RouletteWheel.cpp
)

set(HEADER_DIR inc)

include_directories(${HEADER_DIR})

set(HEADER_FILES
    ${HEADER_DIR}/Chromosome.hpp
    ${HEADER_DIR}/Manager.hpp
    ${HEADER_DIR}/RouletteWheel.hpp
    ${HEADER_DIR}/SafeQueue.hpp
    ${HEADER_DIR}/Selection.hpp
)

add_executable(GALibrary ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries (GALibrary ${CMAKE_THREAD_LIBS_INIT})