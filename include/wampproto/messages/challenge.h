#ifndef WAMPPROTO_CHALLENGE_H
#define WAMPPROTO_CHALLENGE_H

#include "wampproto/messages/message.h"
#include "wampproto/value.h"

#define MESSAGE_TYPE_CHALLENGE 4

typedef struct {
    Message base;
    char* auth_method;
    Dict* auth_extra;
} Challenge;

Challenge* challenge_new(char* auth_method, Dict* auth_extra);

Message* challenge_parse(const List* val);

#endif
