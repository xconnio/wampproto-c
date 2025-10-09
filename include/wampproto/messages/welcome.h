#ifndef WAMPPROTO_WELCOME_H
#define WAMPPROTO_WELCOME_H

#include "wampproto/messages/message.h"
#include "wampproto/value.h"

#define MESSAGE_TYPE_WELCOME 2

typedef struct
{
    Message base;

    int64_t session_id;
    Dict *details;

    char *authid;
    char *authrole;
    char *authmethod;
    Dict *authextra;
    Dict *roles;
} Welcome;

Welcome *welcome_new(int64_t session_id, Dict *details);

Message *welcome_parse(const List *val);

#endif
