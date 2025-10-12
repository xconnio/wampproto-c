#include "wampproto/messages/yield.h"
#include "wampproto/value.h"
#include <stdlib.h>

static List *yield_marshal(const Message *self)
{
    Yield *yield = (Yield *)self;

    int length = 3;
    if (yield->args != NULL)
        length = 4;
    if (yield->kwargs != NULL)
        length = 5;

    Value *list = value_list(length);
    value_list_append(list, value_int(MESSAGE_TYPE_YIELD));
    value_list_append(list, value_int(yield->request_id));
    value_list_append(list, value_from_dict(yield->options));

    if (yield->args != NULL)
        value_list_append(list, value_from_list(yield->args));

    if (yield->kwargs != NULL)
    {
        if (yield->args == NULL)
            value_list_append(list, value_list(0));

        value_list_append(list, value_from_dict(yield->kwargs));
    }

    return (List *)list;
}

static void yield_free(Message *self)
{
    Yield *yield = (Yield *)self;

    value_free(value_from_dict(yield->options));
    if (yield->args != NULL)
        value_free(value_from_list(yield->args));
    if (yield->kwargs != NULL)
        value_free(value_from_dict(yield->kwargs));

    free(yield);
}

Yield *yield_new(int64_t request_id, Dict *options, List *args, Dict *kwargs)
{

    Yield *yield = calloc(1, sizeof(*yield));
    yield->base.message_type = MESSAGE_TYPE_YIELD;
    yield->base.marshal = yield_marshal;
    yield->base.free = yield_free;
    yield->base.parse = yield_parse;

    yield->request_id = request_id;
    yield->options = options;
    yield->args = args;
    yield->kwargs = kwargs;

    return yield;
}

Message *yield_parse(const List *val)
{

    if (!val || val->len < 3)
        return NULL;

    int64_t request_id = value_as_int(val->items[1]);
    Dict *options = value_as_dict(val->items[2]);

    List *args = NULL;
    if (val->len == 4)
        args = value_as_list(val->items[3]);

    Dict *kwargs = NULL;
    if (val->len == 5)
    {
        if (args == NULL)
            args = value_as_list(val->items[3]);

        kwargs = value_as_dict(val->items[4]);
    }
    return (Message *)yield_new(request_id, options, args, kwargs);
}
