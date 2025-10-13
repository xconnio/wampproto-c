#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>

#include "stdio.h"
#include "wampproto/dict.h"
#include "wampproto/messages/authenticate.h"
#include "wampproto/messages/message.h"
#include "wampproto/messages/registered.h"
#include "wampproto/serializers/cbor.h"
#include "wampproto/serializers/json.h"
#include "wampproto/serializers/msgpack.h"
#include "wampproto/serializers/serializer.h"
#include "wampproto/value.h"

void test_cbor(void);
void test_msgpack(void);
void test_json(void);
void test_authenticate_json(void);

int main(void) {
    test_cbor();
    test_msgpack();
    test_json();
    test_authenticate_json();
    return 0;
}

const Message* create_authenticate_message(void) {
    const char* challenge = "challenge_string";
    const char* salt = "hello";

    Value* c = value_str(challenge);
    Value* s = value_str(salt);
    Value* i = value_int(10);
    Value* l = value_int(100);
    Dict* dict = create_dict();

    dict_insert(dict, "challenge", c);
    dict_insert(dict, "salt", s);
    dict_insert(dict, "interations", i);
    dict_insert(dict, "keylen", l);

    Authenticate* authenticate = authenticate_new("cleartext", dict);

    return (Message*)authenticate;
}

const Message* create_registered_message(void) {
    const Registered* registered = registered_new(0, 2);
    const Message* msg = (Message*)registered;
    return msg;
}

const void* check_serializer(const Serializer* serializer, const Message* msg) {
    const Bytes arr = serializer->serialize(serializer, msg);
    const Message* deserialized = serializer->deserialize(serializer, arr);
    assert(deserialized != NULL);

    const Registered* r = (Registered*)deserialized;
    assert(r != NULL);
    assert(r->base.message_type == 65);

    return NULL;
}

const void* check_serializer2(const Serializer* serializer, const Message* msg) {
    const Bytes arr = serializer->serialize(serializer, msg);
    const Message* deserialized = serializer->deserialize(serializer, arr);
    assert(deserialized != NULL);

    const Authenticate* r = (Authenticate*)deserialized;
    assert(r != NULL);
    assert(r->base.message_type == 5);

    Dict* dict = r->auth_extra;
    Value* value = dict_get(dict, "salt");

    assert(strcmp(value_as_str(value), "hello") == 0);
    assert(strcmp(r->signature, "cleartext") == 0);

    return NULL;
}

void test_cbor(void) {
    const Serializer* serializer = msgpack_serializer_new();
    const Message* msg = create_registered_message();
    check_serializer(serializer, msg);
}

void test_msgpack(void) {
    const Serializer* serializer = msgpack_serializer_new();
    const Message* msg = create_registered_message();
    check_serializer(serializer, msg);
}

void test_json(void) {
    const Serializer* serializer = json_serializer_new();
    const Message* msg = create_registered_message();
    check_serializer(serializer, msg);
}

void test_authenticate_json(void) {
    const Serializer* serializer = cbor_serializer_new();
    const Message* msg = create_authenticate_message();
    check_serializer2(serializer, msg);
}
