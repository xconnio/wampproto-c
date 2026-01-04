#ifndef WAMPPROTO_CALL_H
#define WAMPPROTO_CALL_H

#include "wampproto/messages/message.h"
#include "wampproto/value.h"

#define MESSAGE_TYPE_CALL 48

typedef struct {
    Message base;
    int64_t request_id;
    Dict* options;
    const char* procedure;
    List* args;
    Dict* kwargs;
} Call;

Call* call_new(int64_t request_id, Dict* options, const char* procedure, List* args, Dict* kwargs);

Message* call_parse(const List* val);

#endif
