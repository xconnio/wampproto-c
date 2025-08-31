#ifndef WAMPPROTO_REGISTERED_MESSAGE_H
#define WAMPPROTO_REGISTERED_MESSAGE_H

#include "wampproto/messages.h"
#include "wampproto/value.h"

typedef struct {
    Message base;
    int64_t request_id;
    int64_t registration_id;
} Registered;

Registered *registered_new(int64_t request_id, int64_t registration_id);

Message *registered_parse(const List *val);

#endif
