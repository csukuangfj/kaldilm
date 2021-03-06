# Copyright (c)  2020  Xiaomi Corporation (author: Fangjun Kuang)

cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(kaldilm)

# also change scripts/conda/kaldilm/meta.yml
set(kaldilm_VERSION "1.8")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(BUILD_RPATH_USE_ORIGIN TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH "$ORIGIN")
set(CMAKE_BUILD_RPATH "$ORIGIN")

if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "No CMAKE_BUILD_TYPE given, default to Release")
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 11 CACHE STRING "The C++ version to be used.")
set(CMAKE_CXX_EXTENSIONS OFF)

message(STATUS "C++ Standard version: ${CMAKE_CXX_STANDARD}")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(pybind11)
include(openfst)

include_directories(${CMAKE_SOURCE_DIR})
add_subdirectory(kaldilm)
