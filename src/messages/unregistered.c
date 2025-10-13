#include "wampproto/messages/unregistered.h"

#include <stdlib.h>

#include "wampproto/value.h"

static List* unregistered_marshal(const Message* self) {
    Unregistered* unregistered = (Unregistered*)self;

    Value* list = value_list(2);

    value_list_append(list, value_int(MESSAGE_TYPE_UNREGISTERED));
    value_list_append(list, value_int(unregistered->request_id));

    return (List*)list;
}

static void unregistered_free(Message* self) { free(self); }

Unregistered* unregistered_new(int64_t request_id) {
    Unregistered* unregistered = calloc(1, sizeof(*unregistered));

    unregistered->base.message_type = MESSAGE_TYPE_UNREGISTERED;
    unregistered->base.marshal = unregistered_marshal;
    unregistered->base.free = unregistered_free;
    unregistered->base.parse = unregistered_parse;

    unregistered->request_id = request_id;

    return unregistered;
}

Message* unregistered_parse(const List* val) {
    if (!val || val->len != 2) return NULL;

    int64_t request_id = value_as_int(val->items[1]);

    return (Message*)unregistered_new(request_id);
}
