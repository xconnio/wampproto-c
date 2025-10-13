#include "wampproto/messages/interrupt.h"

#include "wampproto/messages/message.h"
#include "wampproto/value.h"

static List* interrupt_marshal(const Message* self) {
    Interrupt* interrupt = (Interrupt*)self;

    Value* list = value_list(3);
    value_list_append(list, value_int(MESSAGE_TYPE_INTERRUPT));
    value_list_append(list, value_int(interrupt->request_id));
    value_list_append(list, value_from_dict(interrupt->options));

    return (List*)list;
}

static void interrupt_free(Message* self) {
    Interrupt* interrupt = (Interrupt*)self;
    value_free(value_from_dict(interrupt->options));
    free(self);
}

Interrupt* interrupt_new(int64_t request_id, Dict* options) {
    Interrupt* interrupt = calloc(1, sizeof(*interrupt));
    interrupt->base.message_type = MESSAGE_TYPE_INTERRUPT;
    interrupt->base.marshal = interrupt_marshal;
    interrupt->base.free = interrupt_free;
    interrupt->base.parse = interrupt_parse;

    interrupt->request_id = request_id;
    interrupt->options = options;

    return interrupt;
}

Message* interrupt_parse(const List* val) {
    if (!val || val->len != 3) return NULL;

    int64_t request_id = value_as_int(val->items[1]);
    Dict* options = value_as_dict(val->items[2]);

    return (Message*)interrupt_new(request_id, options);
}
