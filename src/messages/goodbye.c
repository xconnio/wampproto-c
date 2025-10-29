#include "wampproto/messages/goodbye.h"

#include <stdlib.h>

#include "wampproto/value.h"

static List* goodbye_marshal(const Message* self) {
    Goodbye* goodbye = (Goodbye*)self;

    Value* list = value_list(3);
    value_list_append(list, value_int(MESSAGE_TYPE_GOODBYE));
    value_list_append(list, value_from_dict(goodbye->details));
    value_list_append(list, value_str(goodbye->uri));

    return (List*)list;
}

static void goodbye_free(Message* self) {
    Goodbye* goodbye = (Goodbye*)self;
    value_free(value_from_dict(goodbye->details));
    free(self);
}

Goodbye* goodbye_new(Dict* details, const char* uri) {
    Goodbye* goodbye = calloc(1, sizeof(*goodbye));
    goodbye->base.message_type = MESSAGE_TYPE_GOODBYE;
    goodbye->base.marshal = goodbye_marshal;
    goodbye->base.free = goodbye_free;
    goodbye->base.parse = goodbye_parse;

    goodbye->details = details;
    goodbye->uri = uri;

    return goodbye;
}

Message* goodbye_parse(const List* val) {
    if (!val || val->len != 3) return NULL;

    Dict* details = value_as_dict(val->items[1]);
    char* uri = value_as_str(val->items[2]);

    return (Message*)goodbye_new(details, uri);
}
