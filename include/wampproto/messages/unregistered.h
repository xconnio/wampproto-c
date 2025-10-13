#ifndef WAMPPROTO_UNREGISTERED_H
#define WAMPPROTO_UNREGISTERED_H

#include "wampproto/messages/message.h"

#define MESSAGE_TYPE_UNREGISTERED 67

typedef struct {
    Message base;

    int64_t request_id;

} Unregistered;

Unregistered* unregistered_new(int64_t request_id);

Message* unregistered_parse(const List* val);

#endif
