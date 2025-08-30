cmake_minimum_required(VERSION 3.18)
project(wampproto C)

set(CMAKE_C_STANDARD 11)

file(GLOB_RECURSE WAMPPROTO_SOURCES src/*.c)
add_library(wampproto STATIC ${WAMPPROTO_SOURCES}
        include/wampproto/authenticators.h
        include/wampproto/messages.h
        include/wampproto/serializers.h
        include/wampproto/value.h

        src/serializers/cbor.c
        src/value.c
        src/messages/registered.c
        include/wampproto/messages/registered.h
        include/wampproto/serializers/cbor.h
        include/wampproto/serializers/msgpack.h
        include/wampproto/serializers/json.h
        include/wampproto/authenticators/ticket.h
        include/wampproto/authenticators/cryptosign.h
        include/wampproto/authenticators/wampcra.h
        src/authenticators/cryptosign.c
        src/authenticators/ticket.cpp
        src/authenticators/wampcra.cpp
        include/wampproto/messages/message.h
        src/serializers/msgpack.c
        include/wampproto/serializers/serializer.h
        include/wampproto/authenticators/authenticator.h
        src/serializers/json.c)

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
