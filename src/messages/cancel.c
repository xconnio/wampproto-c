#include "wampproto/messages/cancel.h"

#include <stdlib.h>

#include "wampproto/messages/message.h"
#include "wampproto/value.h"

static List* cancel_marshal(const Message* self) {
    Cancel* cancel = (Cancel*)self;

    Value* list = value_list(3);
    value_list_append(list, value_int(MESSAGE_TYPE_CANCEL));
    value_list_append(list, value_int(cancel->request_id));
    value_list_append(list, value_from_dict(cancel->options));

    return (List*)list;
}

static void cancel_free(Message* self) { free(self); }

Cancel* cancel_new(int64_t request_id, Dict* options) {
    Cancel* cancel = calloc(1, sizeof(*cancel));
    cancel->base.message_type = MESSAGE_TYPE_CANCEL;
    cancel->base.marshal = cancel_marshal;
    cancel->base.free = cancel_free;
    cancel->base.parse = cancel_parse;

    cancel->request_id = request_id;
    cancel->options = options;

    return cancel;
}

Message* cancel_parse(const List* val) {
    if (!val || val->len != 3) return NULL;

    int64_t request_id = value_as_int(val->items[1]);
    Dict* options = value_as_dict(val->items[2]);

    return (Message*)cancel_new(request_id, options);
}
