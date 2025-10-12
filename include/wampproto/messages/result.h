#ifndef WAMPPROTO_RESULT_H
#define WAMPPROTO_RESULT_H

#include "wampproto/messages/message.h"
#define MESSAGE_TYPE_RESULT 50

typedef struct
{

    Message base;
    int64_t request_id;
    Dict *details;
    List *args;
    Dict *kwargs;

} Result;

Result *result_new(int64_t request_id, Dict *details, List *args, Dict *kwargs);

Message *result_parse(const List *val);
#endif
