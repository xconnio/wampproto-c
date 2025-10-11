#ifndef WAMPPROTO_UNREGISTER_H
#define WAMPPROTO_UNREGISTER_H

#include "wampproto/messages/message.h"
#define MESSAGE_TYPE_UNREGISTER 66

typedef struct
{
    Message base;

    int64_t request_id;
    int64_t registration_id;

} Unregister;

Unregister *unregister_new(int64_t request_id, int64_t registration_id);

Message *unregister_parse(const List *val);

#endif
