#ifndef WAMPPROTO_REGISTER_H
#define WAMPPROTO_REGISTER_H

#include "wampproto/messages/message.h"
#include <stdlib.h>
#define MESSAGE_TYPE_REGISTER 64

typedef struct
{
    Message base;
    int64_t request_id;
    Dict *options;
    char *uri;

} Register;

Register *register_new(int64_t request_id, Dict *options, char *uri);

Message *register_parse(const List *list);

#endif
