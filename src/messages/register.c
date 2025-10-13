#include "wampproto/messages/register.h"

#include "wampproto/messages/message.h"
#include "wampproto/value.h"

static List* register_marshal(const Message* self) {
    Register* r = (Register*)self;

    Value* list = value_list(4);
    value_list_append(list, value_int(MESSAGE_TYPE_REGISTER));
    value_list_append(list, value_int(r->request_id));
    value_list_append(list, value_from_dict(r->options));
    value_list_append(list, value_str(r->uri));

    return (List*)list;
}

static void register_free(Message* self) {
    Register* r = (Register*)self;
    value_free(value_from_dict(r->options));

    free(r);
}

Register* register_new(int64_t request_id, Dict* options, char* uri) {
    Register* r = calloc(1, sizeof(*r));

    r->base.message_type = MESSAGE_TYPE_REGISTER;
    r->base.marshal = register_marshal;
    r->base.free = register_free;
    r->base.parse = register_parse;

    r->request_id = request_id;
    r->options = options;
    r->uri = uri;

    return r;
}

Message* register_parse(const List* val) {
    if (!val || val->len != 4) return NULL;

    int64_t request_id = value_as_int(val->items[1]);
    Dict* options = value_as_dict(val->items[2]);
    char* uri = value_as_str(val->items[3]);

    return (Message*)register_new(request_id, options, uri);
}
