#include <stdio.h>
#include <stdlib.h>

#include "wampproto/messages/call.h"
#include "wampproto/value.h"

static List *call_marshal(const Message *self)
{
    const Call *r = (const Call *)self;

    int length = 4;
    if (r->args != NULL)
        length = 5;

    if (r->kwargs != NULL)
        length = 6;

    Value *v = value_list(length);
    value_list_append(v, value_int(MESSAGE_TYPE_CALL));
    value_list_append(v, value_int(r->request_id));
    value_list_append(v, value_from_dict(r->options));
    value_list_append(v, value_str(r->procedure));

    if (r->args != NULL)
        value_list_append(v, value_from_list(r->args));

    if (r->kwargs != NULL)
    {
        if (r->args == NULL)
            value_list_append(v, value_list(0));

        value_list_append(v, value_from_dict(r->kwargs));
    }

    return (List *)v;
}

static void call_free(Message *self)
{
    free(self);
}

Call *call_new(const int64_t request_id, Dict *options, char *procedure, List *args, Dict *kwargs)
{
    Call *r = calloc(1, sizeof(Call));
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

Message *call_parse(const List *val)
{
    if (!val || val->len < 4)
        return NULL;

    const int64_t request_id = value_as_int(val->items[1]);
    Dict *options = val->items[2]->dict_val;
    char *procedure = val->items[3]->str_val;

    List *args = NULL;
    if (val->len == 5)
        args = val->items[4]->list_val;

    Dict *kwargs = NULL;
    if (val->len == 6)
    {
        if (args == NULL)
        {
            args = val->items[4]->list_val;
        }

        kwargs = val->items[5]->dict_val;
    }

    return (Message *)call_new(request_id, options, procedure, args, kwargs);
}
