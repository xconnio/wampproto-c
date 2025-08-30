#include <assert.h>

#include "stdio.h"

#include "wampproto/cbor_serializer.h"
#include "wampproto/serializers.h"

int main(void) {
    const Serializer *serializer = cbor_serializer_new();
    const Registered *registered = registered_new(1, 2);
    const Message *msg = (Message *)registered;

    const ByteArray arr = serializer->serialize(serializer, msg);

    const Message *deserialized = serializer->deserialize(serializer, arr.data, arr.len);
    assert(deserialized != NULL);
    // Message *recreated = registered_parse(value_bytes(arr.data, arr.len));
    // const Registered *r = (Registered *)recreated;
    // assert(r->base.message_type(r) == 65);
    return 0;
}
