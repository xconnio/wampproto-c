#ifndef WAMPPROTO_MESSAGE_H
#define WAMPPROTO_MESSAGE_H

#include "wampproto/messages/message.h"
#include "wampproto/value.h"

typedef struct Message Message;

struct Message
{
    int message_type;

    List *(*marshal)(const Message *self);

    void (*free)(Message *self);

    Message *(*parse)(const List *data);
};

#endif
