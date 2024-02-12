# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

cmake_minimum_required (VERSION 3.20)

project (DirectXMeshTest
  DESCRIPTION "DirectXMesh Test Suite"
  HOMEPAGE_URL "https://github.com/walbourn/directxmeshtest/wiki"
  LANGUAGES CXX)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
  message(FATAL_ERROR "DirectXMesh Test Suite should be built by the main CMakeLists")
endif()

if(WIN32 AND (NOT DEFINED ENV{DIRECTXMESH_MEDIA_PATH}))
  message(WARNING "Tests make use of DIRECTXMESH_MEDIA_PATH environment variable to find media")
endif()

set(TEST_EXES headertest utilitiestest)
add_executable(headertest headertest/main.cpp)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/utilitiestest)

# MESH
list(APPEND TEST_EXES xtmesh)
add_executable(xtmesh
   mesh/adjacency.cpp
   mesh/clean.cpp
   mesh/concat.cpp
   mesh/meshlets.cpp
   mesh/normals.cpp
   mesh/optimize.cpp
   mesh/remap.cpp
   mesh/tangentframe.cpp
   mesh/utils.cpp
   mesh/valid.cpp
   mesh/weld.cpp
   mesh/directxtest.cpp)
target_include_directories(xtmesh PRIVATE ./common)
add_test(NAME "mesh" COMMAND xtmesh)
set_tests_properties(mesh PROPERTIES TIMEOUT 60)

# VB
list(APPEND TEST_EXES xtvb)
set(TEST_SOURCES vb/inputdesc.cpp vb/vb.cpp)
if(BUILD_DX12 OR (${DIRECTX_ARCH} MATCHES "^arm64"))
   set(TEST_SOURCES ${TEST_SOURCES} vb/inputdesc12.cpp vb/vb12.cpp)
endif()
add_executable(xtvb ${TEST_SOURCES}
   vb/testutils.cpp vb/directxtest.cpp)
target_include_directories(xtvb PRIVATE ./common)
add_test(NAME "vertexBuffer" COMMAND xtvb)
set_tests_properties(vertexBuffer PROPERTIES TIMEOUT 60)

# CP
list(APPEND TEST_EXES xtcp)
add_executable(xtcp cp/process.cpp cp/directxtest.cpp)
target_include_directories(xtcp PRIVATE ./common)
add_test(NAME "contentProcess" COMMAND xtcp)
set_tests_properties(contentProcess PROPERTIES TIMEOUT 60)

message("INFO: Enabled tests: ${TEST_EXES}")
foreach(t IN LISTS TEST_EXES)
  target_include_directories(${t} PRIVATE ../Utilities)
  target_link_libraries(${t} PRIVATE DirectXMesh)
endforeach()

if(directxmath_FOUND)
    foreach(t IN LISTS TEST_EXES)
      target_link_libraries(${t} PRIVATE Microsoft::DirectXMath)
    endforeach()
endif()

if(directx-headers_FOUND)
    foreach(t IN LISTS TEST_EXES)
      target_link_libraries(${t} PRIVATE Microsoft::DirectX-Headers)
      target_compile_definitions(${t} PRIVATE USING_DIRECTX_HEADERS)
    endforeach()
endif()

if(MSVC)
    foreach(t IN LISTS TEST_EXES)
      target_compile_options(${t} PRIVATE "$<IF:$<STREQUAL:${t},headertest>,/Wall,/W4>" /GR-)
    endforeach()
endif()

if(DEFINED COMPILER_DEFINES)
    foreach(t IN LISTS TEST_EXES)
      target_compile_definitions(${t} PRIVATE ${COMPILER_DEFINES})
      target_compile_options(${t} PRIVATE ${COMPILER_SWITCHES})
      target_link_options(${t} PRIVATE ${LINKER_SWITCHES})
    endforeach()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|IntelLLVM")
    target_compile_options(headertest PRIVATE -Wall -Wpedantic -Wextra)
    foreach(t IN LISTS TEST_EXES)
      target_compile_options(${t} PRIVATE "-Wno-c++98-compat" "-Wno-c++98-compat-pedantic" "-Wno-language-extension-token")
      if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 16.0)
        target_compile_options(${t} PRIVATE "-Wno-unsafe-buffer-usage")
      endif()
    endforeach()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    foreach(t IN LISTS TEST_EXES)
      target_compile_options(${t} PRIVATE "-Wno-ignored-attributes" "-Walloc-size-larger-than=4GB")
    endforeach()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    foreach(t IN LISTS TEST_EXES)
      set_target_properties(${t} PROPERTIES CXX_STANDARD 14)
    endforeach()
endif()

if(WIN32)
    foreach(t IN LISTS TEST_EXES)
      target_compile_definitions(${t} PRIVATE _WIN32_WINNT=${WINVER})
    endforeach()

    if(DISABLE_MSVC_ITERATOR_DEBUGGING)
      foreach(t IN LISTS TEST_EXES)
        target_compile_definitions(${t} PRIVATE _ITERATOR_DEBUG_LEVEL=0)
      endforeach()
    endif()
endif()
