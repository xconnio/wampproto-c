cmake_minimum_required(VERSION 3.18)
project(wampproto C)

set(CMAKE_C_STANDARD 11)

file(GLOB_RECURSE WAMPPROTO_SOURCES src/*.c)
add_library(wampproto STATIC ${WAMPPROTO_SOURCES}
        include/wampproto/authenticators.h
        include/wampproto/messages.h
        include/wampproto/serializers.h
        include/wampproto/value.h
        include/wampproto/cbor_serializer.h

        src/cbor_serializer.c
        src/value.c
        src/wampproto.c
        src/messages/registered.c)

find_package(TinyCBOR CONFIG REQUIRED)
target_link_libraries(wampproto PUBLIC tinycbor)


# Public include dir (installed, exported)
target_include_directories(wampproto PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
# Test runner / entrypoint
option(BUILD_TESTS "Build test runner" ON)
if(BUILD_TESTS)
    add_executable(wampproto_test tests/main.c)
    target_link_libraries(wampproto_test PRIVATE wampproto)
endif()
