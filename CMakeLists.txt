cmake_minimum_required(VERSION 3.18)
project(wampproto C)

set(CMAKE_C_STANDARD 11)

find_program(CLANG_FORMAT_EXE NAMES clang-format)
if(CLANG_FORMAT_EXE)
    message(STATUS "Found clang-format: ${CLANG_FORMAT_EXE}")

    file(GLOB_RECURSE ALL_SOURCE_FILES
            ${CMAKE_SOURCE_DIR}/include/*.h
            ${CMAKE_SOURCE_DIR}/src/*.c
            ${CMAKE_SOURCE_DIR}/tests/*.c
            ${CMAKE_SOURCE_DIR}/tests/*.h
    )

    add_custom_target(format
            COMMAND ${CLANG_FORMAT_EXE}
            -i
            -style=file
            ${ALL_SOURCE_FILES}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Running clang-format on source files"
    )
endif()

find_program(CLANG_TIDY_EXE NAMES clang-tidy)
if(CLANG_TIDY_EXE)
    message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXE}")
    set(CMAKE_C_CLANG_TIDY "${CLANG_TIDY_EXE};-checks=*,-clang-analyzer-alpha.*")

    add_custom_target(lint
            COMMAND ${CLANG_TIDY_EXE}
            -p ${CMAKE_BINARY_DIR}
            ${ALL_SOURCE_FILES}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Running clang-tidy on source files"
    )
endif()

file(GLOB_RECURSE WAMPPROTO_SOURCES src/*.c)
add_library(wampproto STATIC ${WAMPPROTO_SOURCES}
        # Public headers
        include/wampproto/authenticators.h
        include/wampproto/messages.h
        include/wampproto/serializers.h
        include/wampproto/value.h

        # Authenticators
        include/wampproto/authenticators/authenticator.h
        include/wampproto/authenticators/anonymous.h
        include/wampproto/authenticators/cryptosign.h
        include/wampproto/authenticators/ticket.h
        include/wampproto/authenticators/wampcra.h
        src/authenticators/anonymous.c
        src/authenticators/cryptosign.c
        src/authenticators/ticket.c
        src/authenticators/wampcra.c

        # Messages
        include/wampproto/messages/authenticate.h
        include/wampproto/messages/challenge.h
        include/wampproto/messages/message.h
        include/wampproto/messages/registered.h
        src/messages/registered.c

        # Serializers
        include/wampproto/serializers/cbor.h
        include/wampproto/serializers/json.h
        include/wampproto/serializers/msgpack.h
        include/wampproto/serializers/serializer.h
        src/serializers/cbor.c
        src/serializers/json.c
        src/serializers/msgpack.c

        # Core
        src/value.c
        src/serializers/helpers.c
        include/wampproto/messages/call.h
        src/messages/call.c
)

# Add TinyCBOR install path
# Location of your local TinyCBOR install
set(TINYCBOR_ROOT "${CMAKE_SOURCE_DIR}/deps/tinycbor/install")

add_library(TinyCBOR::TinyCBOR STATIC IMPORTED)

set_target_properties(TinyCBOR::TinyCBOR PROPERTIES
        IMPORTED_LOCATION "${TINYCBOR_ROOT}/lib/libtinycbor.a"
        INTERFACE_INCLUDE_DIRECTORIES "${TINYCBOR_ROOT}/include/"
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MSGPACK msgpack-c)
if (NOT MSGPACK_FOUND)
    pkg_check_modules(MSGPACK msgpackc REQUIRED)
endif()

include_directories(${MSGPACK_INCLUDE_DIRS})
link_directories(${MSGPACK_LIBRARY_DIRS})
target_link_libraries(wampproto PRIVATE TinyCBOR::TinyCBOR cjson ${MSGPACK_LIBRARIES})

# Public include dir (installed, exported)
target_include_directories(wampproto PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
# Test runner / entrypoint
option(BUILD_TESTS "Build test runner" ON)
if(BUILD_TESTS)
    enable_testing()
    add_executable(wampproto_test tests/main.c)
    target_link_libraries(wampproto_test PRIVATE wampproto)

    add_test(NAME wampproto_test COMMAND wampproto_test)
endif()
