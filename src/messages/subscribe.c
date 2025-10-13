#include "wampproto/messages/subscribe.h"

#include <stdlib.h>

#include "wampproto/value.h"

static List *subscribe_marshal(const Message *self) {
    Subscribe *subscribe = (Subscribe *)self;

    Value *list = value_list(4);
    value_list_append(list, value_int(MESSAGE_TYPE_SUBSCRIBE));
    value_list_append(list, value_int(subscribe->request_id));
    value_list_append(list, value_from_dict(subscribe->options));
    value_list_append(list, value_str(subscribe->uri));

    return (List *)list;
}

static void subscribe_free(Message *self) {
    Subscribe *subscribe = (Subscribe *)self;

    value_free(value_from_dict(subscribe->options));

    free(subscribe);
}

Subscribe *subscribe_new(int64_t request_id, Dict *options, char *uri) {
    Subscribe *subscribe = calloc(1, sizeof(*subscribe));

    subscribe->base.message_type = MESSAGE_TYPE_SUBSCRIBE;
    subscribe->base.marshal = subscribe_marshal;
    subscribe->base.free = subscribe_free;
    subscribe->base.parse = subscribe_parse;

    subscribe->request_id = request_id;
    subscribe->options = options;
    subscribe->uri = uri;

    return subscribe;
}

Message *subscribe_parse(const List *val) {
    if (!val || val->len != 4) return NULL;

    int64_t request_id = value_as_int(val->items[1]);
    Dict *options = value_as_dict(val->items[2]);
    char *uri = value_as_str(val->items[3]);

    return (Message *)subscribe_new(request_id, options, uri);
}
