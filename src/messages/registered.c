#include "wampproto/messages/registered.h"

#include <stdio.h>
#include <stdlib.h>

#include "wampproto/value.h"

static List* registered_marshal(const Message* self) {
    const Registered* r = (const Registered*)self;
    Value* v = value_list(3);
    value_list_append(v, value_int(MESSAGE_TYPE_REGISTERED));
    value_list_append(v, value_int(r->request_id));
    value_list_append(v, value_int(r->registration_id));
    return (List*)v;
}

static void registered_free(Message* self) { free(self); }

Registered* registered_new(const int64_t request_id, const int64_t registration_id) {
    Registered* r = calloc(1, sizeof(Registered));
    r->base.message_type = MESSAGE_TYPE_REGISTERED;
    r->base.marshal = registered_marshal;
    r->base.free = registered_free;
    r->base.parse = registered_parse;

    r->request_id = request_id;
    r->registration_id = registration_id;
    return r;
}

Message* registered_parse(const List* val) {
    if (!val || val->len < 3) return NULL;

    const int64_t request_id = value_as_int(val->items[1]);
    const int64_t registration_id = value_as_int(val->items[2]);

    return (Message*)registered_new(request_id, registration_id);
}
