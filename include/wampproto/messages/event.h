#ifndef WAMPPROTO_EVENT_H
#define WAMPPROTO_EVENT_H

#include "wampproto/messages/message.h"
#include "wampproto/value.h"
#define MESSAGE_TYPE_EVENT 36

typedef struct
{

    Message base;

    int64_t subscription_id;
    int64_t publication_id;
    Dict *details;

    List *args;
    Dict *kwargs;

} Event;

Event *event_new(int64_t subscription_id, int64_t publication_id, Dict *details, List *args,
                 Dict *kwargs);

Message *event_parse(const List *val);

#endif
