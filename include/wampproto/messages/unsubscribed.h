#ifndef WAMPPROTO_UNSUBCRIBED_H
#define WAMPPROTO_UNSUBCRIBED_H

#include "wampproto/messages/message.h"
#define MESSAGE_TYPE_UNSUBSCRIBED 35

typedef struct {
    Message base;
    int64_t request_id;

} Unsubscribed;

Unsubscribed *unsubscribed_new(int64_t request_id);

Message *unsubscribed_parse(const List *val);

#endif
