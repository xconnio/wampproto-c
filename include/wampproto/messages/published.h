#ifndef WAMPPROTO_PUBLISHED_H
#define WAMPPROTO_PUBLISHED_H

#include "wampproto/messages/message.h"
#define MESSAGE_TYPE_PUBLISHED 17

typedef struct
{

    Message base;
    int64_t request_id;
    int64_t publication_id;

} Published;

Published *published_new(int64_t request_id, int64_t publication_id);

Message *published_parse(const List *val);

#endif
