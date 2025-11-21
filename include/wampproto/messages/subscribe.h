#ifndef WAMPPROTO_SUBSCRIBE_H
#define WAMPPROTO_SUBSCRIBE_H

#include "wampproto/messages/message.h"
#define MESSAGE_TYPE_SUBSCRIBE 32

typedef struct {
    Message base;

    int64_t request_id;
    Dict *options;
    const char *uri;

} Subscribe;

Subscribe *subscribe_new(int64_t request_id, Dict *options, const char *uri);

Message *subscribe_parse(const List *val);

#endif
