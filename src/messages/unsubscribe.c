#include "wampproto/messages/unsubscribe.h"

#include <stdlib.h>

#include "wampproto/messages/message.h"
#include "wampproto/value.h"

static List *unsubscribe_marhsal(const Message *msg) {
    Unsubscribe *unsubscribe = (Unsubscribe *)msg;

    Value *list = value_list(3);
    value_list_append(list, value_int(MESSAGE_TYPE_UNSUBSCRIBE));
    value_list_append(list, value_int(unsubscribe->request_id));
    value_list_append(list, value_int(unsubscribe->subscription_id));

    return (List *)list;
}

static void unsubscribe_free(Message *msg) { free(msg); }

Unsubscribe *unsubscribe_new(int64_t request_id, int64_t subscription_id) {
    Unsubscribe *unsubscribe = calloc(1, sizeof(*unsubscribe));

    unsubscribe->base.message_type = MESSAGE_TYPE_UNSUBSCRIBE;
    unsubscribe->base.marshal = unsubscribe_marhsal;
    unsubscribe->base.free = unsubscribe_free;
    unsubscribe->base.parse = unsubscribe_parse;

    unsubscribe->request_id = request_id;
    unsubscribe->subscription_id = subscription_id;

    return unsubscribe;
}

Message *unsubscribe_parse(const List *val) {
    if (!val || val->len != 3) return NULL;

    int64_t request_id = value_as_int(val->items[1]);
    int64_t subscription_id = value_as_int(val->items[2]);

    return (Message *)unsubscribe_new(request_id, subscription_id);
}
