#ifndef WAMPPROTO_ABORT_H
#define WAMPPROTO_ABORT_H

#include "wampproto/messages/message.h"
#include "wampproto/value.h"
#define MESSAGE_TYPE_ABORT 3

typedef struct
{
    Message base;
    Dict *details;
    char *reason;
    List *args;
    Dict *kwargs;
} Abort;

Abort *abort_new(Dict *details, char *reason, List *args, Dict *kwargs);

Message *abort_parse(const List *val);

#endif
