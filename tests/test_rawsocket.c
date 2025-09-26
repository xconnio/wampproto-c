#include <assert.h>
#include <stdint.h>

#include "wampproto/transports.h"

void test_handshake(void)
{
    const Handshake *hs = handshake_new(SERIALIZER_CBOR, 1024);
    uint8_t outgoing_bytes[4];
    const int result = send_handshake(hs, outgoing_bytes);
    assert(result == 0);

    int err = 0;
    const Handshake *received = receive_handshake(outgoing_bytes, &err);

    assert(err == 0);
    assert(received->serializer == SERIALIZER_CBOR);
    assert(received->max_message_size == 1024);
}

int main(void)
{
    test_handshake();
    return 0;
}
