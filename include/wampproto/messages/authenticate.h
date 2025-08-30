#ifndef WAMPPROTO_AUTHENTICATE_H
#define WAMPPROTO_AUTHENTICATE_H

#include "wampproto/messages.h"
#include "wampproto/value.h"

typedef struct {
    Message base;
    char *signature;
    Value auth_extra;
} Authenticate;

Authenticate *authenticate_new(char *signature, Value auth_extra);

Authenticate *authenticate_parse(const Value *val);

#endif
