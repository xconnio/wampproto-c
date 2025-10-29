#ifndef WAMPPROTO_HELLO_H
#define WAMPPROTO_HELLO_H

#include "wampproto/messages/message.h"
#include "wampproto/value.h"

#define MESSAGE_TYPE_HELLO 1

typedef struct {
    Message base;
    const char* realm;
    Dict* details;
    const char* authid;
    Dict* roles;
    List* authmethods;
    Dict* auth_extra;

} Hello;

Hello* hello_new(const char* realm, const char* auth_id, Dict* auth_extra, Dict* roles, List* auth_methods);

Message* hello_parse(const List* val);

#endif
