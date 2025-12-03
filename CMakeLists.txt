cmake_minimum_required(VERSION 3.20)
project(wampproto C)
option(WAMPPROTO_BUILD_TESTS "Build WAMPProto tests" OFF)

# -------------------------
# Project Settings
# -------------------------
set(CMAKE_C_STANDARD 11)
include(GNUInstallDirs)

# -------------------------
# Formatting & Linting
# -------------------------
# (unchanged)
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
add_library(wampproto STATIC ${WAMPPROTO_SOURCES})
set_target_properties(wampproto PROPERTIES VERSION "0.1.0" SOVERSION "0")

# -------------------------
# Dependencies
# -------------------------

# Fix include directories using BUILD_INTERFACE / INSTALL_INTERFACE
target_include_directories(
  wampproto
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external/uthash/src>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external/cJSON>
         $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/wampproto>)

# --- Vendored MbedTLS ---
set(ENABLE_PROGRAMS
    OFF
    CACHE BOOL "" FORCE)
set(ENABLE_TESTING
    OFF
    CACHE BOOL "" FORCE)
set(UNSAFE_BUILD
    OFF
    CACHE BOOL "" FORCE)
add_subdirectory(external/mbedtls)
add_definitions(-DHAVE_MBEDTLS)

# Vendored libsodium
set(SODIUM_DISABLE_TESTS
    ON
    CACHE BOOL "" FORCE)
set(SODIUM_BUILD_SHARED
    OFF
    CACHE BOOL "" FORCE)
add_subdirectory(external/libsodium)
set(SODIUM_SOURCE_INCLUDE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/external/libsodium/libsodium/src/libsodium/include"
)
set(SODIUM_BINARY_INCLUDE_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/external/libsodium/libsodium/src/libsodium/include"
)
set_target_properties(
  sodium
  PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES
    "$<BUILD_INTERFACE:${SODIUM_SOURCE_INCLUDE_DIR}>;$<BUILD_INTERFACE:${SODIUM_BINARY_INCLUDE_DIR}>;$<INSTALL_INTERFACE:include>"
)

# Msgpack
set(MSGPACK_BUILD_TESTS
    OFF
    CACHE BOOL "Build msgpack tests" FORCE)
set(MSGPACK_BUILD_EXAMPLES
    OFF
    CACHE BOOL "Build msgpack examples" FORCE)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(external/msgpack-c)

# TinyCBOR
add_subdirectory(external/tinycbor EXCLUDE_FROM_ALL)

# cJSON
target_sources(wampproto PRIVATE external/cJSON/cJSON.c)

# -------------------------
# Target Configuration
# -------------------------
target_link_libraries(
  wampproto
  PUBLIC tinycbor
         sodium
         mbedtls # Changed from MbedTLS::mbedtls
         mbedx509 # Changed from MbedTLS::mbedx509
         mbedcrypto # Changed from MbedTLS::mbedcrypto
         msgpack-c
         m)

# -------------------------
# Installation
# -------------------------
# install( TARGETS wampproto EXPORT wampprotoTargets ARCHIVE DESTINATION
# ${CMAKE_INSTALL_LIBDIR} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} INCLUDES
# DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION
# ${CMAKE_INSTALL_INCLUDEDIR})

# # Export targets file install( EXPORT wampprotoTargets FILE
# wampprotoTargets.cmake NAMESPACE wampproto:: DESTINATION
# ${CMAKE_INSTALL_LIBDIR}/cmake/wampproto)

# -------------------------
# Tests (disabled by default)
# -------------------------
if(WAMPPROTO_BUILD_TESTS)
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
# Installation
# -------------------------
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# 1. Install wampproto AND its dependencies We add sodium, mbedtls, etc. here so
#   they get included in "wampprotoTargets"
install(
  TARGETS wampproto
          sodium
          mbedtls # <--- The real target name
          mbedx509 # <--- The real target name
          mbedcrypto # <--- The real target name
          everest
          p256m
          msgpack-c
          tinycbor
  EXPORT wampprotoTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# 1. Install Headers (Unchanged)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# 1. Export the targets file (Unchanged) Now this file will contain definitions
#   for wampproto::wampproto, wampproto::sodium, wampproto::mbedtls, etc.
install(
  EXPORT wampprotoTargets
  FILE wampprotoTargets.cmake
  NAMESPACE wampproto::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/wampproto)

# 1. Config and Version files (Unchanged)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/wampprotoConfigVersion.cmake"
  VERSION 0.1.0
  COMPATIBILITY SameMajorVersion)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/wampprotoConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/wampprotoConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/wampproto)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/wampprotoConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/wampprotoConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/wampproto)
