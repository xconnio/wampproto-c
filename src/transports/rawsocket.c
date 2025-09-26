#include "wampproto/transports/rawsocket.h"

#include <math.h>
#include <stdlib.h>

Handshake *handshake_new(const Serializer serializer, const int max_message_size)
{
    Handshake *hs = malloc(sizeof(Handshake));
    if (!hs)
        return NULL;

    hs->serializer = serializer;
    hs->max_message_size = max_message_size;
    return hs;
}

void handshake_free(Handshake *hs)
{
    free(hs);
}

int send_handshake(const Handshake *hs, uint8_t out[4])
{
    if (hs->max_message_size > PROTOCOL_MAX_MSG_SIZE)
        return -1;

    const double log2_val = log2(hs->max_message_size);
    const int log2_int = (int)log2_val;

    if (1 << log2_int != hs->max_message_size || log2_int < 9)
        return -2;

    const uint8_t b1 = (uint8_t)((log2_int - 9) << 4 | hs->serializer & 0x0F);
    out[0] = MAGIC_BYTE;
    out[1] = b1;
    out[2] = 0x00;
    out[3] = 0x00;

    return 0;
}

Handshake *receive_handshake(const uint8_t data[4], int *err)
{
    if (err)
        *err = 0;

    if (!data)
    {
        if (err)
            *err = -1;
        return NULL;
    }

    if (data[0] != MAGIC_BYTE)
    {
        if (err)
            *err = -2;
        return NULL;
    }

    if (data[2] != 0x00 || data[3] != 0x00)
    {
        if (err)
            *err = -3;
        return NULL;
    }

    const Serializer serializer = data[1] & 0x0F;
    const int size_shift = (data[1] >> 4) + 9;
    const int max_message_size = 1 << size_shift;

    return handshake_new(serializer, max_message_size);
}

MessageHeader *message_header_new(const MessageType type, const int length)
{
    MessageHeader *header = malloc(sizeof(MessageHeader));
    if (!header)
        return NULL;

    header->type = type;
    header->length = length;
    return header;
}

void message_header_free(MessageHeader *header)
{
    free(header);
}

void send_message_header(const MessageHeader *header, uint8_t out[4])
{
    uint8_t bytes[3];
    int_to_bytes(header->length, bytes);
    out[0] = (uint8_t)header->type;
    out[1] = bytes[0];
    out[2] = bytes[1];
    out[3] = bytes[2];
}

MessageHeader *receive_message_header(const uint8_t data[4])
{
    const int length = bytes_to_int(&data[1]);
    return message_header_new((MessageType)data[0], length);
}

void int_to_bytes(const int value, uint8_t out[3])
{
    out[0] = value >> 16 & 0xFF;
    out[1] = value >> 8 & 0xFF;
    out[2] = value & 0xFF;
}

int bytes_to_int(const uint8_t b[3])
{
    return b[0] << 16 | b[1] << 8 | b[2];
}
