#ifndef WAMPPROTO_PUBLISH_H
#define WAMPPROTO_PUBLISH_H

#include "wampproto/messages/message.h"
#define MESSAGE_TYPE_PUBLISH 16

typedef struct {
    Message base;

    int64_t request_id;
    Dict* options;
    char* uri;
    List* args;
    Dict* kwargs;
} Publish;

Publish* publish_new(int64_t request_id, Dict* options, char* uri, List* args, Dict* kwargs);

Message* publish_parse(const List* val);

#endif
