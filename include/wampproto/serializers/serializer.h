#ifndef WAMPPROTO_SERIALIZER_H
#define WAMPPROTO_SERIALIZER_H
#include <stddef.h>
#include <stdint.h>

#include "wampproto/messages/message.h"

typedef struct Serializer Serializer;

struct Serializer
{
    Bytes (*serialize)(const Serializer *self, const Message *msg);
    Message *(*deserialize)(const Serializer *self, Bytes data);
    void (*free)(Serializer *self);
};

#endif
