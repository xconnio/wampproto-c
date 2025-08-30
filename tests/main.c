#include <assert.h>
#include "stdio.h"

#include "wampproto/messages/registered.h"
#include "wampproto/serializers.h"

void test_cbor(void);
void test_msgpack(void);
void test_json(void);

int main(void) {
    test_cbor();
    test_msgpack();
    test_json();
    return 0;
}

void test_cbor(void) {
    const Serializer *serializer = cbor_serializer_new();
    const Registered *registered = registered_new(1, 2);
    const Message *msg = (Message *) registered;

    const ByteArray arr = serializer->serialize(serializer, msg);

    const Message *deserialized = serializer->deserialize(serializer, arr.data, arr.len);
    assert(deserialized != NULL);

    const Registered *r = (Registered *) deserialized;
    assert(r != NULL);
    assert(r->base.message_type(deserialized) == 65);
}

void test_msgpack(void) {
    const Serializer *serializer = cbor_serializer_new();
    const Registered *registered = registered_new(1, 2);
    const Message *msg = (Message *) registered;

    const ByteArray arr = serializer->serialize(serializer, msg);

    const Message *deserialized = serializer->deserialize(serializer, arr.data, arr.len);
    assert(deserialized != NULL);

    const Registered *r = (Registered *) deserialized;
    assert(r != NULL);
    assert(r->base.message_type(deserialized) == 65);
}

void test_json(void) {
    const Serializer *serializer = cbor_serializer_new();
    const Registered *registered = registered_new(1, 2);
    const Message *msg = (Message *) registered;

    const ByteArray arr = serializer->serialize(serializer, msg);

    const Message *deserialized = serializer->deserialize(serializer, arr.data, arr.len);
    assert(deserialized != NULL);

    const Registered *r = (Registered *) deserialized;
    assert(r != NULL);
    assert(r->base.message_type(deserialized) == 65);
}
