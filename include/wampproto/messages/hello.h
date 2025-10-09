#ifndef WAMPPROTO_HELLO_H
#define WAMPPROTO_HELLO_H

#include "wampproto/messages/message.h"
#include "wampproto/value.h"

#define MESSAGE_TYPE_HELLO 1

typedef struct
{
    Message base;
    char *realm;
    Dict *details;
    char *authid;
    Dict *roles;
    List *authmethods;
    Dict *auth_extra;

} Hello;

Hello *hello_new2(char *realm, Dict *details);
Hello *hello_new(char *realm, char *auth_id, Dict *auth_extra, Dict *roles, List *auth_methods);

Message *hello_parse(const List *val);

#endif
