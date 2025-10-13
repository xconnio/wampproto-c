#ifndef WAMPPROTO_INTERRUPT_H
#define WAMPPROTO_INTERRUPT_H

#include "wampproto/messages/message.h"
#include "wampproto/value.h"

#define MESSAGE_TYPE_INTERRUPT 69

typedef struct {
    Message base;
    int64_t request_id;
    Dict* options;

} Interrupt;

Interrupt* interrupt_new(int64_t request_id, Dict* options);

Message* interrupt_parse(const List* val);

#endif
