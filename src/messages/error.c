#include "wampproto/messages/error.h"
#include "wampproto/messages/message.h"
#include "wampproto/value.h"
#include <stdlib.h>

static List *error_marshal(const Message *self)
{
    Error *error = (Error *)self;
    int length = 5;
    if (error->args != NULL)
        length = 6;
    if (error->kwargs != NULL)
        length = 7;

    Value *list = value_list(length);
    value_list_append(list, value_int(MESSAGE_TYPE_ERROR));
    value_list_append(list, value_int(error->message_type));
    value_list_append(list, value_int(error->request_id));
    value_list_append(list, value_from_dict(error->details));
    value_list_append(list, value_str(error->uri));

    if (error->args != NULL)
        value_list_append(list, value_from_list(error->args));

    if (error->kwargs != NULL)
    {
        if (error->args == NULL)
            value_list_append(list, value_list(0));
        value_list_append(list, value_from_dict(error->kwargs));
    }

    return (List *)list;
}

static void error_free(Message *self)
{
    free(self);
}

Error *error_new(int64_t message_type, int64_t request_id, Dict *details, char *uri, List *args,
                 Dict *kwargs)
{
    Error *error = calloc(1, sizeof(*error));
    error->base.message_type = MESSAGE_TYPE_ERROR;
    error->base.marshal = error_marshal;
    error->base.free = error_free;
    error->base.parse = error_parse;

    error->message_type = message_type;
    error->request_id = request_id;
    error->details = details;
    error->uri = uri;
    error->args = args;
    error->kwargs = kwargs;

    return error;
}

Message *error_parse(const List *val)
{
    if (!val || val->len < 5)
        return NULL;

    int64_t message_type = value_as_int(val->items[1]);
    int64_t request_id = value_as_int(val->items[2]);
    Dict *details = value_as_dict(val->items[3]);
    char *uri = value_as_str(val->items[4]);

    List *args = NULL;
    if (val->len == 6)
        args = value_as_list(val->items[5]);

    Dict *kwargs = NULL;
    if (val->len == 7)
    {
        if (args == NULL)
            args = value_as_list(val->items[5]);

        kwargs = value_as_dict(val->items[6]);
    }

    return (Message *)error_new(message_type, request_id, details, uri, args, kwargs);
}
