#ifndef WAMPPROTO_CANCEL_H
#define WAMPPROTO_CANCEL_H

#include <stdint.h>

#include "wampproto/messages/message.h"
#include "wampproto/value.h"
#define MESSAGE_TYPE_CANCEL 49

typedef struct {
    Message base;

    int64_t request_id;
    Dict* options;

} Cancel;

Cancel* cancel_new(int64_t request_id, Dict* options);

Message* cancel_parse(const List* val);

#endif
