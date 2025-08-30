#include <assert.h>
#include "stdio.h"

#include "wampproto/messages/registered.h"
#include "wampproto/serializers/cbor.h"

int main(void) {
    const Serializer *serializer = cbor_serializer_new();
    const Registered *registered = registered_new(1, 2);
    const Message *msg = (Message *) registered;

    const ByteArray arr = serializer->serialize(serializer, msg);

    const Message *deserialized = serializer->deserialize(serializer, arr.data, arr.len);
    assert(deserialized != NULL);

    const Registered *r = (Registered *) deserialized;
    assert(r != NULL);
    assert(r->base.message_type(deserialized) == 65);
    return 0;
}
