#include "wampproto/messages/abort.h"
#include "wampproto/messages/message.h"
#include "wampproto/value.h"
#include <stdio.h>
#include <stdlib.h>

static List *abort_marshal(const Message *self)
{
    Abort *abort = (Abort *)self;
    int length = 3;
    if (abort->args != NULL)
        length = 4;
    if (abort->kwargs != NULL)
        length = 5;

    Value *list = value_list(length);
    value_list_append(list, value_int(MESSAGE_TYPE_ABORT));
    value_list_append(list, value_from_dict(abort->details));
    value_list_append(list, value_str(abort->reason));

    if (abort->args != NULL)
        value_list_append(list, value_from_list(abort->args));

    if (abort->kwargs != NULL)
    {
        if (abort->args == NULL)
            value_list_append(list, value_list(0));
        value_list_append(list, value_from_dict(abort->kwargs));
    }

    return (List *)list;
}

static void abort_free(Message *self)
{
    free(self);
}

Abort *abort_new(Dict *details, char *reason, List *args, Dict *kwargs)
{
    Abort *abort = calloc(1, sizeof(*abort));
    abort->base.message_type = MESSAGE_TYPE_ABORT;
    abort->base.free = abort_free;
    abort->base.marshal = abort_marshal;
    abort->base.parse = abort_parse;

    abort->details = details;
    abort->reason = reason;
    abort->args = args;
    abort->kwargs = kwargs;

    return abort;
}

Message *abort_parse(const List *val)
{

    Dict *details = value_as_dict(val->items[1]);
    char *reason = value_as_str(val->items[2]);
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

    return (Message *)abort_new(details, reason, args, kwargs);
}
