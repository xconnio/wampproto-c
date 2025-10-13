#ifndef WAMPPROTO_SUBSCRIBED_H
#define WAMPPROTO_SUBSCRIBED_H

#include "wampproto/messages/message.h"
#define MESSAGE_TYPE_SUBSCRIBED 33

typedef struct {
    Message base;

    int64_t request_id;
    int64_t subscription_id;

} Subscribed;

Subscribed *subscribed_new(int64_t request_id, int64_t subscription_id);

Message *subscribed_parse(const List *val);

#endif
