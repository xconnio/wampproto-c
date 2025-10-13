#ifndef WAMPPROTO_AUTHENTICATE_H
#define WAMPPROTO_AUTHENTICATE_H

#include "wampproto/messages/message.h"
#include "wampproto/value.h"

#define MESSAGE_TYPE_AUTHENTICATE 5

typedef struct {
    Message base;
    const char* signature;
    Dict* auth_extra;
} Authenticate;

Authenticate* authenticate_new(const char* signature, Dict* auth_extra);

Message* authenticate_parse(const List* val);

#endif
