#ifndef WAMPPROTO_YIELD_H
#define WAMPPROTO_YIELD_H

#include "wampproto/messages/message.h"
#define MESSAGE_TYPE_YIELD 70

typedef struct {
    Message base;

    int64_t request_id;
    Dict* options;
    List* args;
    Dict* kwargs;

} Yield;

Yield* yield_new(int64_t request_id, Dict* options, List* args, Dict* kwargs);

Message* yield_parse(const List* val);

#endif
