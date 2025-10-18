cmake_minimum_required(VERSION 3.18)
project(wampproto C)

set(CMAKE_C_STANDARD 11)

find_program(CLANG_FORMAT_EXE NAMES clang-format)
if(CLANG_FORMAT_EXE)
  message(STATUS "Found clang-format: ${CLANG_FORMAT_EXE}")

  file(GLOB_RECURSE ALL_SOURCE_FILES ${CMAKE_SOURCE_DIR}/include/*.h
       ${CMAKE_SOURCE_DIR}/src/*.c ${CMAKE_SOURCE_DIR}/tests/*.c
       ${CMAKE_SOURCE_DIR}/tests/*.h)

  add_custom_target(
    format
    COMMAND ${CLANG_FORMAT_EXE} -i -style=file ${ALL_SOURCE_FILES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running clang-format on source files")
endif()

find_program(CLANG_TIDY_EXE NAMES clang-tidy)
if(CLANG_TIDY_EXE)
  message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXE}")
  set(CMAKE_C_CLANG_TIDY "${CLANG_TIDY_EXE};-checks=*,-clang-analyzer-alpha.*")

  add_custom_target(
    lint
    COMMAND ${CLANG_TIDY_EXE} -p ${CMAKE_BINARY_DIR} ${ALL_SOURCE_FILES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running clang-tidy on source files")
endif()

file(GLOB_RECURSE WAMPPROTO_SOURCES src/*.c)

add_library(wampproto STATIC ${WAMPPROTO_SOURCES})

set(TINYCBOR_ROOT "${CMAKE_SOURCE_DIR}/deps/tinycbor/install")

add_library(TinyCBOR::TinyCBOR STATIC IMPORTED)

set_target_properties(
  TinyCBOR::TinyCBOR
  PROPERTIES IMPORTED_LOCATION "${TINYCBOR_ROOT}/lib/libtinycbor.a"
             INTERFACE_INCLUDE_DIRECTORIES "${TINYCBOR_ROOT}/include/")

find_library(MSGPACK_LIB NAMES msgpackc msgpack-c)
if(NOT MSGPACK_LIB)
  message(
    FATAL_ERROR "Could not find msgpack library (tried msgpackc and msgpack-c)")
else()
  message(STATUS "Found msgpack library: ${MSGPACK_LIB}")
endif()

# Public include dir (installed, exported)
target_include_directories(wampproto PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
find_library(SODIUM_LIBRARY sodium)
find_path(SODIUM_INCLUDE_DIR sodium.h)
find_library(UTHASH_LIB, uthash)

find_package(MbedTLS QUIET)
if(MbedTLS_FOUND)
  target_compile_definitions(wampproto PRIVATE HAVE_MBEDTLS)
  target_link_libraries(wampproto PRIVATE mbedtls mbedcrypto)
else()
  message(
    STATUS "MbedTLS CMake config not found, falling back to manual search")

  find_path(MBEDTLS_INCLUDE_DIR mbedtls/version.h)
  find_library(MBEDTLS_LIBRARY mbedtls)
  find_library(MBEDCRYPTO_LIBRARY mbedcrypto)

  if(MBEDTLS_INCLUDE_DIR
     AND MBEDTLS_LIBRARY
     AND MBEDCRYPTO_LIBRARY)
    target_compile_definitions(wampproto PRIVATE HAVE_MBEDTLS)
    target_include_directories(wampproto PRIVATE ${MBEDTLS_INCLUDE_DIR})
    target_link_libraries(wampproto PRIVATE ${MBEDTLS_LIBRARY}
                                            ${MBEDCRYPTO_LIBRARY})
  else()
    message(WARNING "MbedTLS not found; WAMPCRA salted wont be supported.")
  endif()
endif()

target_link_libraries(wampproto PRIVATE TinyCBOR::TinyCBOR cjson ${MSGPACK_LIB}
                                        ${SODIUM_LIBRARY} m)

# Public include dir (installed, exported)
target_include_directories(
  wampproto
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE ${SODIUM_INCLUDE_DIR})
# Test runner / entrypoint
option(BUILD_TESTS "Build test runner" ON)
if(BUILD_TESTS)
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

  add_executable(test_session tests/test_session.c)
  target_link_libraries(test_session PRIVATE wampproto)
  add_test(NAME test_session COMMAND test_session)
endif()
