# DirectXMesh Utilities Module Test
#
# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.
#
# http://go.microsoft.com/fwlink/?LinkId=248926

cmake_minimum_required (VERSION 3.11)

project (utilitiestest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/CMake")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/CMake")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/CMake")

if(MSVC)
    # Use max Warning Level
    string(REPLACE "/W3 " "/Wall " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    string(REPLACE "/W3 " "/Wall " CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
    string(REPLACE "/W3 " "/Wall " CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
endif()

set(TEST_SOURCES
    ../../Utilities/FlexibleVertexFormat.h
    ../../Utilities/WaveFrontReader.h)

add_executable(${PROJECT_NAME} main.cpp fvf.cpp wavefrontobj.cpp ${TEST_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
    ../../Utilities)

if(MSVC)
    if (${CMAKE_SIZEOF_VOID_P} EQUAL "4")
        target_compile_options(${PROJECT_NAME} PRIVATE /arch:SSE2)
    endif()
endif()

if (NOT WIN32)
    find_package(directx-headers CONFIG REQUIRED)
    find_package(directxmath CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE Microsoft::DirectX-Headers Microsoft::DirectXMath)
endif()

if ( CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
    set(WarningsEXE "-Wpedantic" "-Wextra" "-Wno-c++98-compat" "-Wno-c++98-compat-pedantic" "-Wno-float-equal" "-Wno-global-constructors" "-Wno-language-extension-token" "-Wno-missing-prototypes" "-Wno-missing-variable-declarations" "-Wno-reserved-id-macro")
    target_compile_options(${PROJECT_NAME} PRIVATE ${WarningsEXE})
endif()
if ( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
    target_compile_options(${PROJECT_NAME} PRIVATE /permissive- /Zc:__cplusplus)

    set(WarningsEXE "/wd4061" "/wd4365" "/wd4668" "/wd4820" "/wd5031" "/wd5032" "/wd5039" "/wd5045" )
    target_compile_options(${PROJECT_NAME} PRIVATE ${WarningsEXE})
endif()

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _UNICODE UNICODE _WIN32_WINNT=0x0601)
endif()
