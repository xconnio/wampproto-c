#include <assert.h>
#include <stdint.h>

#include "wampproto/transports.h"

int main(void)
{
    const Handshake hs = {.max_message_size = 1024, .serializer = SERIALIZER_JSON};
    uint8_t out[4];
    const int result = send_handshake(&hs, out);
    assert(result == 0);
    return 0;
}
