#include "wampproto/messages/call.h"

#include <stdio.h>
#include <stdlib.h>

#include "wampproto/dict.h"
#include "wampproto/value.h"

static List* call_marshal(const Message* self) {
    const Call* r = (const Call*)self;

    int length = 4;
    if (r->args != NULL) length = 5;

    if (r->kwargs != NULL) length = 6;

    Value* v = value_list(length);
    value_list_append(v, value_int(MESSAGE_TYPE_CALL));
    value_list_append(v, value_int(r->request_id));
    value_list_append(v, value_from_dict(r->options));
    value_list_append(v, value_str(r->procedure));

    if (r->args != NULL) value_list_append(v, value_from_list(r->args));

    if (r->kwargs != NULL) {
        if (r->args == NULL) value_list_append(v, value_list(0));

        value_list_append(v, value_from_dict(r->kwargs));
    }

    return (List*)v;
}

static void call_free(Message* self) { free(self); }

Call* call_new(const int64_t request_id, Dict* options, const char* procedure, List* args, Dict* kwargs) {
    Call* r = calloc(1, sizeof(Call));
    r->base.message_type = MESSAGE_TYPE_CALL;
    r->base.marshal = call_marshal;
    r->base.free = call_free;
    r->base.parse = call_parse;

    r->request_id = request_id;
    r->options = options;
    r->procedure = procedure;
    r->args = args;
    r->kwargs = kwargs;
    return r;
}

Message* call_parse(const List* val) {
    if (!val || val->len < 4) return NULL;

    const int64_t request_id = value_as_int(val->items[1]);
    Dict* options = value_as_dict(val->items[2]);
    char* procedure = value_as_str(val->items[3]);

    List* args = (val->len >= 5) ? value_as_list(val->items[4]) : create_list(0);
    Dict* kwargs = (val->len == 6) ? value_as_dict(val->items[5]) : create_dict();

    return (Message*)call_new(request_id, options, procedure, args, kwargs);
}
