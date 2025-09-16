#ifndef WAMPPROTO_AUTHENTICATE_H
#define WAMPPROTO_AUTHENTICATE_H

#include "wampproto/messages/message.h"
#include "wampproto/value.h"

typedef struct
{
    Message base;
    char *signature;
    Dict auth_extra;
} Authenticate;

Authenticate *authenticate_new(const char *signature, Dict *auth_extra);

Authenticate *authenticate_parse(const List *val);

#endif
