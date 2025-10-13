#include "wampproto/messages/subscribed.h"

#include <stdlib.h>

#include "wampproto/value.h"

static List *subscribd_marhsal(const Message *self) {
    Subscribed *subscribed = (Subscribed *)self;

    Value *list = value_list(3);

    value_list_append(list, value_int(MESSAGE_TYPE_SUBSCRIBED));
    value_list_append(list, value_int(subscribed->request_id));
    value_list_append(list, value_int(subscribed->subscription_id));

    return (List *)list;
}

static void subscribed_free(Message *self) {
    Subscribed *subscribed = (Subscribed *)self;

    free(subscribed);
}

Subscribed *subscribed_new(int64_t request_id, int64_t subscription_id) {
    Subscribed *subscribed = calloc(1, sizeof(*subscribed));

    subscribed->base.message_type = MESSAGE_TYPE_SUBSCRIBED;
    subscribed->base.marshal = subscribd_marhsal;
    subscribed->base.free = subscribed_free;
    subscribed->base.parse = subscribed_parse;

    subscribed->request_id = request_id;
    subscribed->subscription_id = subscription_id;

    return subscribed;
}

Message *subscribed_parse(const List *val) {
    if (!val || val->len != 3) return NULL;

    int64_t request_id = value_as_int(val->items[1]);
    int64_t subscription_id = value_as_int(val->items[2]);

    return (Message *)subscribed_new(request_id, subscription_id);
}
