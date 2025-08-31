#ifndef WAMPPROTO_CHALLENGE_H
#define WAMPPROTO_CHALLENGE_H

#include "wampproto/messages.h"
#include "wampproto/value.h"

typedef struct {
    Message base;
    char *auth_method;
    Dict auth_extra;
} Challenge;

Challenge *challenge_new(char *auth_method, Dict auth_extra);

Challenge *challenge_parse(const List *val);

#endif
