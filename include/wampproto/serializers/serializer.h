#ifndef WAMPPROTO_SERIALIZER_H
#define WAMPPROTO_SERIALIZER_H
#include <stddef.h>
#include <stdint.h>

#include "wampproto/messages/message.h"

typedef struct Serializer Serializer;

typedef struct {
    uint8_t *data;
    size_t   len;
} ByteArray;

struct Serializer {
    ByteArray (*serialize)(const Serializer *self, const Message *msg);
    Message *(*deserialize)(const Serializer *self, const uint8_t *data, size_t len);
    void (*free)(Serializer *self);
};

#endif
