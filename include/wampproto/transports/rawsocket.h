#ifndef WAMPPROTO_RAWSOCKET_H
#define WAMPPROTO_RAWSOCKET_H

#include <stdint.h>

#define MAGIC_BYTE 0x7F
#define PROTOCOL_MAX_MSG_SIZE (1 << 24)
#define DEFAULT_MAX_MSG_SIZE (1 << 20)

typedef enum
{
    SERIALIZER_JSON = 1,
    SERIALIZER_MSGPACK = 2,
    SERIALIZER_CBOR = 3
} Serializer;

typedef enum
{
    MESSAGE_WAMP = 0,
    MESSAGE_PING = 1,
    MESSAGE_PONG = 2
} MessageType;

typedef struct
{
    Serializer serializer;
    int max_message_size;
} Handshake;

typedef struct
{
    MessageType type;
    int length;
} MessageHeader;

/* Handshake functions */
Handshake *handshake_new(Serializer serializer, int max_message_size);
void handshake_free(Handshake *hs);

int send_handshake(const Handshake *hs, uint8_t out[4]);
Handshake *receive_handshake(const uint8_t data[4], int *err);

/* Message header functions */
MessageHeader *message_header_new(MessageType type, int length);
void message_header_free(MessageHeader *header);

void send_message_header(const MessageHeader *header, uint8_t out[4]);
MessageHeader *receive_message_header(const uint8_t data[4]);

/* Utility */
void int_to_bytes(int value, uint8_t out[3]);
int bytes_to_int(const uint8_t b[3]);

#endif // WAMPPROTO_RAWSOCKET_H
