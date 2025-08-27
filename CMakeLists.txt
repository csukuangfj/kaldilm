# Copyright (c)  2020  Xiaomi Corporation (author: Fangjun Kuang)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "4.0.0")
  set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
endif()
cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(kaldilm)

# also change scripts/conda/kaldilm/meta.yml
set(kaldilm_VERSION "1.15.3")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(BUILD_RPATH_USE_ORIGIN TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
if(NOT APPLE)
  set(CMAKE_INSTALL_RPATH "$ORIGIN")
  set(CMAKE_BUILD_RPATH "$ORIGIN")
else()
  set(CMAKE_INSTALL_RPATH "@loader_path")
  set(CMAKE_BUILD_RPATH "@loader_path")
endif()

if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "No CMAKE_BUILD_TYPE given, default to Release")
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17 CACHE STRING "The C++ version to be used.")
set(CMAKE_CXX_EXTENSIONS OFF)

set(BUILD_SHARED_LIBS ON)
if(WIN32)
  message(STATUS "Set BUILD_SHARED_LIBS to OFF for Windows")
  set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
endif()

message(STATUS "C++ Standard version: ${CMAKE_CXX_STANDARD}")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(pybind11)
include(openfst)

include_directories(${CMAKE_SOURCE_DIR})
add_subdirectory(kaldilm)
