#include "wampproto/messages/event.h"

#include <stdlib.h>

#include "wampproto/value.h"

static List* event_marshal(const Message* self) {
    Event* event = (Event*)self;

    int length = 4;
    if (event->args != NULL) length = 5;
    if (event->kwargs != NULL) length = 6;

    Value* list = value_list(length);

    value_list_append(list, value_int(MESSAGE_TYPE_EVENT));
    value_list_append(list, value_int(event->subscription_id));
    value_list_append(list, value_int(event->publication_id));
    value_list_append(list, value_from_dict(event->details));

    if (length >= 5) value_list_append(list, value_from_list(event->args));

    if (length == 6) value_list_append(list, value_from_dict(event->kwargs));

    return (List*)list;
}

static void event_free(Message* self) {
    Event* event = (Event*)self;

    value_free(value_from_dict(event->details));

    if (event->args != NULL) value_free(value_from_list(event->args));

    if (event->kwargs != NULL) value_free(value_from_dict(event->kwargs));

    free(event);
}

Event* event_new(int64_t subscription_id, int64_t publication_id, Dict* details, List* args,

                 Dict* kwargs) {
    Event* event = calloc(1, sizeof(*event));

    event->base.message_type = MESSAGE_TYPE_EVENT;
    event->base.marshal = event_marshal;
    event->base.free = event_free;
    event->base.parse = event_parse;

    event->subscription_id = subscription_id;
    event->publication_id = publication_id;
    event->details = details;
    event->args = args;
    event->kwargs = kwargs;

    return event;
}

Message* event_parse(const List* val) {
    if (!val || val->len < 4) return NULL;

    int64_t subscription_id = value_as_int(val->items[1]);
    int64_t publication_id = value_as_int(val->items[2]);
    Dict* details = value_as_dict(val->items[3]);

    List* args = NULL;
    if (val->len >= 5) args = value_as_list(val->items[4]);

    Dict* kwargs = NULL;
    if (val->len == 6) kwargs = value_as_dict(val->items[5]);

    return (Message*)event_new(subscription_id, publication_id, details, args, kwargs);
}
