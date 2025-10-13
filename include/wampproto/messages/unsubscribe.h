#ifndef WAMPPROTO_UNSUBSCRIBE_H
#define WAMPPROTO_UNSUBSCRIBE_H

#include "wampproto/messages/message.h"

#define MESSAGE_TYPE_UNSUBSCRIBE 34

typedef struct {
    Message base;

    int64_t request_id;
    int64_t subscription_id;
} Unsubscribe;

Unsubscribe *unsubscribe_new(int64_t request_id, int64_t subscription_id);

Message *unsubscribe_parse(const List *val);

#endif
