#include "stdio.h"
#include <assert.h>

#include "wampproto/messages/registered.h"
#include "wampproto/serializers.h"

void test_cbor(void);
void test_msgpack(void);
void test_json(void);

int main(void)
{
    test_cbor();
    test_msgpack();
    test_json();
    return 0;
}

const Message *create_registered_message(void)
{
    const Registered *registered = registered_new(1, 2);
    const Message *msg = (Message *)registered;
    return msg;
}

const void *check_serializer(const Serializer *serializer, const Message *msg)
{
    const Bytes arr = serializer->serialize(serializer, msg);
    const Message *deserialized = serializer->deserialize(serializer, arr);
    assert(deserialized != NULL);

    const Registered *r = (Registered *)deserialized;
    assert(r != NULL);
    assert(r->base.message_type(deserialized) == 65);

    return NULL;
}

void test_cbor(void)
{
    const Serializer *serializer = cbor_serializer_new();
    const Message *msg = create_registered_message();
    check_serializer(serializer, msg);
}

void test_msgpack(void)
{
    const Serializer *serializer = msgpack_serializer_new();
    const Message *msg = create_registered_message();
    check_serializer(serializer, msg);
}

void test_json(void)
{
    const Serializer *serializer = json_serializer_new();
    const Message *msg = create_registered_message();
    check_serializer(serializer, msg);
}
