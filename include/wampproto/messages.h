#ifndef WAMPPROTO_MESSAGES_H
#define WAMPPROTO_MESSAGES_H

#include "wampproto/value.h"

typedef struct Message Message;

struct Message {
    int (*message_type)(const Message *self);

    Value *(*marshal)(const Message *self);

    void (*free)(Message *self);

    Message *(*parse)(const Value *data);
};

#endif
