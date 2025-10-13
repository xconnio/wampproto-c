#include "wampproto/messages/unsubscribed.h"

#include <stdlib.h>

#include "wampproto/value.h"

static List *unsubscribed_marshal(const Message *self) {
    Unsubscribed *unsubscribed = (Unsubscribed *)self;

    Value *list = value_list(2);

    value_list_append(list, value_int(MESSAGE_TYPE_UNSUBSCRIBED));
    value_list_append(list, value_int(unsubscribed->request_id));

    return (List *)list;
}

static void unsubscribed_free(Message *self) { free(self); }

Unsubscribed *unsubscribed_new(int64_t request_id) {
    Unsubscribed *unsubscribed = calloc(1, sizeof(*unsubscribed));

    unsubscribed->base.message_type = MESSAGE_TYPE_UNSUBSCRIBED;
    unsubscribed->base.marshal = unsubscribed_marshal;
    unsubscribed->base.free = unsubscribed_free;
    unsubscribed->base.parse = unsubscribed_parse;

    unsubscribed->request_id = request_id;

    return unsubscribed;
}

Message *unsubscribed_parse(const List *val) {
    if (!val || val->len != 2) return NULL;

    int64_t request_id = value_as_int(val->items[1]);

    return (Message *)unsubscribed_new(request_id);
}
