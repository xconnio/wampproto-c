#ifndef WAMPPROTO_INVOCATION_H
#define WAMPPROTO_INVOCATION_H

#include "wampproto/messages/message.h"
#include "wampproto/value.h"

#define MESSAGE_TYPE_INVOCATION 68

typedef struct
{

    Message base;

    int64_t request_id;
    int64_t registration_id;
    Dict *details;
    List *args;
    Dict *kwargs;

} Invocation;

Invocation *invocation_new(int64_t request_id, int64_t registration_id, Dict *details, List *args,
                           Dict *kwargs);

Message *invocation_parse(const List *val);

#endif
