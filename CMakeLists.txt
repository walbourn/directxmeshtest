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

set(TEST_EXES headertest utilitiestest)
add_executable(headertest headertest/main.cpp)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/utilitiestest)

if(WIN32 AND (NOT DEFINED ENV{DIRECTXMESH_MEDIA_PATH}))
  message(WARNING "Tests make use of DIRECTXMESH_MEDIA_PATH environment variable to find media")
endif()

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
    target_compile_options(${t} PRIVATE "$<IF:$<STREQUAL:${t},headertest>,/Wall,/W4>" /GR- /fp:fast "$<$<NOT:$<CONFIG:DEBUG>>:/guard:cf>")
    target_link_options(${t} PRIVATE /DYNAMICBASE /NXCOMPAT /INCREMENTAL:NO)
  endforeach()

  if((CMAKE_SIZEOF_VOID_P EQUAL 4) AND (NOT (${DIRECTX_ARCH} MATCHES "^arm")))
    foreach(t IN LISTS TEST_EXES)
      target_link_options(${t} PRIVATE /SAFESEH)
    endforeach()
  endif()

  if((MSVC_VERSION GREATER_EQUAL 1924)
     AND ((NOT (CMAKE_CXX_COMPILER_ID MATCHES "Clang")) OR (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 16.0)))
    foreach(t IN LISTS TEST_EXES)
      target_compile_options(${t} PRIVATE /ZH:SHA_256)
    endforeach()
  endif()

  if((MSVC_VERSION GREATER_EQUAL 1928)
    AND (CMAKE_SIZEOF_VOID_P EQUAL 8)
    AND NOT ENABLE_OPENEXR_SUPPORT
    AND ((NOT (CMAKE_CXX_COMPILER_ID MATCHES "Clang")) OR (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 13.0)))
      foreach(t IN LISTS TEST_EXES)
        target_compile_options(${t} PRIVATE "$<$<NOT:$<CONFIG:DEBUG>>:/guard:ehcont>")
        target_link_options(${t} PRIVATE "$<$<NOT:$<CONFIG:DEBUG>>:/guard:ehcont>")
      endforeach()
    endif()
else()
    foreach(t IN LISTS TEST_EXES)
      target_compile_definitions(${t} PRIVATE $<IF:$<CONFIG:DEBUG>,_DEBUG,NDEBUG>)
    endforeach()
endif()

if(NOT (${DIRECTX_ARCH} MATCHES "^arm"))
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(ARCH_SSE2 $<$<CXX_COMPILER_ID:MSVC>:/arch:SSE2> $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-msse2>)
    else()
        set(ARCH_SSE2 $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-msse2>)
    endif()

    foreach(t IN LISTS TEST_EXES)
      target_compile_options(${t} PRIVATE ${ARCH_SSE2})
    endforeach()
endif()

if( CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
    target_compile_options(headertest PRIVATE -Wall -Wpedantic -Wextra)
    foreach(t IN LISTS TEST_EXES)
      target_compile_options(${t} PRIVATE "-Wno-c++98-compat" "-Wno-c++98-compat-pedantic" "-Wno-language-extension-token")
      if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 16.0)
        target_compile_options(${t} PRIVATE "-Wno-unsafe-buffer-usage")
      endif()
    endforeach()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    foreach(t IN LISTS TEST_EXES)
      target_compile_options(${t} PRIVATE -Wno-ignored-attributes)
    endforeach()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    foreach(t IN LISTS TEST_EXES)
      target_compile_options(${t} PRIVATE /sdl /permissive- /JMC- /Zc:__cplusplus /Zc:inline)
    endforeach()

    if(CMAKE_INTERPROCEDURAL_OPTIMIZATION)
      foreach(t IN LISTS TEST_EXES)
        target_compile_options(${t} PRIVATE /Gy /Gw)
      endforeach()
    endif()

    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.26)
      foreach(t IN LISTS TEST_EXES)
        target_compile_options(${t} PRIVATE /Zc:preprocessor /wd5105)
      endforeach()
    endif()

    if((CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.27) AND (NOT ((${DIRECTX_ARCH} MATCHES "^arm"))))
      foreach(t IN LISTS TEST_EXES)
        target_link_options(${t} PRIVATE /CETCOMPAT)
      endforeach()
    endif()

    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.28)
      foreach(t IN LISTS TEST_EXES)
        target_compile_options(${t} PRIVATE /Zc:lambda)
      endforeach()
    endif()

    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.34)
      foreach(t IN LISTS TEST_EXES)
        target_compile_options(${t} PRIVATE /wd5262 /wd5264)
      endforeach()
    endif()

    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.35)
      foreach(t IN LISTS TEST_EXES)
        target_compile_options(${t} PRIVATE /Zc:checkGwOdr $<$<VERSION_GREATER_EQUAL:${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION},10.0.22000>:/Zc:templateScope>)
      endforeach()
    endif()
endif()

if(WIN32)
    foreach(t IN LISTS TEST_EXES)
      target_compile_definitions(${t} PRIVATE _UNICODE UNICODE _WIN32_WINNT=${WINVER})
    endforeach()

    if(DISABLE_MSVC_ITERATOR_DEBUGGING)
      foreach(t IN LISTS TEST_EXES)
        target_compile_definitions(${t} PRIVATE _ITERATOR_DEBUG_LEVEL=0)
      endforeach()
    endif()
endif()
