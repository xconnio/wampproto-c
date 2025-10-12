#include "wampproto/messages/invocation.h"
#include "wampproto/value.h"
#include <stdio.h>

static List *invocation_marshal(const Message *self)
{
    Invocation *invocation = (Invocation *)self;

    int length = 4;
    if (invocation->args != NULL)
        length = 5;

    if (invocation->kwargs != NULL)
        length = 6;

    Value *list = value_list(length);
    value_list_append(list, value_int(MESSAGE_TYPE_INVOCATION));
    value_list_append(list, value_int(invocation->request_id));
    value_list_append(list, value_int(invocation->registration_id));
    value_list_append(list, value_from_dict(invocation->details));

    if (invocation->args != NULL)
        value_list_append(list, value_from_list(invocation->args));

    if (invocation->kwargs != NULL)
    {
        if (invocation->args == NULL)
            value_list_append(list, value_list(0));
        value_list_append(list, value_from_dict(invocation->kwargs));
    }

    return (List *)list;
}

static void invocation_free(Message *self)
{
    Invocation *invocation = (Invocation *)self;

    value_free(value_from_dict(invocation->details));
    value_free(value_from_dict(invocation->kwargs));
    value_free(value_from_list(invocation->args));

    free(invocation);
}

Invocation *invocation_new(int64_t request_id, int64_t registration_id, Dict *details, List *args,
                           Dict *kwargs)
{
    Invocation *invocation = calloc(1, sizeof(*invocation));

    invocation->base.message_type = MESSAGE_TYPE_INVOCATION;
    invocation->base.marshal = invocation_marshal;
    invocation->base.free = invocation_free;
    invocation->base.parse = invocation_parse;

    invocation->request_id = request_id;
    invocation->registration_id = registration_id;
    invocation->details = details;
    invocation->args = args;
    invocation->kwargs = kwargs;

    return invocation;
}

Message *invocation_parse(const List *val)
{

    if (!val || val->len < 4)
        return NULL;

    const int64_t request_id = value_as_int(val->items[1]);
    const int64_t registration_id = value_as_int(val->items[2]);
    Dict *details = value_as_dict(val->items[3]);

    List *args = NULL;
    if (val->len == 5)
        args = value_as_list(val->items[4]);

    Dict *kwargs = NULL;
    if (val->len == 6)
    {
        if (args == NULL)
            args = value_as_list(val->items[4]);

        kwargs = value_as_dict(val->items[5]);
    }

    return (Message *)invocation_new(request_id, registration_id, details, args, kwargs);
}
