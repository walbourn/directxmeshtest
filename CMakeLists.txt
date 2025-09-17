# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

cmake_minimum_required (VERSION 3.21)

project (DirectXMeshTest
  DESCRIPTION "DirectXMesh Test Suite"
  HOMEPAGE_URL "https://github.com/walbourn/directxmeshtest/wiki"
  LANGUAGES CXX)

option(BUILD_BVT "Build-verification test" OFF)

if(PROJECT_IS_TOP_LEVEL)
  message(FATAL_ERROR "DirectXMesh Test Suite should be built by the main CMakeLists")
endif()

if(WIN32 AND (NOT DEFINED ENV{DIRECTXMESH_MEDIA_PATH}) AND (NOT BUILD_BVT))
  message(WARNING "Tests make use of DIRECTXMESH_MEDIA_PATH environment variable to find media")
endif()

if(BUILD_BVT)
  message(STATUS "Building for Build Verification Tests (BVTs)")
  set(BVT_MEDIA_PATH "${CMAKE_CURRENT_LIST_DIR}/media")
  cmake_path(NATIVE_PATH BVT_MEDIA_PATH BVT_MEDIA_PATH)
endif()

set(TEST_EXES headertest utilitiestest)
add_executable(headertest headertest/main.cpp)

# UTILITIES
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/utilitiestest)
list(APPEND TEST_EXES utilitiestest)
add_test(NAME "utilsTest" COMMAND utilitiestest)
set_tests_properties(utilsTest PROPERTIES TIMEOUT 120)
if(BUILD_BVT)
  set_tests_properties(utilsTest PROPERTIES ENVIRONMENT "DIRECTXMESH_MEDIA_PATH=${BVT_MEDIA_PATH}")
endif()

# fuzzloaders
list(APPEND TEST_EXES fuzzloaders)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/fuzzloaders)

# MESH
list(APPEND TEST_EXES xtmesh)
add_executable(xtmesh
   mesh/adjacency.cpp
   mesh/clean.cpp
   mesh/concat.cpp
   mesh/meshlets.cpp
   mesh/misc.cpp
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
if(BUILD_BVT)
  set_tests_properties(vertexBuffer PROPERTIES ENVIRONMENT "DIRECTXMESH_MEDIA_PATH=${BVT_MEDIA_PATH}")
endif()

# CP
list(APPEND TEST_EXES xtcp)
add_executable(xtcp cp/process.cpp cp/directxtest.cpp)
target_include_directories(xtcp PRIVATE ./common)
add_test(NAME "contentProcess" COMMAND xtcp)
set_tests_properties(contentProcess PROPERTIES TIMEOUT 60)
if(BUILD_BVT)
  set_tests_properties(contentProcess PROPERTIES ENVIRONMENT "DIRECTXMESH_MEDIA_PATH=${BVT_MEDIA_PATH}")
endif()

message(STATUS "Enabled tests: ${TEST_EXES}")
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

if(BUILD_BVT)
    foreach(t IN LISTS TEST_EXES)
      target_compile_definitions(${t} PRIVATE -DBUILD_BVT_ONLY)
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

      if(BUILD_SHARED_LIBS)
        target_compile_options(${t} PRIVATE "-Wno-attributes")
      endif()
    endforeach()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    foreach(t IN LISTS TEST_EXES)
      set_target_properties(${t} PROPERTIES CXX_STANDARD 14)
    endforeach()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    if(ENABLE_CODE_ANALYSIS)
      foreach(t IN LISTS TEST_EXES)
        target_compile_options(${t} PRIVATE /analyze /WX)
      endforeach()
    endif()
    if(BUILD_FUZZING
       AND (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.32))
        foreach(t IN LISTS TEST_EXES)
          target_compile_options(${t} PRIVATE ${ASAN_SWITCHES})
          target_link_libraries(${t} PRIVATE ${ASAN_LIBS})
        endforeach()
    endif()
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

if(ENABLE_CODE_COVERAGE)
  foreach(t IN LISTS TEST_EXES)
    if(DEFINED COV_COMPILER_SWITCHES)
      target_compile_options(${t} PRIVATE ${COV_COMPILER_SWITCHES})
    endif()
    if(DEFINED COV_LINKER_SWITCHES)
      target_link_options(${t} PRIVATE ${COV_LINKER_SWITCHES})
    endif()
    if(DEFINED COV_LIBS)
      target_link_libraries(${t} PRIVATE ${COV_LIBS})
    endif()
  endforeach()

  if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    configure_file(${CMAKE_CURRENT_LIST_DIR}/codecov/MSVCCodeCov.cmd.in ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MSVCCodeCov.cmd COPYONLY)
    configure_file(${CMAKE_CURRENT_LIST_DIR}/codecov/coverage.config.in ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/coverage.config COPYONLY)
  endif()
endif()
