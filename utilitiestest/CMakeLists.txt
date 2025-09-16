# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

cmake_minimum_required (VERSION 3.21)

project (utilitiestest
  DESCRIPTION "DirectXMesh Utilities Module Test"
  HOMEPAGE_URL "https://github.com/walbourn/directxmeshtest/wiki"
  LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL)
  message(FATAL_ERROR "DirectXMesh Test Suite should be built by the main CMakeLists")
endif()

set(TEST_SOURCES
    wavefrontobj.cpp
    ../../Utilities/WaveFrontReader.h)

if(NOT WINDOWS_STORE)
  set(TEST_SOURCES ${TEST_SOURCES}
      fvf.cpp
      fvf11.cpp
      fvf12.cpp
      ../../Utilities/FlexibleVertexFormat.h)
endif()

add_executable(${PROJECT_NAME} main.cpp ${TEST_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
    ../../Utilities)

target_include_directories(${PROJECT_NAME} PRIVATE
    ../common)
