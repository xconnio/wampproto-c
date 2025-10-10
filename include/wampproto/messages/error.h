#ifndef WAMPPROTO_ERROR_H
#define WAMPPROTO_ERROR_H

#include "wampproto/messages/message.h"

#define MESSAGE_TYPE_ERROR 8

typedef struct
{
    Message base;
    int64_t message_type;
    int64_t request_id;
    Dict *details;
    char *uri;
    List *args;
    Dict *kwargs;

} Error;

Error *error_new(int64_t message_type, int64_t request_id, Dict *details, char *uri, List *args,
                 Dict *kwargs);

Message *error_parse(const List *val);

#endif
