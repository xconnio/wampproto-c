#ifndef WAMPPROTO_GOODBYE_H
#define WAMPPROTO_GOODBYE_H

#include "wampproto/messages/message.h"
#define MESSAGE_TYPE_GOODBYE 6

typedef struct {
    Message base;
    Dict* details;
    char* uri;

} Goodbye;

Goodbye* goodbye_new(Dict* details, char* uri);

Message* goodbye_parse(const List* val);

#endif
