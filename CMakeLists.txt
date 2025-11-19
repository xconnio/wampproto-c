cmake_minimum_required(VERSION 3.18)
project(wampproto C)

# -------------------------
# Project Settings
# -------------------------
set(CMAKE_C_STANDARD 11)
include(GNUInstallDirs)

# -------------------------
# Formatting & Linting
# -------------------------

# Clang-format
if(TARGET format OR CMAKE_BUILD_TYPE STREQUAL "Format")
  find_program(CLANG_FORMAT_EXE NAMES clang-format)
  if(CLANG_FORMAT_EXE)
    message(STATUS "Found clang-format: ${CLANG_FORMAT_EXE}")

    file(GLOB_RECURSE ALL_SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/*.c
         ${CMAKE_SOURCE_DIR}/src/*.h ${CMAKE_SOURCE_DIR}/tests/*.c
         ${CMAKE_SOURCE_DIR}/tests/*.h)

    add_custom_target(
      format
      COMMAND ${CLANG_FORMAT_EXE} -i -style=file ${ALL_SOURCE_FILES}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMENT "Running clang-format on source files")
  endif()
endif()

# Clang-tidy
if(TARGET lint OR CMAKE_BUILD_TYPE STREQUAL "Lint")
  find_program(CLANG_TIDY_EXE NAMES clang-tidy)
  if(CLANG_TIDY_EXE)
    message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXE}")

    set(CMAKE_C_CLANG_TIDY
        "${CLANG_TIDY_EXE};-checks=*,-clang-analyzer-alpha.*")

    file(GLOB_RECURSE ALL_SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/*.c
         ${CMAKE_SOURCE_DIR}/tests/*.c)

    add_custom_target(
      lint
      COMMAND ${CLANG_TIDY_EXE} -p ${CMAKE_BINARY_DIR} ${ALL_SOURCE_FILES}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMENT "Running clang-tidy on source files")
  endif()
endif()

# -------------------------
# Sources & Library
# -------------------------
file(GLOB_RECURSE WAMPPROTO_SOURCES src/*.c)
add_library(wampproto SHARED ${WAMPPROTO_SOURCES})
set_target_properties(wampproto PROPERTIES VERSION "0.1.0" SOVERSION "0")

# -------------------------
# Dependencies
# -------------------------

# Msgpack
find_library(MSGPACK_LIB NAMES msgpackc msgpack-c)
if(NOT MSGPACK_LIB)
  message(
    FATAL_ERROR "Could not find msgpack library (tried msgpackc and msgpack-c)")
else()
  message(STATUS "Found msgpack library: ${MSGPACK_LIB}")
endif()

# Sodium
find_library(SODIUM_LIBRARY sodium)
find_path(SODIUM_INCLUDE_DIR sodium.h)

# uthash (optional)
find_library(UTHASH_LIB NAMES uthash)

# TinyCBOR
find_path(
  TINYCBOR_INCLUDE_DIR
  NAMES cbor.h
  PATH_SUFFIXES tinycbor)
find_library(TINYCBOR_LIBRARIES NAMES tinycbor)

if(NOT TINYCBOR_INCLUDE_DIR OR NOT TINYCBOR_LIBRARIES)
  message(
    FATAL_ERROR
      "TinyCBOR not found. Set TINYCBOR_INCLUDE_DIR / TINYCBOR_LIBRARIES")
else()
  message(
    STATUS "Found TinyCBOR: ${TINYCBOR_LIBRARIES} -- ${TINYCBOR_INCLUDE_DIR}")
endif()

# MbedTLS (optional)
find_package(MbedTLS QUIET)
if(MbedTLS_FOUND)
  set(MBEDTLS_INCLUDE_DIRS ${MbedTLS_INCLUDE_DIRS})
  set(MBEDTLS_LIBS mbedtls mbedcrypto)
  add_definitions(-DHAVE_MBEDTLS)
else()
  # Fallback search
  find_path(MBEDTLS_INCLUDE_DIR mbedtls/version.h)
  find_library(MBEDTLS_LIBRARY mbedtls)
  find_library(MBEDCRYPTO_LIBRARY mbedcrypto)
  if(MBEDTLS_INCLUDE_DIR
     AND MBEDTLS_LIBRARY
     AND MBEDCRYPTO_LIBRARY)
    set(MBEDTLS_INCLUDE_DIRS ${MBEDTLS_INCLUDE_DIR})
    set(MBEDTLS_LIBS ${MBEDTLS_LIBRARY} ${MBEDCRYPTO_LIBRARY})
    add_definitions(-DHAVE_MBEDTLS)
  else()
    message(WARNING "MbedTLS not found; WAMPCRA salted won't be supported.")
    set(MBEDTLS_INCLUDE_DIRS "")
    set(MBEDTLS_LIBS "")
  endif()
endif()

# -------------------------
# Target Configuration
# -------------------------
target_include_directories(
  wampproto
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/wampproto>
  PUBLIC ${SODIUM_INCLUDE_DIR} ${TINYCBOR_INCLUDE_DIR} ${MBEDTLS_INCLUDE_DIRS})

target_link_libraries(
  wampproto PUBLIC cjson ${TINYCBOR_LIBRARIES} ${MSGPACK_LIB} ${SODIUM_LIBRARY}
                   ${MBEDTLS_LIBS} m)

# -------------------------
# Installation
# -------------------------
install(
  TARGETS wampproto
  EXPORT wampprotoTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# -------------------------
# Tests
# -------------------------
option(WAMMPROTO_BUILD_TESTS "Build WAMPProto tests" OFF)
if(WAMMPROTO_BUILD_TESTS)
  enable_testing()

  add_executable(test_serializers tests/test_serializers.c)
  target_link_libraries(test_serializers PRIVATE wampproto)
  add_test(NAME test_serializers COMMAND test_serializers)

  add_executable(test_rawsocket tests/test_rawsocket.c)
  target_link_libraries(test_rawsocket PRIVATE wampproto)
  add_test(NAME test_rawsocket COMMAND test_rawsocket)

  add_executable(test_authenticators tests/test_authenticators.c)
  target_link_libraries(test_authenticators PRIVATE wampproto)
  add_test(NAME test_authenticators COMMAND test_authenticators)

  add_executable(test_messages tests/test_messages.c)
  target_link_libraries(test_messages PRIVATE wampproto)
  add_test(NAME test_messages COMMAND test_messages)

  add_executable(test_wampproto_session tests/test_wampproto_session.c)
  target_link_libraries(test_wampproto_session PRIVATE wampproto)
  add_test(NAME test_wampproto_session COMMAND test_wampproto_session)
endif()

# -------------------------
# CMake Package Config
# -------------------------
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/wampprotoConfigVersion.cmake"
  VERSION 0.1.0
  COMPATIBILITY SameMajorVersion)

install(
  EXPORT wampprotoTargets
  FILE wampprotoTargets.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/wampproto)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/wampprotoConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/wampprotoConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/wampproto)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/wampprotoConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/wampprotoConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/wampproto)
